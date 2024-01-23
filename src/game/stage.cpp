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
		Texture::Get("data/textures/gui/ambulance.tga")
	);

	optionsButton = new EntityGUIElement(
		Vector2(width / 2, 300),
		Vector2(200, 60),
		Texture::Get("data/textures/gui/optionsButton.tga"),
		eButtonId::OptionsButton
	);

	exitButton = new EntityGUIElement(
		Vector2(width / 2, 400),
		Vector2(200, 60),
		Texture::Get("data/textures/gui/exitButton.tga"),
		eButtonId::ExitButton
	);

	root2d.addChild(background);
	root2d.addChild(optionsButton);
	root2d.addChild(exitButton);
}

void MenuStage::render()
{
	Vector4 bgColor = World::get_instance()->bgColor;
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	root2d.render(World::get_instance()->camera2D);

	int width = Game::instance->window_width;

	if (UI::addButton(Vector2(width / 2, 200), Vector2(200, 60), "data/textures/gui/playButton.tga"))
	{
		StageManager::get_instance()->goTo("playStage");
	}
}

void MenuStage::update(float delta_time)
{
	root2d.update(delta_time);
}

void MenuStage::onButtonPressed(eButtonId buttonId)
{
	switch (buttonId) {
	case PlayButton:
		StageManager::get_instance()->goTo("playStage");
		break;
	case OptionsButton:
		break;
	case ExitButton:
		Game::instance->must_exit = true;
		break;
	default:
		break;
	}
}

void MenuStage::onEnter(Stage* previousStage)
{
	SDL_ShowCursor(true);
	Game::instance->mouse_locked = false;
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
	Entity* current_player = world->player;

	if (!freeCam) {

		Matrix44 mYaw;
		mYaw.setRotation(camera_yaw, Vector3::UP);
		Matrix44 mPitch;
		mPitch.setRotation(camera_pitch, Vector3(-1, 0, 0));
		Vector3 front = (mPitch * mYaw).frontVector();
		Vector3 eye;
		Vector3 center;

		eye = current_player->getGlobalMatrix() * Vector3(0.f, 3.0f, 0.3f);
		center = eye + front;

		camera->lookAt(
			eye,
			center,
			Vector3::UP
		);
	}

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

	glEnable(GL_DEPTH_TEST);

	// Render map in different viewport
	// renderMinimap();
}

void PlayStage::update(float delta_time)
{
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
