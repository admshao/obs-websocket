#include "WSEvents.h"

/**
 * Returns the latest version of the plugin and the API.
 *
 * @return {double} `version` OBSRemote compatible API version. Fixed to 1.1 for
 * retrocompatibility.
 * @return {String} `obs-websocket-version` obs-websocket plugin version.
 * @return {String} `obs-studio-version` OBS Studio program version.
 * @return {String} `available-requests` List of available request types,
 * formatted as a comma-separated list string (e.g. : "Method1,Method2,Method3").
 *
 * @api requests
 * @name GetVersion
 * @category general
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleGetVersion(WSRequestHandler *req)
{
	string obsVersion = Utils::OBSVersionString();

	string requests;
	for (auto it : req->messageMap)
		requests += it.first + ",";
	requests = requests.substr(0, requests.length() - 1);

	OBSDataAutoRelease resp = obs_data_create();
	obs_data_set_string(resp, "obs-websocket-version",
			OBS_WEBSOCKET_VERSION);
	obs_data_set_string(resp, "obs-studio-version", obsVersion.c_str());
	obs_data_set_string(resp, "available-requests", requests.c_str());

	return req->SendOKResponse(resp);
}

/**
 * Tells the client if authentication is required. If so, returns authentication
 * parameters `challenge` and `salt` (see "Authentication" for more information)
 *
 * @return {boolean} `authRequired` Indicates whether authentication is required
 * @return {String (optional)} `challenge`
 * @return {String (optional)} `salt`
 *
 * @api requests
 * @name GetAuthRequired
 * @category general
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleGetAuthRequired(
		WSRequestHandler *req)
{
	bool authRequired = Config::Current()->AuthRequired;

	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_bool(response, "authRequired", authRequired);

	if (authRequired) {
		obs_data_set_string(response, "challenge",
				Config::Current()->SessionChallenge.c_str());
		obs_data_set_string(response, "salt",
				Config::Current()->Salt.c_str());
	}

	return req->SendOKResponse(response);
}

/**
 * Attempt to authenticate the client to the server.
 *
 * @param {String} `auth` Response to the auth challenge (see "Authentication"
 * for more information)
 *
 * @api requests
 * @name Authenticate
 * @category general
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleAuthenticate(WSRequestHandler *req)
{
	if (!req->hasField("auth"))
		return req->SendErrorResponse("missing request parameters");

	string auth = obs_data_get_string(req->data, "auth");
	if (auth.empty())
		return req->SendErrorResponse("auth not specified!");

	if (Config::Current()->CheckAuth(auth)) {
		req->authenticated = true;
		return req->SendOKResponse();
	}

	return req->SendErrorResponse("Authentication Failed.");
}

/**
 * Enable/disable sending of the Heartbeat event
 *
 * @param {boolean} `enable` Starts/Stops emitting heartbeat messages
 *
 * @api requests
 * @name SetHeartbeat
 * @category general
 * @since 4.3.0
 */
OBSDataAutoRelease WSRequestHandler::HandleSetHeartbeat(WSRequestHandler *req)
{
	if (!req->hasField("enable"))
		return req->SendErrorResponse(
				"Heartbeat <enable> parameter missing");

	WSEvents::Instance->HeartbeatIsActive = obs_data_get_bool(req->data,
			"enable");

	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_bool(response, "enable",
			WSEvents::Instance->HeartbeatIsActive);
	return req->SendOKResponse(response);
}

/**
 * Set the filename formatting string
 *
 * @param {String} `filename-formatting` Filename formatting string to set.
 *
 * @api requests
 * @name SetFilenameFormatting
 * @category general
 * @since 4.3.0
 */
OBSDataAutoRelease WSRequestHandler::HandleSetFilenameFormatting(
		WSRequestHandler *req)
{
	if (!req->hasField("filename-formatting"))
		return req->SendErrorResponse(
				"<filename-formatting> parameter missing");

	string filenameFormatting = obs_data_get_string(req->data,
			"filename-formatting");
	if (filenameFormatting.empty())
		return req->SendErrorResponse("invalid request parameters");

	Utils::SetFilenameFormatting(filenameFormatting.c_str());
	return req->SendOKResponse();
}

/**
 * Get the filename formatting string
 *
 * @return {String} `filename-formatting` Current filename formatting string.
 *
 * @api requests
 * @name GetFilenameFormatting
 * @category general
 * @since 4.3.0
 */
OBSDataAutoRelease WSRequestHandler::HandleGetFilenameFormatting(
		WSRequestHandler *req)
{
	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_string(response, "filename-formatting",
			Utils::GetFilenameFormatting());
	return req->SendOKResponse(response);
}
