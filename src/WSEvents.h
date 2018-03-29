/*
obs-websocket
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>
Copyright (C) 2017	Brendan Hagan <https://github.com/haganbmj>
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

#ifndef WSEVENTS_H
#define WSEVENTS_H

#include "obs-websocket.h"
#include "WSServer.h"
#include <QListWidgetItem>

class WSEvents {
public:
	explicit WSEvents();
	~WSEvents();

	bool HeartbeatIsActive;
	deque<OBSDataAutoRelease> updatesToSend;

	static void FrontendEventHandler(enum obs_frontend_event event,
			void *privateData);

	static WSEvents *Instance;

	void connectTransitionSignals(obs_source_t *transition);
	void connectSceneSignals(obs_source_t *scene);

	uint64_t GetStreamingTime();
	string GetStreamingTimecode();
	uint64_t GetRecordingTime();
	string GetRecordingTimecode();

	void deferredInitOperations();
	void StreamStatus();
	void Heartbeat();
	void TransitionDurationChanged(int ms);
	void SelectedSceneChanged(QListWidgetItem *current,
			QListWidgetItem *prev);

private:
	OBSSource currentScene;
	OBSSource currentTransition;

	bool pulse;
	bool _streamingActive;
	bool _recordingActive;

	uint64_t _streamStarttime;
	uint64_t _recStarttime;
	uint64_t _lastBytesSent;
	uint64_t _lastBytesSentTime;

	void broadcastUpdate(const char *updateType,
			obs_data_t *additionalFields = nullptr);

	void OnSceneChange();
	void OnSceneListChange();
	void OnSceneCollectionChange();
	void OnSceneCollectionListChange();

	void OnTransitionChange();
	void OnTransitionListChange();

	void OnProfileChange();
	void OnProfileListChange();

	void OnStreamStarting();
	void OnStreamStarted();
	void OnStreamStopping();
	void OnStreamStopped();

	void OnRecordingStarting();
	void OnRecordingStarted();
	void OnRecordingStopping();
	void OnRecordingStopped();
	void OnReplayStarting();
	void OnReplayStarted();
	void OnReplayStopping();
	void OnReplayStopped();

	void OnStudioModeSwitched(bool enabled);
	void OnExit();

	static void OnTransitionBegin(void *param, calldata_t *data);
	static void OnSceneReordered(void *param, calldata_t *data);
	static void OnSceneItemAdd(void *param, calldata_t *data);
	static void OnSceneItemDelete(void *param, calldata_t *data);
	static void OnSceneItemVisibilityChanged(void *param, calldata_t *data);
};

#endif // WSEVENTS_H