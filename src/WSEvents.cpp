/**
 * obs-websocket
 * Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>
 * Copyright (C) 2017	Brendan Hagan <https://github.com/haganbmj>
 * Copyright (C) 2018           Fabio Madia <admsha@gmail.com>
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

#include <util/platform.h>

#include <QTimer>
#include <QPushButton>
#include "WSEvents.h"

bool transitionIsCut(obs_source_t *transition)
{
	if (!transition)
		return false;

	return obs_source_get_type(transition) == OBS_SOURCE_TYPE_TRANSITION
			&& string(obs_source_get_id(transition)) ==
			"cut_transition";
}

string nsToTimestamp(uint64_t ns)
{
	uint64_t ms      = ns / (1000 * 1000);
	uint64_t secs    = ms / 1000;
	uint64_t minutes = secs / 60;

	auto hoursPart   = int(minutes / 60);
	auto minutesPart = int(minutes % 60);
	auto secsPart    = int(secs % 60);
	auto msPart      = int(ms % 1000);

	char buff[100];
	sprintf(buff, "%02d:%02d:%02d.%03d", hoursPart, minutesPart, secsPart,
			msPart);

	return string(buff);
}

void *calldata_get_ptr(const calldata_t *data, const char *name)
{
	void *ptr = nullptr;
	calldata_get_ptr(data, name, &ptr);
	return ptr;
}

WSEvents *WSEvents::Instance = nullptr;

WSEvents::WSEvents()
{
	obs_frontend_add_event_callback(WSEvents::FrontendEventHandler, this);

	/*QSpinBox *durationControl = Utils::GetTransitionDurationControl();
	QObject::connect(durationControl,
			QOverload<int>::of(&QSpinBox::valueChanged),
			WSEvents::Instance,
			&WSEvents::TransitionDurationChanged);

	auto *statusTimer = new QTimer();
	QObject::connect(statusTimer, &QTimer::timeout, WSEvents::Instance,
			&WSEvents::StreamStatus);
	QObject::connect(statusTimer, &QTimer::timeout, WSEvents::Instance,
			&WSEvents::Heartbeat);
	// equal to frontend's constant BITRATE_UPDATE_SECONDS
	statusTimer->start(2000);

	QListWidget *sceneList = Utils::GetSceneListControl();
	QObject::connect(sceneList, &QListWidget::currentItemChanged,
			WSEvents::Instance, &WSEvents::SelectedSceneChanged);

	currentScene      = nullptr;
	currentTransition = nullptr;

	QTimer::singleShot(1000, this, &WSEvents::deferredInitOperations);*/

	pulse = false;
	HeartbeatIsActive = false;
	_streamingActive = false;
	_recordingActive = false;

	_streamStarttime = 0;
	_recStarttime    = 0;
}

WSEvents::~WSEvents()
{
	obs_frontend_remove_event_callback(WSEvents::FrontendEventHandler,
			this);
}

void WSEvents::deferredInitOperations()
{
	OBSSourceAutoRelease transition = obs_frontend_get_current_transition();
	connectTransitionSignals(transition);

	OBSSourceAutoRelease scene = obs_frontend_get_current_scene();
	connectSceneSignals(scene);
}

