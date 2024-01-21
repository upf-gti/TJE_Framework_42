#include "ui.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "game/world.h"
#include "camera.h"
#include "input.h"

#include <cassert>

bool UI::addButton(Vector2 position, Vector2 size, const char* name)
{
	Vector2 mouse_pos = Input::mouse_position;
	bool isHovered = false;
	bool isClicked = false;

	// Mouse interaction

	if (mouse_pos.x > (position.x - size.x * 0.5f) &&
		mouse_pos.x < (position.x + size.x * 0.5f) &&
		mouse_pos.y >(position.y - size.y * 0.5f) &&
		mouse_pos.y < (position.y + size.y * 0.5f)) {

		isHovered = true;
		isClicked = Input::isMousePressed(SDL_BUTTON_LEFT);
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* shader = Shader::Get("data/shaders/basic_gui.vs", "data/shaders/texture.fs");
	Texture* texture = Texture::Get(name);

	shader->enable();

	shader->setUniform("u_color", isHovered ? Vector4::RED : Vector4::WHITE);
	shader->setUniform("u_model", Matrix44());
	shader->setUniform("u_viewprojection", World::get_instance()->camera2D->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_tiling", 1.f);

	Mesh quad;
	quad.createQuad(position.x, position.y, size.x, size.y, true);
	quad.render(GL_TRIANGLES);

	shader->disable();

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	return isClicked;
}