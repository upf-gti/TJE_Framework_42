#pragma once

#include "framework/entities/entity_gui_element.h"

class EntityGUIHUD : public EntityGUIElement {

	float value = 1.0f;

public:

	EntityGUIHUD() {};
	EntityGUIHUD(Vector2 pos, Vector2 size, const Vector4& color = Vector4::WHITE, const std::string& name = "");
	~EntityGUIHUD() {};

	// Methods overwritten from base class
	void render(Camera* camera) override;

	void setValue(float value) { this->value = clamp(value, 0.0f, 1.0f); }
};