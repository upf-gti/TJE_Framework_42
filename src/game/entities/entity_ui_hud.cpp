#include "game/entities/entity_ui_hud.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/stage.h"

EntityGUIHUD::EntityGUIHUD(Vector2 pos, Vector2 size, const Vector4& color, const std::string& name)
	: EntityGUIElement(pos, size, nullptr, color, NoButton, name)
{
	material.shader = Shader::Get("data/shaders/basic_gui.vs", "data/shaders/hud_gui.fs");
}

void EntityGUIHUD::render(Camera* camera2d)
{
	if (!visible)
		return;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	material.shader->enable();

	World* world = World::get_instance();
	Matrix44 viewProj = use_pixel_coords ? camera2d->viewprojection_matrix : Matrix44();

	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_viewprojection", viewProj);
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_time", Game::instance->time);
	material.shader->setUniform("u_mask", value);

	mesh->render(GL_TRIANGLES);

	material.shader->disable();

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}