/*
obs-websocket
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>
Copyright (C) 2018   	Fabio Madia <admshao@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <QMainWindow>
#include <QMessageBox>
#include "WSServer.h"
#include "WSEvents.h"

WSServer* WSServer::Instance = nullptr;

void writeData(lws *wsi, const char *messageBuf, size_t sendLength)
{
	if (lws_write(wsi, (unsigned char *) messageBuf + LWS_PRE, sendLength,
			LWS_WRITE_TEXT) < 0) {
		blog(LOG_ERROR, "ERROR writing to socket");
	}
}

void sendData(lws *wsi, obs_data_t *message)
{
	const char *messageText = obs_data_get_json(message);

	if (messageText) {
		size_t sendLength = strlen(messageText);

		char *messageBuf = (char *) malloc(LWS_PRE + sendLength);
		memcpy(messageBuf + LWS_PRE, messageText, sendLength);
		writeData(wsi, messageBuf, sendLength);
		free(messageBuf);
	}
}

int callback_obsapi(struct lws *wsi, enum lws_callback_reasons reason,
		void *user, void *in, size_t len)
{
	WSRequestHandler** userp;
	WSRequestHandler* messageHandler;
	bool sendCallback = false;

	if (user != NULL) {
		userp = (WSRequestHandler**) user;
		messageHandler = *(userp);
	}

	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED: {
		const int IP_SIZE = 50;
		char client_name[IP_SIZE];
		char client_ip[IP_SIZE];
		lws_get_peer_addresses(wsi, lws_get_socket_fd(wsi),
				client_name, IP_SIZE,
				client_ip, IP_SIZE);

		blog(LOG_INFO, "Established connection from %s (%s)",
				client_name, client_ip);

		*userp = new WSRequestHandler();
		(*userp)->ip = client_ip;
		(*userp)->name = client_name;

		obs_frontend_push_ui_translation(obs_module_get_string);
		QString title = tr("OBSWebsocket.NotifyConnect.Title");
		QString msg = tr("OBSWebsocket.NotifyConnect.Message")
				.arg(client_ip);
		obs_frontend_pop_ui_translation();

		Utils::SysTrayNotify(msg, QSystemTrayIcon::Information, title);
	}
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!obsremote_event_handler->updatesToSend.empty()) {
			obs_data_t *message = obsremote_event_handler->
					updatesToSend.front();
			sendData(wsi, message);

			sendCallback = true;
		}

		if (!messageHandler->messagesToSend.empty()) {
			obs_data_t *message = messageHandler->messagesToSend
					.front();
			messageHandler->messagesToSend.pop_front();
			sendData(wsi, message);
			obs_data_release(message);

			sendCallback = true;

		}

		if (sendCallback)
			lws_callback_on_writable(wsi);
		break;
	case LWS_CALLBACK_RECEIVE:
		messageHandler->processIncomingMessage(in, len);
		lws_callback_on_writable(wsi);
		break;
	case LWS_CALLBACK_CLOSED: {
		blog(LOG_INFO, "client %s disconnected from (%s)",
				(*userp)->name.c_str(), (*userp)->ip.c_str());

		obs_frontend_push_ui_translation(obs_module_get_string);
		QString title = tr("OBSWebsocket.NotifyDisconnect.Title");
		QString msg = tr("OBSWebsocket.NotifyDisconnect.Message")
				.arg((*userp)->ip.c_str());
		obs_frontend_pop_ui_translation();

		Utils::SysTrayNotify(msg, QSystemTrayIcon::Information, title);

		delete (*userp);
	}
		break;
	default:
		break;
	}

	return 0;
}

static const struct lws_extension exts[] = {
	{"permessage-deflate", lws_extension_callback_pm_deflate,
				"permessage-deflate"},
	{"deflate-frame", lws_extension_callback_pm_deflate, "deflate_frame"},
	{ NULL, NULL, NULL /* terminator */ }
};

static struct lws_protocols protocols[] = {
	{
		WEBSOCKET_LWS_PROTOCOL_NAME,
		callback_obsapi,
		sizeof(WSRequestHandler*),
		1024,
	},
	{NULL, NULL, 0, 0}
};

void WSServer::Loop()
{
	memset(&info, 0, sizeof(info));
	info.port = Config::Current()->ServerPort;
	info.server_string = WEBSOCKET_LWS_SERVER_STRING;
	info.options |= LWS_SERVER_OPTION_VALIDATE_UTF8;
	info.protocols = protocols;
	info.extensions = exts;
	context = lws_create_context(&info);
	if (context == NULL) {
		blog(LOG_ERROR, "libwebsocket init failed to create context on "
				"TCP port %d", info.port);

		QMainWindow* mainWindow = (QMainWindow*)obs_frontend_get_main_window();

		obs_frontend_push_ui_translation(obs_module_get_string);
		QString title = tr("OBSWebsocket.Server.StartFailed.Title");
		QString msg = tr("OBSWebsocket.Server.StartFailed.Message")
				.arg(info.port);
		obs_frontend_pop_ui_translation();

		QMessageBox::warning(mainWindow, title, msg);
		return;
	}
	blog(LOG_INFO, "server started successfully on TCP port %d", info.port);

	while (enabled || !WSEvents::Instance->updatesToSend.empty()) {
		if (!obsremote_event_handler->updatesToSend.empty()) {
			lws_callback_on_writable_all_protocol(context,
					protocols);
			lws_service(context, 50);
			obs_data_t *message = obsremote_event_handler->
					updatesToSend.front();
			obs_data_release(message);
			obsremote_event_handler->updatesToSend.pop_front();
		} else {
			lws_service(context, 50);
		}
	}

	lws_context_destroy(context);
	blog(LOG_INFO, "server stopped successfully");
}

void WSServer::Start()
{
	if (!th.joinable()) {
		enabled = true;
		th = thread([]() { WSServer::Instance->Loop(); });
	}
}

void WSServer::Stop()
{
	if (th.joinable()) {
		enabled = false;
		th.join();
	}
}