void WSEvents::FrontendEventHandler(enum obs_frontend_event event,
		void *private_data)
{
	auto *owner = static_cast<WSEvents *>(private_data);

	if (!Config::Current()->ServerEnabled)
		return;

	switch(event) {
	case OBS_FRONTEND_EVENT_SCENE_CHANGED:
		owner->OnSceneChange();
		break;
	case OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED:
		owner->OnSceneListChange();
		break;
	case OBS_FRONTEND_EVENT_SCENE_COLLECTION_CHANGED:
		owner->OnSceneCollectionChange();
		break;
	case OBS_FRONTEND_EVENT_SCENE_COLLECTION_LIST_CHANGED:
		owner->OnSceneCollectionListChange();
		break;
	case OBS_FRONTEND_EVENT_TRANSITION_CHANGED:
		owner->OnTransitionChange();
		break;
	case OBS_FRONTEND_EVENT_TRANSITION_LIST_CHANGED:
		owner->OnTransitionListChange();
		break;
	case OBS_FRONTEND_EVENT_PROFILE_CHANGED:
		owner->OnProfileChange();
		break;
	case OBS_FRONTEND_EVENT_PROFILE_LIST_CHANGED:
		owner->OnProfileListChange();
		break;
	case OBS_FRONTEND_EVENT_STREAMING_STARTING:
		owner->OnStreamStarting();
		break;
	case OBS_FRONTEND_EVENT_STREAMING_STARTED:
		owner->_streamingActive = true;
		owner->OnStreamStarted();
		break;
	case OBS_FRONTEND_EVENT_STREAMING_STOPPING:
		owner->OnStreamStopping();
		break;
	case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
		owner->_streamingActive = false;
		owner->OnStreamStopped();
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STARTING:
		owner->OnRecordingStarting();
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STARTED:
		owner->_recordingActive = true;
		owner->OnRecordingStarted();
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STOPPING:
		owner->OnRecordingStopping();
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
		owner->_recordingActive = false;
		owner->OnRecordingStopped();
		break;
	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTING:
		owner->OnReplayStarting();
		break;
	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED:
		owner->OnReplayStarted();
		break;
	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPING:
		owner->OnReplayStopping();
		break;
	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED:
		owner->OnReplayStopped();
		break;
	case OBS_FRONTEND_EVENT_STUDIO_MODE_ENABLED:
		owner->OnStudioModeSwitched(true);
		break;
	case OBS_FRONTEND_EVENT_STUDIO_MODE_DISABLED:
		owner->OnStudioModeSwitched(false);
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		owner->connectSceneSignals(nullptr);
		owner->connectTransitionSignals(nullptr);
		owner->OnExit();
		break;
	default:
		blog(LOG_INFO, "OBS_FRONTEND_EVENT (%d) not handled", event);
	}
}

void WSEvents::broadcastUpdate(const char *updateType,
		obs_data_t *additionalFields)
{
	OBSDataAutoRelease update = obs_data_create();
	obs_data_set_string(update, "update-type", updateType);

	if (_streamingActive)
		obs_data_set_string(update, "stream-timecode", nsToTimestamp
				(os_gettime_ns() - _streamStarttime).c_str());

	if (_recordingActive)
		obs_data_set_string(update, "rec-timecode", nsToTimestamp
				(os_gettime_ns() - _recStarttime).c_str());

	if (additionalFields)
		obs_data_apply(update, additionalFields);

	updatesToSend.emplace_back(update);

	if (Config::Current()->DebugEnabled)
		blog(LOG_DEBUG, "Update << '%s'", obs_data_get_json(update));
}

void WSEvents::connectTransitionSignals(obs_source_t *transition)
{
	signal_handler_t *sh = nullptr;

	if (currentTransition) {
		sh = obs_source_get_signal_handler(currentTransition);
		signal_handler_disconnect(sh, "transition_start",
				OnTransitionBegin, this);
	}

	currentTransition = transition;
	if (currentTransition) {
		if (!transitionIsCut(transition)) {
			currentTransition = transition;

			sh = obs_source_get_signal_handler(currentTransition);
			signal_handler_connect(sh, "transition_start",
					OnTransitionBegin, this);
		} else {
			currentTransition = nullptr;
		}
	}
}

void WSEvents::connectSceneSignals(obs_source_t *scene)
{
	signal_handler_t *sh = nullptr;

	if (currentScene) {
		sh = obs_source_get_signal_handler(currentScene);

		signal_handler_disconnect(sh, "reorder", OnSceneReordered,
				this);
		signal_handler_disconnect(sh, "item_add", OnSceneItemAdd, this);
		signal_handler_disconnect(sh, "item_remove", OnSceneItemDelete,
				this);
		signal_handler_disconnect(sh, "item_visible",
				OnSceneItemVisibilityChanged, this);
	}

	currentScene = scene;
	if (currentScene) {
		// TODO : connect to all scenes, not just the current one.
		sh = obs_source_get_signal_handler(currentScene);
		signal_handler_connect(sh, "reorder", OnSceneReordered, this);
		signal_handler_connect(sh, "item_add", OnSceneItemAdd, this);
		signal_handler_connect(sh, "item_remove", OnSceneItemDelete,
				this);
		signal_handler_connect(sh, "item_visible",
				OnSceneItemVisibilityChanged, this);
	}
}

