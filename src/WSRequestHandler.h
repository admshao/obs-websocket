/*
obs-websocket
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>
Copyright (C) 2017	Mikhail Swift <https://github.com/mikhailswift>
Copyright (C) 2018	Fabio Madia <admshao@gmail.com>

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

#ifndef WSREQUESTHANDLER_H
#define WSREQUESTHANDLER_H

#include <deque>
#include <unordered_map>
#include <map>
#include <set>
#include <obs-data.h>
#include "obs-websocket.h"
#include "Config.h"
#include "Utils.h"

class WSRequestHandler {
public:
	explicit WSRequestHandler();
	void processIncomingMessage(void *in, size_t len);
	bool hasField(const char *name);

	string ip;
	string name;
	bool authenticated = false;

	typedef map<string, OBSDataAutoRelease(*)(WSRequestHandler*)>
			requestMap;
	static requestMap messageMap;
	static set<string> authNotRequired;

	deque<OBSDataAutoRelease> messagesToSend;
	OBSDataAutoRelease data;

private:
	OBSDataAutoRelease SendOKResponse(OBSDataAutoRelease ret = nullptr);
	OBSDataAutoRelease SendErrorResponse(const char *error,
			OBSDataAutoRelease ret = nullptr);

	// General
	static OBSDataAutoRelease HandleGetVersion(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetAuthRequired(WSRequestHandler *req);
	static OBSDataAutoRelease HandleAuthenticate(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetHeartbeat(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetFilenameFormatting(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetFilenameFormatting(
			WSRequestHandler *req);

	// Profiles
	static OBSDataAutoRelease HandleSetCurrentProfile(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetCurrentProfile(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleListProfiles(WSRequestHandler *req);

	// Recording
	static OBSDataAutoRelease HandleStartStopRecording(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleStartRecording(WSRequestHandler *req);
	static OBSDataAutoRelease HandleStopRecording(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetRecordingFolder(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetRecordingFolder(
			WSRequestHandler *req);

	// Replay Buffer
	static OBSDataAutoRelease HandleStartStopReplayBuffer(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleStartReplayBuffer(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleStopReplayBuffer(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSaveReplayBuffer(WSRequestHandler *req);

	// Scene Colletions
	static OBSDataAutoRelease HandleSetCurrentSceneCollection(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetCurrentSceneCollection(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleListSceneCollections(
			WSRequestHandler *req);

	// Scene Items
	static OBSDataAutoRelease HandleGetSceneItemProperties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSceneItemProperties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleResetSceneItem(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSceneItemRender(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSceneItemPosition(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSceneItemTransform(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSceneItemCrop(WSRequestHandler *req);

	// Scenes
	static OBSDataAutoRelease HandleSetCurrentScene(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetCurrentScene(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetSceneList(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSceneItemOrder(
			WSRequestHandler *req);

	// Sources
	static OBSDataAutoRelease HandleGetSourcesList(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetSourceTypesList(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetVolume(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetVolume(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetMute(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetMute(WSRequestHandler *req);
	static OBSDataAutoRelease HandleToggleMute(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSyncOffset(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetSyncOffset(WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetSourceSettings(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetSourceSettings(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetTextGDIPlusProperties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetTextGDIPlusProperties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetTextFreetype2Properties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetTextFreetype2Properties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetBrowserSourceProperties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetBrowserSourceProperties(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleDeleteSceneItem(WSRequestHandler *req);
	static OBSDataAutoRelease HandleDuplicateSceneItem(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetSpecialSources(
			WSRequestHandler *req);

	// Streaming
	static OBSDataAutoRelease HandleGetStreamingStatus(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleStartStopStreaming(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleStartStreaming(WSRequestHandler *req);
	static OBSDataAutoRelease HandleStopStreaming(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetStreamSettings(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetStreamSettings(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSaveStreamSettings(
			WSRequestHandler *req);

	// Studio Mode
	static OBSDataAutoRelease HandleGetStudioModeStatus(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetPreviewScene(WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetPreviewScene(WSRequestHandler *req);
	static OBSDataAutoRelease HandleTransitionToProgram(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleEnableStudioMode(WSRequestHandler *req);
	static OBSDataAutoRelease HandleDisableStudioMode(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleToggleStudioMode(WSRequestHandler *req);

	// Transitions
	static OBSDataAutoRelease HandleGetTransitionList(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetCurrentTransition(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetCurrentTransition(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleSetTransitionDuration(
			WSRequestHandler *req);
	static OBSDataAutoRelease HandleGetTransitionDuration(
			WSRequestHandler *req);
};

#endif // WSREQUESTHANDLER_H
