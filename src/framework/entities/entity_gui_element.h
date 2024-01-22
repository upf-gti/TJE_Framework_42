#pragma once

#include "entity_mesh.h"

class EntityGUIElement : public EntityMesh {

public:

	EntityGUIElement() {};
	EntityGUIElement(Vector2 pos, Vector2 size, Texture* texture = nullptr, eButtonId button_id = NoButton, const std::string& name = "");
	~EntityGUIElement() {};

	Vector2 position;
	Vector2 size;

	bool use_pixel_coords = true;
	bool selected = false;
	bool visible = true;

	// 3d hud
	bool is3D = false;
	Vector3 pos3d;

	eButtonId button_id = NoButton;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void update3D(Vector3 position3d);
};