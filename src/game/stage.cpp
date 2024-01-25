#include "stage.h"
#include "game.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/camera.h"
#include "graphics/texture.h"
#include "framework/ui.h"
#include "framework/utils.h"
#include "framework/particles.h"
#include "game/entities/entity_player.h"
#include "game/entities/entity_ui_hud.h"

#include <iomanip>
#include <sstream>

ParticleEmitter emitter;

StageManager* StageManager::instance = nullptr;

/*
	MenuStage
*/

MenuStage::MenuStage()
{
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	background = new EntityGUIElement(
		Vector2(width / 2, height / 2),
		Vector2(width, height),
		Texture::Get("data/textures/gui/menu.png")
	);
}

void MenuStage::render()
{
	Vector4 bgColor = World::get_instance()->bgColor;
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	background->render(World::get_instance()->camera2D);

	// WRITE HERE THE CODE TO RENDER THE "PLAY" AND "EXIT" BUTTONS
	// (You can use "data/textures/gui/playButton.png" and "data/textures/gui/exitButton.png" as source for the images)
	// (The button of the size is in the MenuStage header (.h))
	// ...
}

void MenuStage::onEnter(Stage* previousStage)
{
	SDL_ShowCursor(true);
	Game::instance->mouse_locked = false;

	SDL_SetRelativeMouseMode(SDL_FALSE);
}

/*
	WinStage
*/

// WRITE HERE THE CODE TO IMPLEMENT THE WIN STAGE!
// (You can use "data/textures/gui/you_win.png" as background)
// ...

/*
	PlayStage
*/

PlayStage::PlayStage()
{
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	float margin = 24.f;
	Vector2 wall_hud_size(36.0f, height * 0.5f + 12.f);

	wall_hud_back = new EntityGUIHUD(
		Vector2(width - wall_hud_size.x * 0.5f - margin, wall_hud_size.y * 0.5 + margin),
		wall_hud_size,
		Vector4::BLACK
	);

	margin = 30.f;
	wall_hud_size = Vector2(24.f, height * 0.5f);

	wall_hud = new EntityGUIHUD(
		Vector2(width - wall_hud_size.x * 0.5f - margin, wall_hud_size.y * 0.5 + margin),
		wall_hud_size
	);
}

void PlayStage::onEnter(Stage* previousStage)
{
	//win_countdown = 60.0f;

	// Manage hide/show cursor
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		Game::instance->mouse_locked = true;
	}
}

void PlayStage::render()
{
	World* world = World::get_instance();
	float camera_yaw = world->camera_yaw;
	float camera_pitch = world->camera_pitch;

	Camera* camera = world->camera;

	Vector4 bgColor = world->bgColor;
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// renderSky();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	camera->enable();

	world->render();

	// Render time left to win!

	/*std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << win_countdown;
	std::string countdown_str = stream.str();
	drawText(10, 10, countdown_str, Vector3(1, 0.4, 0.4), 8.0f);*/

	// Render HUD on top of the rest

	/*glClear(GL_DEPTH_BUFFER_BIT);
	wall_hud_back->render(World::get_instance()->camera2D);
	wall_hud->render(World::get_instance()->camera2D);*/
}

void PlayStage::update(float delta_time)
{
	/*win_countdown -= delta_time;

	if (win_countdown <= 0.0f) {
		StageManager::get_instance()->goTo("winStage");
	}*/

	World* world = World::get_instance();
	world->update(delta_time);

	// emitter.update(delta_time);
}

void PlayStage::renderSky()
{
	/*Camera* camera = World::get_instance()->camera;
	Entity* landscape = World::get_instance()->landscape;

	landscape->model.setTranslation(camera->eye);

	glDisable(GL_DEPTH_TEST);

	landscape->render(camera);

	glEnable(GL_DEPTH_TEST);*/
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
	World* world = World::get_instance();

	switch (event.keysym.sym)
	{
		case SDLK_F1: 
			Shader::ReloadAll(); 
			break;
	}
}

void PlayStage::onResize(int width, int height)
{
	World::get_instance()->camera->aspect = width / (float)height;
}
