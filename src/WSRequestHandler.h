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
	bool hasField(OBSDataAutoRelease data, const char *name);
	string ip;
	string name;
	bool authenticated = false;

private:
	static map<string, OBSDataAutoRelease(*)(WSRequestHandler*,
			OBSDataAutoRelease)> messageMap;
	static set<string> authNotRequired;
	deque<OBSDataAutoRelease> messagesToSend;

	OBSDataAutoRelease SendOKResponse(OBSDataAutoRelease ret);
	OBSDataAutoRelease SendErrorResponse(const char *error);

	// General
	static OBSDataAutoRelease HandleGetVersion(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetAuthRequired(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleAuthenticate(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetHeartbeat(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetFilenameFormatting(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetFilenameFormatting(
			WSRequestHandler *req, OBSDataAutoRelease data);

	// Profiles
	static OBSDataAutoRelease HandleSetCurrentProfile(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetCurrentProfile(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleListProfiles(WSRequestHandler *req,
			OBSDataAutoRelease data);

	// Recording
	static OBSDataAutoRelease HandleStartStopRecording(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStartRecording(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStopRecording(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetRecordingFolder(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetRecordingFolder(
			WSRequestHandler *req, OBSDataAutoRelease data);

	// Replay Buffer
	static OBSDataAutoRelease HandleStartStopReplayBuffer(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStartReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStopReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSaveReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease data);

	// Scene Colletions
	static OBSDataAutoRelease HandleSetCurrentSceneCollection(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetCurrentSceneCollection(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleListSceneCollections(
			WSRequestHandler *req, OBSDataAutoRelease data);

	// Scene Items
	static OBSDataAutoRelease HandleGetSceneItemProperties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSceneItemProperties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleResetSceneItem(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSceneItemRender(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSceneItemPosition(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSceneItemTransform(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSceneItemCrop(WSRequestHandler *req,
			OBSDataAutoRelease data);

	// Scenes
	static OBSDataAutoRelease HandleSetCurrentScene(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetCurrentScene(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetSceneList(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSceneItemOrder(WSRequestHandler *req,
			OBSDataAutoRelease data);

	// Sources
	static OBSDataAutoRelease HandleGetSourcesList(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetSourceTypesList(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetVolume(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetVolume(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetMute(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetMute(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleToggleMute(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSyncOffset(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetSyncOffset(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetSourceSettings(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetSourceSettings(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetTextGDIPlusProperties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetTextGDIPlusProperties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetTextFreetype2Properties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetTextFreetype2Properties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetBrowserSourceProperties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetBrowserSourceProperties(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleDeleteSceneItem(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleDuplicateSceneItem(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetSpecialSources(WSRequestHandler *req,
			OBSDataAutoRelease data);

	// Streaming
	static OBSDataAutoRelease HandleGetStreamingStatus(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStartStopStreaming(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStartStreaming(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleStopStreaming(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetStreamSettings(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetStreamSettings(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSaveStreamSettings(
			WSRequestHandler *req, OBSDataAutoRelease data);

	// Studio Mode
	static OBSDataAutoRelease HandleGetStudioModeStatus(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetPreviewScene(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetPreviewScene(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleTransitionToProgram(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleEnableStudioMode(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleDisableStudioMode(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleToggleStudioMode(WSRequestHandler *req,
			OBSDataAutoRelease data);

	// Transitions
	static OBSDataAutoRelease HandleGetTransitionList(WSRequestHandler *req,
			OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetCurrentTransition(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetCurrentTransition(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleSetTransitionDuration(
			WSRequestHandler *req, OBSDataAutoRelease data);
	static OBSDataAutoRelease HandleGetTransitionDuration(
			WSRequestHandler *req, OBSDataAutoRelease data);
};

#endif // WSREQUESTHANDLER_H