uint64_t WSEvents::GetStreamingTime()
{
	if (_streamingActive)
		return (os_gettime_ns() - _streamStarttime);
	else
		return 0;
}

string WSEvents::GetStreamingTimecode()
{
	return nsToTimestamp(GetStreamingTime());
}

uint64_t WSEvents::GetRecordingTime()
{
	if (_recordingActive)
		return (os_gettime_ns() - _recStarttime);
	else
		return 0;
}

string WSEvents::GetRecordingTimecode()
{
	return nsToTimestamp(GetRecordingTime());
}

/**
* Indicates a scene change.
*
* @return {String} `scene-name` The new scene.
* @return {Array} `sources` List of sources in the new scene.
*
* @api events
* @name SwitchScenes
* @category scenes
* @since 0.3
*/
void WSEvents::OnSceneChange()
{
	OBSSourceAutoRelease currentScene = obs_frontend_get_current_scene();
	OBSDataArrayAutoRelease sceneItems = Utils::GetSceneItems(currentScene);
	connectSceneSignals(currentScene);

	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_string(data, "scene-name",
			obs_source_get_name(currentScene));
	obs_data_set_array(data, "sources", sceneItems);

	broadcastUpdate("SwitchScenes", data);

	// Dirty fix : OBS blocks signals when swapping scenes in Studio Mode
	// after transition end, so SelectedSceneChanged is never called...
	if (obs_frontend_preview_program_mode_active()) {
		QListWidget *list = Utils::GetSceneListControl();
		SelectedSceneChanged(list->currentItem(), nullptr);
	}
}

/**
 * The scene list has been modified.
 * Scenes have been added, removed, or renamed.
 * 
 * @api events
 * @name ScenesChanged
 * @category scenes
 * @since 0.3
 */
void WSEvents::OnSceneListChange()
{
	broadcastUpdate("ScenesChanged");
}

/**
 * Triggered when switching to another scene collection or when renaming the
 * current scene collection.
 * 
 * @api events
 * @name SceneCollectionChanged
 * @category scenes
 * @since 4.0.0
 */
void WSEvents::OnSceneCollectionChange()
{
	broadcastUpdate("SceneCollectionChanged");

	currentScene      = nullptr;
	currentTransition = nullptr;

	OnTransitionListChange();
	OnTransitionChange();

	OnSceneListChange();
	OnSceneChange();
}

/**
 * Triggered when a scene collection is created, added, renamed, or removed.
 * 
 * @api events
 * @name SceneCollectionListChanged
 * @category scenes
 * @since 4.0.0
 */
void WSEvents::OnSceneCollectionListChange()
{
	broadcastUpdate("SceneCollectionListChanged");
}

/**
 * The active transition has been changed.
 *
 * @return {String} `transition-name` The name of the new active transition.
 *
 * @api events
 * @name SwitchTransition
 * @category transitions
 * @since 4.0.0
 */
void WSEvents::OnTransitionChange()
{
	OBSSourceAutoRelease currentTransition =
			obs_frontend_get_current_transition();
	connectTransitionSignals(currentTransition);

	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_string(data, "transition-name",
			obs_source_get_name(currentTransition));

	broadcastUpdate("SwitchTransition", data);
}

/**
 * The list of available transitions has been modified.
 * Transitions have been added, removed, or renamed.
 *
 * @api events
 * @name TransitionListChanged
 * @category transitions
 * @since 4.0.0
 */
void WSEvents::OnTransitionListChange()
{
	broadcastUpdate("TransitionListChanged");
}

/**
 * Triggered when switching to another profile or when renaming the current
 * profile.
 *
 * @api events
 * @name ProfileChanged
 * @category profiles
 * @since 4.0.0
 */
void WSEvents::OnProfileChange()
{
	broadcastUpdate("ProfileChanged");
}

