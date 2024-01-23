#include "game.h"

#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/animation.h"
#include "framework/entities/entity.h"

#include "stage.h"

#include <cmath>

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	delta_time = 0.0f;
	mouse_locked = true;

	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	// set initial stage
	StageManager::get_instance()->goTo("menuStage");
}

//what to do when the image has to be draw
void Game::render(void)
{
	StageManager::get_instance()->render();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double delta_time)
{
	StageManager::get_instance()->update(delta_time);
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	}

	StageManager::get_instance()->onKeyDown(event);
}

void Game::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
	}

	StageManager::get_instance()->onMouseButtonDown(event);
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	StageManager::get_instance()->onMouseButtonUp(event);
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	StageManager::get_instance()->onMouseWheel(event);
}

void Game::onResize(int width, int height)
{
	std::cout << "Window resized: " << width << "," << height << std::endl;
	glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;

	StageManager::get_instance()->onResize(width, height);
}

void Game::onKeyUp(SDL_KeyboardEvent event) { }

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event) { }

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event) { }
