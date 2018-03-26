/**
 * obs-websocket
 * Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>
 * Copyright (C) 2017	Mikhail Swift <https://github.com/mikhailswift>
 * Copyright (C) 2018   Fabio Madia <admshao@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>
 */

#include "WSRequestHandler.h"

WSRequestHandler::WSRequestHandler()
{
	authNotRequired.insert({
			REQ_GET_VERSION, REQ_GET_AUTH_REQUIRED, REQ_AUTHENTICATE
	});

	messageMap[REQ_GET_VERSION]       = WSRequestHandler::HandleGetVersion;
	messageMap[REQ_GET_AUTH_REQUIRED] = WSRequestHandler::HandleGetAuthRequired;
	messageMap[REQ_AUTHENTICATE]      = WSRequestHandler::HandleAuthenticate;

	messageMap[SET_HEART_BEAT] = WSRequestHandler::HandleSetHeartbeat;

	messageMap[SET_FILENAME_FORMATTING] = WSRequestHandler::HandleSetFilenameFormatting;
	messageMap[GET_FILENAME_FORMATTING] = WSRequestHandler::HandleGetFilenameFormatting;

	messageMap[SET_CURRENT_SCENE] = WSRequestHandler::HandleSetCurrentScene;
	messageMap[GET_CURRENT_SCENE] = WSRequestHandler::HandleGetCurrentScene;
	messageMap[GET_SCENE_LIST]    = WSRequestHandler::HandleGetSceneList;

	messageMap[SET_SCENE_ITEM_ORDER]      = WSRequestHandler::HandleSetSceneItemOrder;
	messageMap[SET_SOURCE_RENDER]         = WSRequestHandler::HandleSetSceneItemRender;
	messageMap[SET_SCENE_ITEM_RENDER]     = WSRequestHandler::HandleSetSceneItemRender;
	messageMap[SET_SCENE_ITEM_POSITION]   = WSRequestHandler::HandleSetSceneItemPosition;
	messageMap[SET_SCENE_ITEM_TRANSFORM]  = WSRequestHandler::HandleSetSceneItemTransform;
	messageMap[SET_SCENE_ITEM_CROP]       = WSRequestHandler::HandleSetSceneItemCrop;
	messageMap[GET_SCENE_ITEM_PROPERTIES] = WSRequestHandler::HandleGetSceneItemProperties;
	messageMap[SET_SCENE_ITEM_PROPERTIES] = WSRequestHandler::HandleSetSceneItemProperties;
	messageMap[DUPLICATE_SCENE_ITEM]      = WSRequestHandler::HandleDuplicateSceneItem;
	messageMap[DELETE_SCENE_ITEM]         = WSRequestHandler::HandleDeleteSceneItem;
	messageMap[RESET_SCENE_ITEM]          = WSRequestHandler::HandleResetSceneItem;

	messageMap[GET_STREAMING_STATUS] = WSRequestHandler::HandleGetStreamingStatus;
	messageMap[START_STOP_STREAMING] = WSRequestHandler::HandleStartStopStreaming;
	messageMap[START_STOP_RECORDING] = WSRequestHandler::HandleStartStopRecording;
	messageMap[START_STREAMING]      = WSRequestHandler::HandleStartStreaming;
	messageMap[STOP_STREAMING]       = WSRequestHandler::HandleStopStreaming;
	messageMap[START_RECORDING]      = WSRequestHandler::HandleStartRecording;
	messageMap[STOP_RECORDING]       = WSRequestHandler::HandleStopRecording;

	messageMap[START_STOP_REPLAY_BUFFER] = WSRequestHandler::HandleStartStopReplayBuffer;
	messageMap[START_REPLAY_BUFFER]      = WSRequestHandler::HandleStartReplayBuffer;
	messageMap[STOP_REPLAY_BUFFER]       = WSRequestHandler::HandleStopReplayBuffer;
	messageMap[SAVE_REPLAY_BUFFER]       = WSRequestHandler::HandleSaveReplayBuffer;

	messageMap[SET_RECORDING_FOLDER] = WSRequestHandler::HandleSetRecordingFolder;
	messageMap[GET_RECORDING_FOLDER] = WSRequestHandler::HandleGetRecordingFolder;

	messageMap[GET_TRANSITION_LIST]     = WSRequestHandler::HandleGetTransitionList;
	messageMap[GET_CURRENT_TRANSITION]  = WSRequestHandler::HandleGetCurrentTransition;
	messageMap[SET_CURRENT_TRANSITION]  = WSRequestHandler::HandleSetCurrentTransition;
	messageMap[SET_TRANSITION_DURATION] = WSRequestHandler::HandleSetTransitionDuration;
	messageMap[GET_TRANSITION_DURATION] = WSRequestHandler::HandleGetTransitionDuration;

	messageMap[SET_VOLUME]            = WSRequestHandler::HandleSetVolume;
	messageMap[GET_VOLUME]            = WSRequestHandler::HandleGetVolume;
	messageMap[TOGGLE_MUTE]           = WSRequestHandler::HandleToggleMute;
	messageMap[SET_MUTE]              = WSRequestHandler::HandleSetMute;
	messageMap[GET_MUTE]              = WSRequestHandler::HandleGetMute;
	messageMap[SET_SYNC_OFFSET]       = WSRequestHandler::HandleSetSyncOffset;
	messageMap[GET_SYNC_OFFSET]       = WSRequestHandler::HandleGetSyncOffset;
	messageMap[GET_SPECIAL_SOURCES]   = WSRequestHandler::HandleGetSpecialSources;
	messageMap[GET_SOURCES_LIST]      = WSRequestHandler::HandleGetSourcesList;
	messageMap[GET_SOURCE_TYPES_LIST] = WSRequestHandler::HandleGetSourceTypesList;
	messageMap[GET_SOURCE_SETTINGS]   = WSRequestHandler::HandleGetSourceSettings;
	messageMap[SET_SOURCE_SETTINGS]   = WSRequestHandler::HandleSetSourceSettings;

	messageMap[SET_CURRENT_SCENE_COLLECTION] = WSRequestHandler::HandleSetCurrentSceneCollection;
	messageMap[GET_CURRENT_SCENE_COLLECTION] = WSRequestHandler::HandleGetCurrentSceneCollection;
	messageMap[LIST_SCENE_COLLECTIONS]       = WSRequestHandler::HandleListSceneCollections;

	messageMap[SET_CURRENT_PROFILE] = WSRequestHandler::HandleSetCurrentProfile;
	messageMap[GET_CURRENT_PROFILE] = WSRequestHandler::HandleGetCurrentProfile;
	messageMap[LIST_PROFILES]       = WSRequestHandler::HandleListProfiles;

	messageMap[SET_STREAM_SETTINGS]  = WSRequestHandler::HandleSetStreamSettings;
	messageMap[GET_STREAM_SETTINGS]  = WSRequestHandler::HandleGetStreamSettings;
	messageMap[SAVE_STREAM_SETTINGS] = WSRequestHandler::HandleSaveStreamSettings;

	messageMap[GET_STUDIO_MODE_STATUS] = WSRequestHandler::HandleGetStudioModeStatus;
	messageMap[GET_PREVIEW_SCENE]      = WSRequestHandler::HandleGetPreviewScene;
	messageMap[SET_PREVIEW_SCENE]      = WSRequestHandler::HandleSetPreviewScene;
	messageMap[TRANSITION_TO_PROGRAM]  = WSRequestHandler::HandleTransitionToProgram;
	messageMap[ENABLE_STUDIO_MODE]     = WSRequestHandler::HandleEnableStudioMode;
	messageMap[DISABLE_STUDIO_MODE]    = WSRequestHandler::HandleDisableStudioMode;
	messageMap[TOGGLE_STUDIO_MODE]     = WSRequestHandler::HandleToggleStudioMode;

	messageMap[SET_TEXT_GDI_PLUS_PROPERTIES]  = WSRequestHandler::HandleSetTextGDIPlusProperties;
	messageMap[GET_TEXT_GDI_PLUS_PROPERTIES]  = WSRequestHandler::HandleGetTextGDIPlusProperties;
	messageMap[SET_TEXT_FREETYPE2_PROPERTIES] = WSRequestHandler::HandleSetTextFreetype2Properties;
	messageMap[GET_TEXT_FREETYPE2_PROPERTIES] = WSRequestHandler::HandleGetTextFreetype2Properties;

	messageMap[GET_BROWSER_SOURCE_PROPERTIES] = WSRequestHandler::HandleGetBrowserSourceProperties;
	messageMap[SET_BROWSER_SOURCE_PROPERTIES] = WSRequestHandler::HandleSetBrowserSourceProperties;
}