/**
 * Triggered when a profile is created, added, renamed, or removed.
 *
 * @api events
 * @name ProfileListChanged
 * @category profiles
 * @since 4.0.0
 */
void WSEvents::OnProfileListChange()
{
	broadcastUpdate("ProfileListChanged");
}

/**
 * A request to start streaming has been issued.
 *
 * @return {boolean} `preview-only` Always false (retrocompatibility).
 *
 * @api events
 * @name StreamStarting
 * @category streaming
 * @since 0.3
 */
void WSEvents::OnStreamStarting()
{
	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_bool(data, "preview-only", false);

	broadcastUpdate("StreamStarting", data);
}

/**
 * Streaming started successfully.
 *
 * @api events
 * @name StreamStarted
 * @category streaming
 * @since 0.3
 */
void WSEvents::OnStreamStarted()
{
	_streamStarttime = os_gettime_ns();
	_lastBytesSent   = 0;
	broadcastUpdate("StreamStarted");
}

/**
 * A request to stop streaming has been issued.
 * 
 * @return {boolean} `preview-only` Always false (retrocompatibility).
 *
 * @api events
 * @name StreamStopping
 * @category streaming
 * @since 0.3
 */
void WSEvents::OnStreamStopping()
{
	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_bool(data, "preview-only", false);

	broadcastUpdate("StreamStopping", data);
}

/**
 * Streaming stopped successfully.
 *
 * @api events
 * @name StreamStopped
 * @category streaming
 * @since 0.3
 */
void WSEvents::OnStreamStopped()
{
	_streamStarttime = 0;
	broadcastUpdate("StreamStopped");
}

/**
 * A request to start recording has been issued.
 *
 * @api events
 * @name RecordingStarting
 * @category recording
 * @since 0.3
 */
void WSEvents::OnRecordingStarting()
{
	broadcastUpdate("RecordingStarting");
}

/**
 * Recording started successfully.
 *
 * @api events
 * @name RecordingStarted
 * @category recording
 * @since 0.3
 */
void WSEvents::OnRecordingStarted()
{
	_recStarttime = os_gettime_ns();
	broadcastUpdate("RecordingStarted");
}

/**
 * A request to stop recording has been issued.
 *
 * @api events
 * @name RecordingStopping
 * @category recording
 * @since 0.3
 */
void WSEvents::OnRecordingStopping()
{
	broadcastUpdate("RecordingStopping");
}

/**
 * Recording stopped successfully.
 *
 * @api events
 * @name RecordingStopped
 * @category recording
 * @since 0.3
 */
void WSEvents::OnRecordingStopped()
{
	_recStarttime = 0;
	broadcastUpdate("RecordingStopped");
}

/**
* A request to start the replay buffer has been issued.
*
* @api events
* @name ReplayStarting
* @category replay buffer
* @since 4.2.0
*/
void WSEvents::OnReplayStarting()
{
	broadcastUpdate("ReplayStarting");
}

/**
* Replay Buffer started successfully
*
* @api events
* @name ReplayStarted
* @category replay buffer
* @since 4.2.0
*/
void WSEvents::OnReplayStarted()
{
	broadcastUpdate("ReplayStarted");
}

/**
* A request to start the replay buffer has been issued.
*
* @api events
* @name ReplayStopping
* @category replay buffer
* @since 4.2.0
*/
void WSEvents::OnReplayStopping()
{
	broadcastUpdate("ReplayStopping");
}

/**
* Replay Buffer stopped successfully
*
* @api events
* @name ReplayStopped
* @category replay buffer
* @since 4.2.0
*/
void WSEvents::OnReplayStopped()
{
	broadcastUpdate("ReplayStopped");
}

/**
 * OBS is exiting.
 *
 * @api events
 * @name Exiting
 * @category other
 * @since 0.3
 */
void WSEvents::OnExit()
{
	broadcastUpdate("Exiting");
}

