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

#include "framework/entities/entity_gui_element.h"

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

	root2d.addChild(background);
}

void MenuStage::render()
{
	Vector4 bgColor = World::get_instance()->bgColor;
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	root2d.render(World::get_instance()->camera2D);

	float width = (float)Game::instance->window_width;
	float margin = 24.0f;
	float posY = 500.0f;

	if (UI::addButton(Vector2(width * 0.5f - buttonSize.x * 0.5 - margin, posY), buttonSize, "data/textures/gui/playButton.png"))
	{
		StageManager::get_instance()->goTo("playStage");
	}

	if (UI::addButton(Vector2(width * 0.5f + buttonSize.x * 0.5f + margin, posY), buttonSize, "data/textures/gui/exitButton.png"))
	{
		Game::instance->must_exit = true;
	}
}

void MenuStage::update(float delta_time)
{
	root2d.update(delta_time);
}

// In case of using UIEntities for buttons use this..
void MenuStage::onButtonPressed(eButtonId buttonId)
{
	switch (buttonId) {
	case PlayButton:
		// ...
		break;
	case OptionsButton:
		// ...
		break;
	case ExitButton:
		// ...
		break;
	default:
		break;
	}
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

WinStage::WinStage()
{
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	background = new EntityGUIElement(
		Vector2(width / 2, height / 2),
		Vector2(width, height),
		Texture::Get("data/textures/gui/you_win.png")
	);

	root2d.addChild(background);
}

void WinStage::render()
{
	Vector4 bgColor = World::get_instance()->bgColor;
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	root2d.render(World::get_instance()->camera2D);
}

void WinStage::update(float delta_time)
{
	root2d.update(delta_time);
	restart_countdown -= delta_time;

	if (restart_countdown <= 0.0f) {
		StageManager::get_instance()->goTo("menuStage");
	}
}

// In case of using UIEntities for buttons use this..
void WinStage::onButtonPressed(eButtonId buttonId)
{
	switch (buttonId) {
	case PlayButton:
		// ...
		break;
	case OptionsButton:
		// ...
		break;
	case ExitButton:
		// ...
		break;
	default:
		break;
	}
}

void WinStage::onEnter(Stage* previousStage)
{
	restart_countdown = 4.0f; // seconds

	SDL_SetRelativeMouseMode(SDL_FALSE);
}

/*
	PlayStage
*/

PlayStage::PlayStage()
{
	pixelate = new RenderToTexture();
	pixelate->create(Game::instance->window_width, Game::instance->window_height);

	vignetting = new RenderToTexture();
	vignetting->create(Game::instance->window_width, Game::instance->window_height);
}

void PlayStage::onEnter(Stage* previousStage)
{
	win_countdown = 60.0f;

	bool freeCam = World::get_instance()->freeCam;
	SDL_ShowCursor(freeCam);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	Game::instance->mouse_locked = !freeCam;
}

void PlayStage::render()
{
	World* world = World::get_instance();
	bool freeCam = world->freeCam;
	float camera_yaw = world->camera_yaw;
	float camera_pitch = world->camera_pitch;

	Camera* camera = world->camera;

	pixelate->enable();

	Vector4 bgColor = world->bgColor;
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderSky();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	camera->enable();

	world->render();

	pixelate->disable();

	glDisable(GL_DEPTH_TEST);

	vignetting->enable();

	pixelate->toViewport(Shader::Get("data/shaders/screen.vs", "data/shaders/pixelate.fs"));

	vignetting->disable();

	vignetting->toViewport(Shader::Get("data/shaders/screen.vs", "data/shaders/vignetting.fs"));

	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << win_countdown;
	std::string countdown_str = stream.str();

	drawText(10, 10, countdown_str, Vector3(1, 0.4, 0.4), 8.0f);

	glEnable(GL_DEPTH_TEST);

	// Render map in different viewport
	// renderMinimap();
}

void PlayStage::update(float delta_time)
{
	win_countdown -= delta_time;

	if (win_countdown <= 0.0f) {
		StageManager::get_instance()->goTo("winStage");
	}

	World* world = World::get_instance();
	world->update(delta_time);

	emitter.update(delta_time);
}

void PlayStage::renderMinimap()
{
	// Clear depth buffer again to render UI
	glClear(GL_DEPTH_BUFFER_BIT);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	// update map viewport
	int size = 150;
	int margin = 10;
	glViewport(width - size - margin, height - size - margin, size, size);

	World* world = World::get_instance();

	Camera cam;
	cam.setPerspective(60.f, 1.f, 0.1, 100.f);

	Entity* current_player = world->player;

	Matrix44 mYaw;
	mYaw.setRotation(world->camera_yaw, Vector3::UP);
	Vector3 eye = current_player->model.getTranslation();
	eye.y = 0; // Don't use height
	Vector3 center = eye;
	Vector3 up(0, 0, 1);

	cam.lookAt(
		eye + Vector3(0, 25, 0),
		center,
		mYaw.rotateVector(up)
	);

	cam.enable();
	
	world->root.render(&cam);

	// reset viewport
	glViewport(0, 0, width, height);
}

void PlayStage::renderSky()
{
	Camera* camera = World::get_instance()->camera;
	Entity* landscape = World::get_instance()->landscape;

	landscape->model.setTranslation(camera->eye);

	glDisable(GL_DEPTH_TEST);

	landscape->render(camera);

	glEnable(GL_DEPTH_TEST);
}

void PlayStage::renderPath()
{
	std::vector<Vector3>& waypoints = World::get_instance()->waypoints;
	Camera* camera = World::get_instance()->camera;

	// Must be min 2 points
	if (waypoints.size() < 2)
		return;
	
	Mesh mesh;
	mesh.vertices = waypoints;

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");

	shader->enable();

	// Upload uniforms
	shader->setUniform("u_color", Vector4(1.f, 0.f, 0.f, 1.0));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", Matrix44());

	mesh.render(GL_LINE_STRIP);

	// Disable shader
	shader->disable();
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
	World* world = World::get_instance();

	switch (event.keysym.sym)
	{
		case SDLK_F1: Shader::ReloadAll(); break;
		case SDLK_TAB: 
			world->freeCam = !world->freeCam;
			Game::instance->mouse_locked = !world->freeCam;
			SDL_ShowCursor(world->freeCam); //hide or show the mouse
			break;
	}
}

void PlayStage::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	switch (event.button)
	{
	case SDL_BUTTON_LEFT:
		// ...
		break;
	}
}

void PlayStage::onResize(int width, int height)
{
	World::get_instance()->camera->aspect = width / (float)height;
}
