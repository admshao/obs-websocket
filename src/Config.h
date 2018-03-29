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

#ifndef CONFIG_H
#define CONFIG_H

#include <obs-frontend-api.h>

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>
#include <string>

#define WEBSOCKET_LWS_SERVER_STRING "obs-websocket"
#define WEBSOCKET_LWS_PROTOCOL_NAME "obswebsocketapi"

#define DEFAULT_PORT 4444

#define STREAM_SERVICE_ID "websocket_custom_service"

#define SECTION_NAME "WebsocketAPI"
#define PARAM_ENABLE "ServerEnabled"
#define PARAM_PORT "ServerPort"
#define PARAM_DEBUG "DebugEnabled"
#define PARAM_ALERT "AlertsEnabled"
#define PARAM_AUTHREQUIRED "AuthRequired"
#define PARAM_SECRET "AuthSecret"
#define PARAM_SALT "AuthSalt"

#define REQ_GET_VERSION "GetVersion"
#define REQ_GET_AUTH_REQUIRED "GetAuthRequired"
#define REQ_AUTHENTICATE "Authenticate"

#define SET_HEART_BEAT "SetHeartbeat"

#define SET_FILENAME_FORMATTING "SetFilenameFormatting"
#define GET_FILENAME_FORMATTING "GetFilenameFormatting"

#define SET_CURRENT_SCENE "SetCurrentScene"
#define GET_CURRENT_SCENE "GetCurrentScene"
#define GET_SCENE_LIST "GetSceneList"

#define SET_SCENE_ITEM_ORDER "SetSceneItemOrder"
#define SET_SOURCE_RENDER "SetSourceRender"
#define SET_SCENE_ITEM_RENDER "SetSceneItemRender"
#define SET_SCENE_ITEM_POSITION "SetSceneItemPosition"
#define SET_SCENE_ITEM_TRANSFORM "SetSceneItemTransform"
#define SET_SCENE_ITEM_CROP "SetSceneItemCrop"
#define GET_SCENE_ITEM_PROPERTIES "GetSceneItemProperties"
#define SET_SCENE_ITEM_PROPERTIES "SetSceneItemProperties"
#define DUPLICATE_SCENE_ITEM "DuplicateSceneItem"
#define DELETE_SCENE_ITEM "DeleteSceneItem"
#define RESET_SCENE_ITEM "ResetSceneItem"

#define GET_STREAMING_STATUS "GetStreamingStatus"
#define START_STOP_STREAMING "StartStopStreaming"
#define START_STOP_RECORDING "StartStopRecording"
#define START_STREAMING "StartStreaming"
#define STOP_STREAMING "StopStreaming"
#define START_RECORDING "StartRecording"
#define STOP_RECORDING "StopRecording"

#define START_STOP_REPLAY_BUFFER "StartStopReplayBuffer"
#define START_REPLAY_BUFFER "StartReplayBuffer"
#define STOP_REPLAY_BUFFER "StopReplayBuffer"
#define SAVE_REPLAY_BUFFER "SaveReplayBuffer"

#define SET_RECORDING_FOLDER "SetRecordingFolder"
#define GET_RECORDING_FOLDER "GetRecordingFolder"

#define GET_TRANSITION_LIST "GetTransitionList"
#define GET_CURRENT_TRANSITION "GetCurrentTransition"
#define SET_CURRENT_TRANSITION "SetCurrentTransition"
#define SET_TRANSITION_DURATION "SetTransitionDuration"
#define GET_TRANSITION_DURATION "GetTransitionDuration"

#define SET_VOLUME "SetVolume"
#define GET_VOLUME "GetVolume"
#define TOGGLE_MUTE "ToggleMute"
#define SET_MUTE "SetMute"
#define GET_MUTE "GetMute"
#define SET_SYNC_OFFSET "SetSyncOffset"
#define GET_SYNC_OFFSET "GetSyncOffset"
#define GET_SPECIAL_SOURCES "GetSpecialSources"
#define GET_SOURCES_LIST "GetSourcesList"
#define GET_SOURCE_TYPES_LIST "GetSourceTypesList"
#define GET_SOURCE_SETTINGS "GetSourceSettings"
#define SET_SOURCE_SETTINGS "SetSourceSettings"

#define SET_CURRENT_SCENE_COLLECTION "SetCurrentSceneCollection"
#define GET_CURRENT_SCENE_COLLECTION "GetCurrentSceneCollection"
#define LIST_SCENE_COLLECTIONS "ListSceneCollections"

#define SET_CURRENT_PROFILE "SetCurrentProfile"
#define GET_CURRENT_PROFILE "GetCurrentProfile"
#define LIST_PROFILES "ListProfiles"

#define SET_STREAM_SETTINGS "SetStreamSettings"
#define GET_STREAM_SETTINGS "GetStreamSettings"
#define SAVE_STREAM_SETTINGS "SaveStreamSettings"

#define GET_STUDIO_MODE_STATUS "GetStudioModeStatus"
#define GET_PREVIEW_SCENE "GetPreviewScene"
#define SET_PREVIEW_SCENE "SetPreviewScene"
#define TRANSITION_TO_PROGRAM "TransitionToProgram"
#define ENABLE_STUDIO_MODE "EnableStudioMode"
#define DISABLE_STUDIO_MODE "DisableStudioMode"
#define TOGGLE_STUDIO_MODE "ToggleStudioMode"

#define SET_TEXT_GDI_PLUS_PROPERTIES "SetTextGDIPlusProperties"
#define GET_TEXT_GDI_PLUS_PROPERTIES "GetTextGDIPlusProperties"
#define SET_TEXT_FREETYPE2_PROPERTIES "SetTextFreetype2Properties"
#define GET_TEXT_FREETYPE2_PROPERTIES "GetTextFreetype2Properties"

#define GET_BROWSER_SOURCE_PROPERTIES "GetBrowserSourceProperties"
#define SET_BROWSER_SOURCE_PROPERTIES "SetBrowserSourceProperties"

#define QT_TO_UTF8(str) str.toUtf8().constData()

using namespace std;

class Config {
public:
	Config();
	~Config();

	void Load();
	void Save();

	void SetPassword(string password);
	bool CheckAuth(string userChallenge);
	string GenerateSalt();
	static string GenerateSecret(string password, string salt);

	bool     ServerEnabled;
	uint16_t ServerPort;

	bool DebugEnabled;
	bool AlertsEnabled;

	bool   AuthRequired;
	string Secret;
	string Salt;
	string SessionChallenge;
	bool   SettingsLoaded;

	static Config *Current();

private:
	static Config            *_instance;
	mbedtls_entropy_context  entropy;
	mbedtls_ctr_drbg_context rng;
};

#endif // CONFIG_H