/**
 * Emit every 2 seconds.
 *
 * @return {boolean} `streaming` Current streaming state.
 * @return {boolean} `recording` Current recording state.
 * @return {boolean} `preview-only` Always false (retrocompatibility).
 * @return {int} `bytes-per-sec` Amount of data per second (in bytes)
 * transmitted by the stream encoder.
 * @return {int} `kbits-per-sec` Amount of data per second (in kilobits)
 * transmitted by the stream encoder.
 * @return {double} `strain` Percentage of dropped frames.
 * @return {int} `total-stream-time` Total time (in seconds) since the stream
 * started.
 * @return {int} `num-total-frames` Total number of frames transmitted since
 * the stream started.
 * @return {int} `num-dropped-frames` Number of frames dropped by the encoder
 * since the stream started.
 * @return {double} `fps` Current framerate.
 *
 * @api events
 * @name StreamStatus
 * @category streaming
 * @since 0.3
 */
void WSEvents::StreamStatus()
{
	bool streamingActive = obs_frontend_streaming_active();
	bool recordingActive = obs_frontend_recording_active();

	OBSOutputAutoRelease streamOutput = obs_frontend_get_streaming_output();

	if (!streamOutput || !streamingActive)
		return;

	uint64_t bytesSent     = obs_output_get_total_bytes(streamOutput);
	uint64_t bytesSentTime = os_gettime_ns();

	if (bytesSent < _lastBytesSent)
		bytesSent = 0;

	if (bytesSent == 0)
		_lastBytesSent = 0;

	uint64_t bytesBetween = bytesSent - _lastBytesSent;
	uint64_t bytesPerSec = bytesBetween /
			((bytesSentTime - _lastBytesSentTime) / 1000000000);

	_lastBytesSent     = bytesSent;
	_lastBytesSentTime = bytesSentTime;

	uint64_t totalStreamTime = (os_gettime_ns() - _streamStarttime) /
			1000000000;

	int totalFrames   = obs_output_get_total_frames(streamOutput);
	int droppedFrames = obs_output_get_frames_dropped(streamOutput);

	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_bool(data, "streaming", streamingActive);
	obs_data_set_bool(data, "recording", recordingActive);
	obs_data_set_int(data, "bytes-per-sec", (int64_t )bytesPerSec);
	obs_data_set_int(data, "kbits-per-sec",
			(int64_t)(bytesPerSec * 8) / 1024);
	obs_data_set_int(data, "total-stream-time", (int64_t )totalStreamTime);
	obs_data_set_int(data, "num-total-frames", totalFrames);
	obs_data_set_int(data, "num-dropped-frames", droppedFrames);
	obs_data_set_double(data, "fps", obs_get_active_fps());
	obs_data_set_double(data, "strain",
			obs_output_get_congestion(streamOutput));
	// Retrocompat with OBSRemote
	obs_data_set_bool(data, "preview-only", false);

	broadcastUpdate("StreamStatus", data);
}

/**
 * Emitted every 2 seconds after enabling it by calling SetHeartbeat.
 * 
 * @return {boolean} `pulse` Toggles between every JSON meassage as an "I am
 * alive" indicator.
 * @return {string (optional)} `current-profile` Current active profile.
 * @return {string (optional)} `current-scene` Current active scene.
 * @return {boolean (optional)} `streaming` Current streaming state.
 * @return {int (optional)} `total-stream-time` Total time (in seconds) since
 * the stream started.
 * @return {int (optional)} `total-stream-bytes` Total bytes sent since the
 * stream started.
 * @return {int (optional)} `total-stream-frames` Total frames streamed since
 * the stream started.
 * @return {boolean (optional)} `recording` Current recording state.
 * @return {int (optional)} `total-record-time` Total time (in seconds) since
 * recording started.
 * @return {int (optional)} `total-record-bytes` Total bytes recorded since the
 * recording started.
 * @return {int (optional)} `total-record-frames` Total frames recorded since
 * the recording started.
 * 
 * @api events
 * @name Heartbeat
 * @category general
 */
