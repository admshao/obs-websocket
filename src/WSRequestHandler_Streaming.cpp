#include "WSEvents.h"

/**
* Get current streaming and recording status.
*
* @return {boolean} `streaming` Current streaming status.
* @return {boolean} `recording` Current recording status.
* @return {String (optional)} `stream-timecode` Time elapsed since streaming
* started (only present if currently streaming).
* @return {String (optional)} `rec-timecode` Time elapsed since recording
* started (only present if currently recording).
* @return {boolean} `preview-only` Always false. Retrocompatibility with
* OBSRemote.
*
* @api requests
* @name GetStreamingStatus
* @category streaming
* @since 0.3
*/
OBSDataAutoRelease WSRequestHandler::HandleGetStreamingStatus(
		WSRequestHandler *req)
{
	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_bool(data, "streaming", obs_frontend_streaming_active());
	obs_data_set_bool(data, "recording", obs_frontend_recording_active());
	obs_data_set_bool(data, "preview-only", false);

	if (obs_frontend_streaming_active())
		obs_data_set_string(data, "stream-timecode",
				WSEvents::Instance->GetStreamingTimecode()
						.c_str());

	if (obs_frontend_recording_active())
		obs_data_set_string(data, "rec-timecode",
				WSEvents::Instance->GetRecordingTimecode()
						.c_str());

	return req->SendOKResponse(data);
}