void WSRequestHandler::processIncomingMessage(void *in, size_t len)
{
	OBSDataAutoRelease message = obs_data_create_from_json((char *)in);
	if (!message) {
		blog(LOG_ERROR, "invalid JSON payload received");
		this->messagesToSend.push_back(SendErrorResponse(
				"invalid JSON payload"));
		return;
	}

	const char *type = obs_data_get_string(message, "request-type");
	const char *id   = obs_data_get_string(message, "message-id");
	if (!type || !id) {
		this->messagesToSend.push_back(SendErrorResponse(
				"message type not specified"));
		return;
	}

	if (Config::Current()->DebugEnabled) {
		blog(LOG_DEBUG, "Request >> '%s'", message);
	}

	OBSDataAutoRelease (*handlerFunc)(WSRequestHandler *,
			OBSDataAutoRelease) = (messageMap[type]);
	OBSDataAutoRelease ret = NULL;

	if (handlerFunc) {
		if (!Config::Current()->AuthRequired || this->authenticated ||
				authNotRequired.find(type) !=
						authNotRequired.end())
			ret = handlerFunc(this, message);
		else
			ret = SendErrorResponse("Not Authenticated");
	} else {
		this->messagesToSend.push_back(SendErrorResponse(
				"invalid request type"));
		return;
	}

	if (ret) {
		obs_data_set_string(ret, "message-id", id);
		this->messagesToSend.push_back(ret);
	} else {
		this->messagesToSend.push_back(SendErrorResponse(
				"no response given"));
	}
}

OBSDataAutoRelease WSRequestHandler::SendOKResponse(OBSDataAutoRelease ret)
{
	if (!ret)
		ret = obs_data_create();
	obs_data_set_string(ret, "status", "ok");
	return ret;
}

OBSDataAutoRelease WSRequestHandler::SendErrorResponse(const char *error)
{
	obs_data_t *ret = obs_data_create();
	obs_data_set_string(ret, "status", "error");
	obs_data_set_string(ret, "error", error);
	return ret;
}

bool WSRequestHandler::hasField(OBSDataAutoRelease data, const char* name)
{
    return obs_data_has_user_value(data, name);
}