void WSEvents::Heartbeat()
{
	if (!HeartbeatIsActive)
		return;

	bool streamingActive = obs_frontend_streaming_active();
	bool recordingActive = obs_frontend_recording_active();

	OBSDataAutoRelease   data         = obs_data_create();
	OBSOutputAutoRelease recordOutput = obs_frontend_get_recording_output();
	OBSOutputAutoRelease streamOutput = obs_frontend_get_streaming_output();

	pulse = !pulse;
	obs_data_set_bool(data, "pulse", pulse);

	obs_data_set_string(data, "current-profile",
			obs_frontend_get_current_profile());

	OBSSourceAutoRelease currentScene = obs_frontend_get_current_scene();
	obs_data_set_string(data, "current-scene",
			obs_source_get_name(currentScene));

	obs_data_set_bool(data, "streaming", streamingActive);
	if (streamingActive) {
		uint64_t totalStreamTime =
					 (os_gettime_ns() - _streamStarttime) /
							 1000000000;
		obs_data_set_int(data, "total-stream-time", totalStreamTime);
		obs_data_set_int(data, "total-stream-bytes",
				(int64_t)obs_output_get_total_bytes(
						streamOutput));
		obs_data_set_int(data, "total-stream-frames",
				obs_output_get_total_frames(streamOutput));
	}

	obs_data_set_bool(data, "recording", recordingActive);
	if (recordingActive) {
		uint64_t totalRecordTime = (os_gettime_ns() - _recStarttime) /
				1000000000;
		obs_data_set_int(data, "total-record-time", totalRecordTime);
		obs_data_set_int(data, "total-record-bytes",
				(int64_t)obs_output_get_total_bytes(
						recordOutput));
		obs_data_set_int(data, "total-record-frames",
				obs_output_get_total_frames(recordOutput));
	}

	broadcastUpdate("Heartbeat", data);
}

/**
 * The active transition duration has been changed.
 *
 * @return {int} `new-duration` New transition duration.
 *
 * @api events
 * @name TransitionDurationChanged
 * @category transitions
 * @since 4.0.0
 */
void WSEvents::TransitionDurationChanged(int ms)
{
	OBSDataAutoRelease fields = obs_data_create();
	obs_data_set_int(fields, "new-duration", ms);

	broadcastUpdate("TransitionDurationChanged", fields);
}

/**
 * A transition (other than "cut") has begun.
 *
 * @return {String} `name` Transition name.
 * @return {int} `duration` Transition duration (in milliseconds).
 *
 * @api events
 * @name TransitionBegin
 * @category transitions
 * @since 4.0.0
 */
void WSEvents::OnTransitionBegin(void *param, calldata_t *data)
{
	UNUSED_PARAMETER(data);
	auto *instance = static_cast<WSEvents *>(param);

	OBSSourceAutoRelease currentTransition =
					obs_frontend_get_current_transition();

	OBSDataAutoRelease fields = obs_data_create();
	obs_data_set_string(fields, "name",
			obs_source_get_name(currentTransition));
	obs_data_set_int(fields, "duration", Utils::GetTransitionDuration());

	instance->broadcastUpdate("TransitionBegin", fields);
}

/**
 * Scene items have been reordered.
 *
 * @return {String} `name` Name of the scene where items have been reordered.
 * @return {Array} `sources` Array of sources.
 *
 * @api events
 * @name SourceOrderChanged
 * @category sources
 * @since 4.0.0
 */
void WSEvents::OnSceneReordered(void *param, calldata_t *data)
{
	auto *instance = static_cast<WSEvents *>(param);

	obs_scene_t *scene = nullptr;
	calldata_get_ptr(data, "scene", &scene);

	OBSDataAutoRelease fields = Utils::GetSceneData(
			obs_scene_get_source(scene));
	obs_data_set_string(fields, "name",
			obs_source_get_name(obs_scene_get_source(scene)));

	instance->broadcastUpdate("SourceOrderChanged", fields);
}

/**
 * An item has been added to the current scene.
 *
 * @return {String} `scene-name` Name of the scene.
 * @return {String} `item-name` Name of the item added to the scene.
 *
 * @api events
 * @name SceneItemAdded
 * @category sources
 * @since 4.0.0
 */