/**
 * Toggle streaming on or off.
 *
 * @api requests
 * @name StartStopStreaming
 * @category streaming
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleStartStopStreaming(
		WSRequestHandler *req)
{
	if (obs_frontend_streaming_active())
		return HandleStopStreaming(req);

	return HandleStartStreaming(req);
}

/**
 * Start streaming.
 * Will return an `error` if streaming is already active.
 *
 * @param {Object (optional)} `stream` Special stream configuration. Please
 * note: these won't be saved to OBS' configuration.
 * @param {String (optional)} `stream.type` If specified ensures the type of
 * stream matches the given type (usually 'rtmp_custom' or 'rtmp_common'). If
 * the currently configured stream type does not match the given stream type,
 * all settings must be specified in the `settings` object or an error will
 * occur when starting the stream.
 * @param {Object (optional)} `stream.metadata` Adds the given object
 * parameters as encoded query string parameters to the 'key' of the RTMP
 * stream. Used to pass data to the RTMP service about the streaming. May be
 * any String, Numeric, or Boolean field.
 * @param {Object (optional)} `stream.settings` Settings for the stream.
 * @param {String (optional)} `stream.settings.server` The publish URL.
 * @param {String (optional)} `stream.settings.key` The publish key of the
 * stream.
 * @param {boolean (optional)} `stream.settings.use-auth` Indicates whether
 * authentication should be used when connecting to the streaming server.
 * @param {String (optional)} `stream.settings.username` If authentication is
 * enabled, the username for the streaming server. Ignored if `use-auth` is not
 * set to `true`.
 * @param {String (optional)} `stream.settings.password` If authentication is
 * enabled, the password for the streaming server. Ignored if `use-auth` is not
 * set to `true`.
 *
 * @api requests
 * @name StartStreaming
 * @category streaming
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleStartStreaming(WSRequestHandler *req)
{
	if (obs_frontend_streaming_active())
		return req->SendErrorResponse("streaming already active");

	OBSService configuredService = obs_frontend_get_streaming_service();
	OBSService newService = nullptr;
	// TODO: fix service memory leak
	if (req->hasField("stream")) {
		OBSDataAutoRelease streamData  = obs_data_get_obj(req->data,
				"stream");
		OBSDataAutoRelease newSettings = obs_data_get_obj(streamData,
				"settings");
		OBSDataAutoRelease newMetadata = obs_data_get_obj(streamData,
				"metadata");
		OBSDataAutoRelease csHotkeys   = obs_hotkeys_save_service(
				configuredService);

		string currentType = obs_service_get_type(configuredService);
		string newType     = obs_data_get_string(streamData, "type");
		if (newType.empty())
			newType = currentType;

		//Supporting adding metadata parameters to key query string
		string query = Utils::ParseDataToQueryString(newMetadata);
		if (!query.empty() && obs_data_has_user_value(newSettings,
				"key")) {
			string key = obs_data_get_string(newSettings, "key");
			if (key.find('?') != string::npos)
				query += '&';
			else
				query += '?';

			query += key;
			obs_data_set_string(newSettings, "key", query.c_str());
		}

		if (newType == currentType) {
			// Service type doesn't change: apply settings to
			// current service

			// By doing this, you can send a request to the
			// websocket that only contains settings you want to
			// change, instead of having to do a get and then
			// change them

			OBSDataAutoRelease currentSettings =
				obs_service_get_settings(configuredService);
			OBSDataAutoRelease updatedSettings = obs_data_create();

			obs_data_apply(updatedSettings, currentSettings);
			obs_data_apply(updatedSettings, newSettings);

			newService = obs_service_create(newType.c_str(),
					STREAM_SERVICE_ID, updatedSettings,
					csHotkeys);
		} else {
			// Service type changed: override service settings
			newService = obs_service_create(newType.c_str(),
					STREAM_SERVICE_ID, newSettings,
					csHotkeys);
		}

		obs_frontend_set_streaming_service(newService);
	}

	obs_frontend_streaming_start();

	// Stream settings provided in StartStreaming are not persisted to disk
	if (newService)
		obs_frontend_set_streaming_service(configuredService);

	return req->SendOKResponse();
}

/**
 * Stop streaming.
 * Will return an `error` if streaming is not active.
 *
 * @api requests
 * @name StopStreaming
 * @category streaming
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleStopStreaming(WSRequestHandler *req)
{
	if (obs_frontend_streaming_active()) {
		obs_frontend_streaming_stop();
		return req->SendOKResponse();
	} else {
		return req->SendErrorResponse("streaming not active");
	}
}

/**
 * Sets one or more attributes of the current streaming server settings. Any
 * options not passed will remain unchanged. Returns the updated settings in
 * response. If 'type' is different than the current streaming service type,
 * all settings are required. Returns the full settings of the stream (the same
 * as GetStreamSettings).
 * 
 * @param {String} `type` The type of streaming service configuration, usually
 * `rtmp_custom` or `rtmp_common`.
 * @param {Object} `settings` The actual settings of the stream.
 * @param {String (optional)} `settings.server` The publish URL.
 * @param {String (optional)} `settings.key` The publish key.
 * @param {boolean (optional)} `settings.use-auth` Indicates whether
 * authentication should be used when connecting to the streaming server.
 * @param {String (optional)} `settings.username` The username for the
 * streaming service.
 * @param {String (optional)} `settings.password` The password for the
 * streaming service.
 * @param {boolean} `save` Persist the settings to disk.
 *
 * @api requests
 * @name SetStreamSettings
 * @category streaming
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleSetStreamSettings(
		WSRequestHandler *req)
{
	OBSService service = obs_frontend_get_streaming_service();
	OBSDataAutoRelease requestSettings = obs_data_get_obj(req->data,
			"settings");
	if (!requestSettings)
		return req->SendErrorResponse("'settings' are required'");

	string serviceType   = obs_service_get_type(service);
	string requestedType = obs_data_get_string(req->data, "type");

	if (requestedType != serviceType) {
		OBSDataAutoRelease hotkeys = obs_hotkeys_save_service(service);
		service = obs_service_create(requestedType.c_str(),
				STREAM_SERVICE_ID, requestSettings, hotkeys);
	} else {
		// If type isn't changing, we should overlay the settings we
		// got to the existing settings. By doing so, you can send a
		// request that only contains the settings you want to change,
		// instead of having to do a get and then change them
		OBSDataAutoRelease existingSettings = obs_service_get_settings(
				service);
		OBSDataAutoRelease newSettings      = obs_data_create();

		// Apply existing settings
		obs_data_apply(newSettings, existingSettings);
		// Then apply the settings from the request
		obs_data_apply(newSettings, requestSettings);

		obs_service_update(service, newSettings);
	}

	//if save is specified we should immediately save the streaming service
	if (obs_data_get_bool(req->data, "save"))
		obs_frontend_save_streaming_service();

	OBSDataAutoRelease serviceSettings = obs_service_get_settings(service);

	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_string(response, "type", requestedType.c_str());
	obs_data_set_obj(response, "settings", serviceSettings);

	return req->SendOKResponse(response);
}

/**
 * Get the current streaming server settings.
 *
 * @return {String} `type` The type of streaming service configuration.
 * Possible values: 'rtmp_custom' or 'rtmp_common'.
 * @return {Object} `settings` Stream settings object.
 * @return {String} `settings.server` The publish URL.
 * @return {String} `settings.key` The publish key of the stream.
 * @return {boolean} `settings.use-auth` Indicates whether audentication should
 * be used when connecting to the streaming server.
 * @return {String} `settings.username` The username to use when accessing the
 * streaming server. Only present if `use-auth` is `true`.
 * @return {String} `settings.password` The password to use when accessing the
 * streaming server. Only present if `use-auth` is `true`.
 *
 * @api requests
 * @name GetStreamSettings
 * @category streaming
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleGetStreamSettings(
		WSRequestHandler *req)
{
	OBSService service = obs_frontend_get_streaming_service();

	string serviceType = obs_service_get_type(service);
	OBSDataAutoRelease settings = obs_service_get_settings(service);

	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_string(response, "type", serviceType.c_str());
	obs_data_set_obj(response, "settings", settings);

	return req->SendOKResponse(response);
}

/**
 * Save the current streaming server settings to disk.
 *
 * @api requests
 * @name SaveStreamSettings
 * @category streaming
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleSaveStreamSettings(
		WSRequestHandler *req)
{
	obs_frontend_save_streaming_service();
	return req->SendOKResponse();
}
