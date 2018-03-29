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
#include <QtWidgets/QApplication>
#include "WSServer.h"
#include "WSEvents.h"

QT_USE_NAMESPACE
WSServer *WSServer::Instance = nullptr;

void writeData(lws *wsi, const char *messageBuf, size_t sendLength)
{
	if (lws_write(wsi, (unsigned char *)messageBuf + LWS_PRE, sendLength,
			LWS_WRITE_TEXT) < 0)
		blog(LOG_ERROR, "ERROR writing to socket");
}

void sendData(lws *wsi, obs_data_t *message)
{
	const char *messageText = obs_data_get_json(message);
	if (!messageText)
		return;

	size_t sendLength = strlen(messageText);
	auto *messageBuf = (char *)bmalloc(LWS_PRE + sendLength);
	memcpy(messageBuf + LWS_PRE, messageText, sendLength);
	writeData(wsi, messageBuf, sendLength);
	bfree(messageBuf);
}

int callback_obsapi(struct lws *wsi, enum lws_callback_reasons reason,
		void *user, void *in, size_t len)
{
	WSRequestHandler **userp;
	WSRequestHandler *messageHandler = nullptr;
	bool sendCallback = false;

	if (user != nullptr) {
		userp          = (WSRequestHandler **)user;
		messageHandler = *(userp);
	}

	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED: {
		const int IP_SIZE = 50;
		char client_name[IP_SIZE];
		char client_ip[IP_SIZE];
		lws_get_peer_addresses(wsi, lws_get_socket_fd(wsi), client_name,
				IP_SIZE, client_ip, IP_SIZE);

		blog(LOG_INFO, "Established connection from %s (%s)",
				client_name, client_ip);

		*userp = new WSRequestHandler();
		(*userp)->ip   = client_ip;
		(*userp)->name = client_name;

		obs_frontend_push_ui_translation(obs_module_get_string);
		QString title = QApplication::tr(
				"OBSWebsocket.NotifyConnect.Title");
		QString msg = QApplication::tr(
				"OBSWebsocket.NotifyConnect.Message")
				.arg(QString(client_ip));
		obs_frontend_pop_ui_translation();

		Utils::SysTrayNotify(msg, QSystemTrayIcon::Information, title);
	}
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!WSEvents::Instance->updatesToSend.empty()) {
			OBSDataAutoRelease message = WSEvents::Instance
					->updatesToSend.front();
			sendData(wsi, message);
			sendCallback = true;
		}

		if (!messageHandler->messagesToSend.empty()) {
			OBSDataAutoRelease message = messageHandler
					->messagesToSend.front();
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
		QString title = QApplication::tr(
				"OBSWebsocket.NotifyDisconnect.Title");
		QString msg = QApplication::tr(
				"OBSWebsocket.NotifyDisconnect.Message")
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
	{"deflate-frame", lws_extension_callback_pm_deflate,
			"deflate_frame"},
	{nullptr, nullptr, nullptr /* terminator */ }
};

static struct lws_protocols protocols[] = {
	{
		WEBSOCKET_LWS_PROTOCOL_NAME,
		callback_obsapi,
		sizeof(WSRequestHandler *),
		1024,
	},
	{nullptr, nullptr, 0, 0}
};

void WSServer::Loop()
{
	memset(&info, 0, sizeof(info));
	info.port          = Config::Current()->ServerPort;
	info.server_string = WEBSOCKET_LWS_SERVER_STRING;
	info.options      |= LWS_SERVER_OPTION_VALIDATE_UTF8;
	info.protocols     = protocols;
	info.extensions    = exts;
	context = lws_create_context(&info);
	if (context == nullptr) {
		blog(LOG_ERROR, "libwebsocket init failed to create context on "
				"TCP port %d", info.port);

		auto *mainWindow = (QMainWindow *)
				obs_frontend_get_main_window();

		obs_frontend_push_ui_translation(obs_module_get_string);
		QString title = QApplication::tr(
				"OBSWebsocket.Server.StartFailed.Title");
		QString msg = QApplication::tr(
				"OBSWebsocket.Server.StartFailed.Message")
				.arg(info.port);
		obs_frontend_pop_ui_translation();

		QMessageBox::warning(mainWindow, title, msg);
		return;
	}
	blog(LOG_INFO, "server started successfully on TCP port %d", info.port);

	while (enabled || !WSEvents::Instance->updatesToSend.empty()) {
		if (WSEvents::Instance->updatesToSend.empty()) {
			lws_service(context, 50);
		} else {
			lws_callback_on_writable_all_protocol(context,
					protocols);
			lws_service(context, 50);
			WSEvents::Instance->updatesToSend.pop_front();
		}
	}

	lws_context_destroy(context);
	blog(LOG_INFO, "server stopped successfully");
}

void WSServer::Start()
{
	if (!th.joinable()) {
		enabled = true;
		th      = thread([]() { WSServer::Instance->Loop(); });
	}
}

void WSServer::Stop()
{
	if (th.joinable()) {
		enabled = false;
		th.join();
	}
}
