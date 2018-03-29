#include "WSEvents.h"

/**
 * Toggle recording on or off.
 *
 * @api requests
 * @name StartStopRecording
 * @category recording
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleStartStopRecording(
		WSRequestHandler *req)
{
	if (obs_frontend_recording_active())
		obs_frontend_recording_stop();
	else
		obs_frontend_recording_start();

	return req->SendOKResponse();
}

/**
 * Start recording.
 * Will return an `error` if recording is already active.
 *
 * @api requests
 * @name StartRecording
 * @category recording
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleStartRecording(WSRequestHandler *req)
{
	if (obs_frontend_recording_active())
		return req->SendErrorResponse("recording already active");

	obs_frontend_recording_start();
	return req->SendOKResponse();
}

/**
 * Stop recording.
 * Will return an `error` if recording is not active.
 *
 * @api requests
 * @name StopRecording
 * @category recording
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleStopRecording(WSRequestHandler *req)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
		return req->SendOKResponse();
	}

	return req->SendErrorResponse("recording not active");
}

/**
 * Change the current recording folder.
 *
 * @param {String} `rec-folder` Path of the recording folder.
 *
 * @api requests
 * @name SetRecordingFolder
 * @category recording
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleSetRecordingFolder(
		WSRequestHandler *req)
{
	if (!req->hasField("rec-folder"))
		return req->SendErrorResponse("missing request parameters");

	if (Utils::SetRecordingFolder(
			obs_data_get_string(req->data, "rec-folder")))
		return req->SendOKResponse();

	return req->SendErrorResponse("invalid request parameters");
}

/**
 * Get the path of  the current recording folder.
 *
 * @return {String} `rec-folder` Path of the recording folder.
 *
 * @api requests
 * @name GetRecordingFolder
 * @category recording
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleGetRecordingFolder(
		WSRequestHandler *req)
{
	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_string(response, "rec-folder",
			Utils::GetRecordingFolder());
	return req->SendOKResponse(response);
}