void WSEvents::OnSceneItemAdd(void *param, calldata_t *data)
{
	auto *instance = static_cast<WSEvents *>(param);

	obs_scene_t *scene = nullptr;
	calldata_get_ptr(data, "scene", &scene);

	obs_sceneitem_t *sceneItem = nullptr;
	calldata_get_ptr(data, "item", &sceneItem);

	OBSDataAutoRelease fields = obs_data_create();
	obs_data_set_string(fields, "scene-name",
			obs_source_get_name(obs_scene_get_source(scene)));
	obs_data_set_string(fields, "item-name", obs_source_get_name(
			obs_sceneitem_get_source(sceneItem)));

	instance->broadcastUpdate("SceneItemAdded", fields);
}

/**
 * An item has been removed from the current scene.
 *
 * @return {String} `scene-name` Name of the scene.
 * @return {String} `item-name` Name of the item removed from the scene.
 *
 * @api events
 * @name SceneItemRemoved
 * @category sources
 * @since 4.0.0
 */
void WSEvents::OnSceneItemDelete(void *param, calldata_t *data)
{
	auto *instance = static_cast<WSEvents *>(param);

	obs_scene_t *scene = nullptr;
	calldata_get_ptr(data, "scene", &scene);

	obs_sceneitem_t *sceneItem = nullptr;
	calldata_get_ptr(data, "item", &sceneItem);

	OBSDataAutoRelease fields = obs_data_create();
	obs_data_set_string(fields, "scene-name",
			obs_source_get_name(obs_scene_get_source(scene)));
	obs_data_set_string(fields, "item-name", obs_source_get_name(
			obs_sceneitem_get_source(sceneItem)));

	instance->broadcastUpdate("SceneItemRemoved", fields);
}

/**
 * An item's visibility has been toggled.
 *
 * @return {String} `scene-name` Name of the scene.
 * @return {String} `item-name` Name of the item in the scene.
 * @return {boolean} `item-visible` New visibility state of the item.
 *
 * @api events
 * @name SceneItemVisibilityChanged
 * @category sources
 * @since 4.0.0
 */
void WSEvents::OnSceneItemVisibilityChanged(void *param, calldata_t *data)
{
	auto *instance = static_cast<WSEvents *>(param);

	obs_scene_t *scene = nullptr;
	calldata_get_ptr(data, "scene", &scene);

	obs_sceneitem_t *sceneItem = nullptr;
	calldata_get_ptr(data, "item", &sceneItem);

	bool visible = false;
	calldata_get_bool(data, "visible", &visible);

	OBSDataAutoRelease fields = obs_data_create();
	obs_data_set_string(fields, "scene-name",
			obs_source_get_name(obs_scene_get_source(scene)));
	obs_data_set_string(fields, "item-name", obs_source_get_name(
			obs_sceneitem_get_source(sceneItem)));
	obs_data_set_bool(fields, "item-visible", visible);

	instance->broadcastUpdate("SceneItemVisibilityChanged", fields);
}

/**
 * The selected preview scene has changed (only available in Studio Mode).
 *
 * @return {String} `scene-name` Name of the scene being previewed.
 * @return {Source|Array} `sources` List of sources composing the scene. Same
 * specification as [`GetCurrentScene`](#getcurrentscene).
 *
 * @api events
 * @name PreviewSceneChanged
 * @category studio mode
 * @since 4.1.0
 */
void WSEvents::SelectedSceneChanged(QListWidgetItem *current,
		QListWidgetItem *prev)
{
	if (obs_frontend_preview_program_mode_active()) {
		OBSScene scene = Utils::SceneListItemToScene(current);
		if (!scene)
			return;

		OBSSource sceneSource = obs_scene_get_source(scene);

		OBSDataAutoRelease data = obs_data_create();
		obs_data_set_string(data, "scene-name",
				obs_source_get_name(sceneSource));
		obs_data_set_array(data, "sources",
				Utils::GetSceneItems(sceneSource));

		broadcastUpdate("PreviewSceneChanged", data);
	}
}

/**
 * Studio Mode has been enabled or disabled.
 *
 * @return {boolean} `new-state` The new enabled state of Studio Mode.
 *
 * @api events
 * @name StudioModeSwitched
 * @category studio mode
 * @since 4.1.0
 */
void WSEvents::OnStudioModeSwitched(bool checked)
{
	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_bool(data, "new-state", checked);

	broadcastUpdate("StudioModeSwitched", data);
}
