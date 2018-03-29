#include "WSEvents.h"

/**
 * Indicates if Studio Mode is currently enabled.
 *
 * @return {boolean} `studio-mode` Indicates if Studio Mode is enabled.
 *
 * @api requests
 * @name GetStudioModeStatus
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleGetStudioModeStatus(
		WSRequestHandler *req)
{
	OBSDataAutoRelease response = obs_data_create();
	obs_data_set_bool(response, "studio-mode",
			obs_frontend_preview_program_mode_active());

	return req->SendOKResponse(response);
}

/**
 * Get the name of the currently previewed scene and its list of sources.
 * Will return an `error` if Studio Mode is not enabled.
 *
 * @return {String} `name` The name of the active preview scene.
 * @return {Source|Array} `sources`
 *
 * @api requests
 * @name GetPreviewScene
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleGetPreviewScene(
		WSRequestHandler *req)
{
	if (!obs_frontend_preview_program_mode_active())
		return req->SendErrorResponse("studio mode not enabled");

	OBSSourceAutoRelease scene = obs_frontend_get_current_preview_scene();
	OBSDataArrayAutoRelease sceneItems = Utils::GetSceneItems(scene);

	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_string(data, "name", obs_source_get_name(scene));
	obs_data_set_array(data, "sources", sceneItems);

	return req->SendOKResponse(data);
}

/**
 * Set the active preview scene.
 * Will return an `error` if Studio Mode is not enabled.
 *
 * @param {String} `scene-name` The name of the scene to preview.
 *
 * @api requests
 * @name SetPreviewScene
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleSetPreviewScene(
		WSRequestHandler *req)
{
	if (!obs_frontend_preview_program_mode_active())
		return req->SendErrorResponse("studio mode not enabled");

	if (!req->hasField("scene-name"))
		return req->SendErrorResponse("missing request parameters");

	OBSSourceAutoRelease scene = Utils::GetSceneFromNameOrCurrent(
			obs_data_get_string(req->data, "scene-name"));

	if (!scene)
		return req->SendErrorResponse("specified scene doesn't exist");

	obs_frontend_set_current_preview_scene(scene);
	return req->SendOKResponse();
}

/**
 * Transitions the currently previewed scene to the main output.
 * Will return an `error` if Studio Mode is not enabled.
 *
 * @param {Object (optional)} `with-transition` Change the active transition
 * before switching scenes. Defaults to the active transition.
 * @param {String} `with-transition.name` Name of the transition.
 * @param {int (optional)} `with-transition.duration` Transition duration
 * (in milliseconds).
 *
 * @api requests
 * @name TransitionToProgram
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleTransitionToProgram(
		WSRequestHandler *req)
{
	if (!obs_frontend_preview_program_mode_active())
		return req->SendErrorResponse("studio mode not enabled");

	if (req->hasField("with-transition")) {
		OBSDataAutoRelease transitionInfo = obs_data_get_obj(req->data,
				"with-transition");

		if (obs_data_has_user_value(transitionInfo, "name")) {
			string transitionName = obs_data_get_string(
					transitionInfo, "name");
			if (transitionName.empty())
				return req->SendErrorResponse(
						"invalid request parameters");

			if (!Utils::SetTransitionByName(transitionName))
				return req->SendErrorResponse(
					"specified transition doesn't exist");
		}

		if (obs_data_has_user_value(transitionInfo, "duration"))
			Utils::SetTransitionDuration((int)obs_data_get_int(
					transitionInfo, "duration"));
	}

	Utils::TransitionToProgram();
	return req->SendOKResponse();
}

/**
 * Enables Studio Mode.
 *
 * @api requests
 * @name EnableStudioMode
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleEnableStudioMode(
		WSRequestHandler *req)
{
	obs_frontend_set_preview_program_mode(true);
	return req->SendOKResponse();
}

/**
 * Disables Studio Mode.
 *
 * @api requests
 * @name DisableStudioMode
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleDisableStudioMode(
		WSRequestHandler *req)
{
	obs_frontend_set_preview_program_mode(false);
	return req->SendOKResponse();
}

/**
 * Toggles Studio Mode.
 *
 * @api requests
 * @name ToggleStudioMode
 * @category studio mode
 * @since 4.1.0
 */
OBSDataAutoRelease WSRequestHandler::HandleToggleStudioMode(
		WSRequestHandler *req)
{
	obs_frontend_set_preview_program_mode(
			!obs_frontend_preview_program_mode_active());
	return req->SendOKResponse();
}
