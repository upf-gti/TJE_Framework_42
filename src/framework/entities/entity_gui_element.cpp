#include "entity_gui_element.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/stage.h"

EntityGUIElement::EntityGUIElement(Vector2 pos, Vector2 size, Texture* texture, eButtonId button_id, const std::string& name)
{
	position = pos;
	this->size = size;
	this->button_id = button_id;

	mesh = new Mesh();
	mesh->createQuad(pos.x, pos.y, size.x, size.y, true);

	material.shader = Shader::Get("data/shaders/basic_gui.vs", "data/shaders/texture.fs");
	material.diffuse = texture;
}

void EntityGUIElement::update(float elapsed_time)
{
	Vector2 mouse_pos = Input::mouse_position;

	if (button_id != NoButton &&
		mouse_pos.x > (position.x - size.x * 0.5f) &&
		mouse_pos.x < (position.x + size.x * 0.5f) &&
		mouse_pos.y >(position.y - size.y * 0.5f) &&
		mouse_pos.y < (position.y + size.y * 0.5f)) {

		material.color = Vector4::RED;

		if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
			StageManager::get_instance()->onButtonPressed(button_id);
		}
	}
	else {
		material.color = Vector4::WHITE;
	}
}

void EntityGUIElement::render(Camera* camera2d)
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
	material.shader->setUniform("u_texture", material.diffuse, 0);
	material.shader->setUniform("u_tiling", 1.0f);
	material.shader->setUniform("u_time", Game::instance->time);

	if (is3D)
	{
		Vector2 _size = size;
		float max_dist = 300.f;
		float dist = clamp(world->camera->eye.distance(pos3d), 0.01f, max_dist);
		_size *= 1.f - dist / max_dist;

		Mesh quad;
		quad.createQuad(position.x, position.y, _size.x, _size.y, true);
		quad.render(GL_TRIANGLES);
	}
	else
	{
		mesh->render(GL_TRIANGLES);
	}

	material.shader->disable();

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void EntityGUIElement::update3D(Vector3 position3d)
{
	pos3d = position3d;

	// Update 3D HUD
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;
	World* world = World::get_instance();

	visible = true;

	Vector3 pos = world->camera->project(position3d, width, height);
	if (pos.z < 0.0f || pos.z > 1.f) {
		visible = false;
	}
	else {
		pos.y = height - pos.y;
		position = Vector2(pos.x, pos.y);
	}
}