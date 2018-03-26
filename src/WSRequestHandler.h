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

	static void HandleSetCurrentScene(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetCurrentScene(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSceneList(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleSetSceneItemOrder(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetSceneItemRender(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetSceneItemPosition(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetSceneItemTransform(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetSceneItemCrop(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSceneItemProperties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetSceneItemProperties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleDuplicateSceneItem(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleDeleteSceneItem(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleResetSceneItem(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetStreamingStatus(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStartStopStreaming(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStartStopRecording(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStartStreaming(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStopStreaming(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStartRecording(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStopRecording(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStartStopReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStartReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleStopReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSaveReplayBuffer(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetRecordingFolder(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetRecordingFolder(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetTransitionList(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetCurrentTransition(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetCurrentTransition(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetVolume(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleGetVolume(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleToggleMute(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleSetMute(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleGetMute(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleSetSyncOffset(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSyncOffset(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSpecialSources(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSourcesList(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSourceTypesList(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetSourceSettings(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetSourceSettings(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetCurrentSceneCollection(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetCurrentSceneCollection(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleListSceneCollections(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetCurrentProfile(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetCurrentProfile(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleListProfiles(WSRequestHandler *req, OBSDataAutoRelease
	data);

	static void HandleSetStreamSettings(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetStreamSettings(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSaveStreamSettings(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetTransitionDuration(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetTransitionDuration(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetStudioModeStatus(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetPreviewScene(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetPreviewScene(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleTransitionToProgram(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleEnableStudioMode(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleDisableStudioMode(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleToggleStudioMode(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetTextGDIPlusProperties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetTextGDIPlusProperties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetTextFreetype2Properties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetTextFreetype2Properties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleSetBrowserSourceProperties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);

	static void HandleGetBrowserSourceProperties(WSRequestHandler *req,
			OBSDataAutoRelease
			data);
};

#endif // WSPROTOCOL_H
