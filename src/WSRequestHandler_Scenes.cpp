#include "WSEvents.h"

/**
 * Switch to the specified scene.
 *
 * @param {String} `scene-name` Name of the scene to switch to.
 *
 * @api requests
 * @name SetCurrentScene
 * @category scenes
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleSetCurrentScene(
		WSRequestHandler *req)
{
	if (!req->hasField("scene-name"))
		return req->SendErrorResponse("missing request parameters");

	OBSSourceAutoRelease source = obs_get_source_by_name(
			obs_data_get_string(req->data, "scene-name"));

	if (!source)
		return req->SendErrorResponse("requested scene does not exist");

	obs_frontend_set_current_scene(source);
	return req->SendOKResponse();
}

/**
 * Get the current scene's name and source items.
 * 
 * @return {String} `name` Name of the currently active scene.
 * @return {Source|Array} `sources` Ordered list of the current scene's source
 * items.
 *
 * @api requests
 * @name GetCurrentScene
 * @category scenes
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleGetCurrentScene(
		WSRequestHandler *req)
{
	OBSSourceAutoRelease currentScene = obs_frontend_get_current_scene();
	OBSDataArrayAutoRelease sceneItems = Utils::GetSceneItems(currentScene);

	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_string(data, "name", obs_source_get_name(currentScene));
	obs_data_set_array(data, "sources", sceneItems);

	return req->SendOKResponse(data);
}

/**
 * Get a list of scenes in the currently active profile.
 * 
 * @return {String} `current-scene` Name of the currently active scene.
 * @return {Scene|Array} `scenes` Ordered list of the current profile's scenes
 * (See `[GetCurrentScene](#getcurrentscene)` for more information).
 *
 * @api requests
 * @name GetSceneList
 * @category scenes
 * @since 0.3
 */
OBSDataAutoRelease WSRequestHandler::HandleGetSceneList(WSRequestHandler *req)
{
	OBSSourceAutoRelease currentScene = obs_frontend_get_current_scene();
	OBSDataArrayAutoRelease scenes = Utils::GetScenes();

	OBSDataAutoRelease data = obs_data_create();
	obs_data_set_string(data, "current-scene",
			obs_source_get_name(currentScene));
	obs_data_set_array(data, "scenes", scenes);

	return req->SendOKResponse(data);
}

/**
* Changes the order of scene items in the requested scene.
*
* @param {String (optional)} `scene` Name of the scene to reorder (defaults to
* current).
* @param {Scene|Array} `items` Ordered list of objects with name and/or id
* specified. Id prefered due to uniqueness per scene
* @param {int (optional)} `items[].id` Id of a specific scene item. Unique on a
* scene by scene basis.
* @param {String (optional)} `items[].name` Name of a scene item. Sufficiently
* unique if no scene items share sources within the scene.
*
* @api requests
* @name SetSceneItemOrder
* @category scenes
* @since unreleased
*/
OBSDataAutoRelease WSRequestHandler::HandleSetSceneItemOrder(
		WSRequestHandler *req)
{
	string sceneName = obs_data_get_string(req->data, "scene");
	OBSSourceAutoRelease scene = Utils::GetSceneFromNameOrCurrent(
			sceneName);
	if (!scene)
		return req->SendErrorResponse("requested scene doesn't exist");

	OBSDataArrayAutoRelease items = obs_data_get_array(req->data, "items");
	if (!items)
		return req->SendErrorResponse("sceneItem order not specified");

	size_t count = obs_data_array_count(items);
	vector<obs_sceneitem_t *> newOrder;
	newOrder.reserve(count);
	for (size_t i = 0; i < count; i++) {
		OBSDataAutoRelease item = obs_data_array_item(items, i);
		OBSSceneItemAutoRelease sceneItem = Utils::GetSceneItemFromItem(
				scene, item);
		if (!sceneItem)
			return req->SendErrorResponse(
				"Invalid sceneItem id or name in order");

		for (size_t j = 0; j < i; j++)
			if (sceneItem == newOrder[j]) {
				for (size_t i = 0; i < count; i++)
					obs_sceneitem_release(newOrder[i]);
				return req->SendErrorResponse(
				"Duplicate sceneItem in specified order");
			}

		newOrder.emplace_back(sceneItem);
	}

	bool ret = obs_scene_reorder_items(obs_scene_from_source(scene),
			newOrder.data(), count);
	for (size_t i = 0; i < count; i++)
		obs_sceneitem_release(newOrder[i]);

	if (ret)
		return req->SendOKResponse();

	return req->SendErrorResponse("Invalid sceneItem order");
}
