#pragma once

#include "framework/includes.h"
#include "framework/framework.h"
#include "world.h"
#include <map>

class EntityGUIElement;
class EntityGUIHUD;

class Stage {

public:

	Stage() {}; 			// Constructor

	virtual void onEnter(Stage* previousStage)  {};
	virtual void onLeave(Stage* nextStage)  {};

	virtual void onButtonPressed(eButtonId buttonId) {}

	virtual void onKeyDown(SDL_KeyboardEvent event)  {};
	virtual void onMouseWheel(SDL_MouseWheelEvent event)  {};
	virtual void onResize(int width, int height) {};
	virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	virtual void onMouseButtonUp(SDL_MouseButtonEvent event) {};

	virtual void render()  {};
	virtual void update(float delta_time)  {};
};

class MenuStage : public Stage {

public:
	MenuStage();

	EntityGUIElement* background = nullptr;

	Vector2 buttonSize = Vector2(200.0f, 60.0f);

	// Methods overwritten from base class
	void render();

	void onEnter(Stage* previousStage);
};

// FILL THE GAPS! WRITE HERE THE CODE TO DEFINE THE WIN STAGE!
// ...

class PlayStage : public Stage {

	float win_countdown = 60.0f; // seconds

public:
	PlayStage();

	EntityGUIHUD* wall_hud = nullptr;
	EntityGUIHUD* wall_hud_back = nullptr;

	// Methods overwritten from base class
	void render();
	void update(float delta_time);

	void renderSky();

	void onEnter(Stage* previousStage);
	void onKeyDown(SDL_KeyboardEvent event);
	void onResize(int width, int height);
};

class StageManager : public Stage {

	static StageManager* instance;

public:

	static StageManager* get_instance() {
		if (instance != nullptr)
			return instance;
		instance = new StageManager();
		return instance;
	}

	Stage* current = nullptr;
	std::map<std::string, Stage*> stages;

	StageManager()
	{
		stages["playStage"] = new PlayStage();
		stages["menuStage"] = new MenuStage();
		// stages["winStage"] = new WinStage();
	}

	void goTo(const std::string& stageName)
	{
		Stage* nextStage = stages[stageName];
		if (!nextStage)
			return;

		if (current) {
			current->onLeave(nextStage);
		}

		nextStage->onEnter(current);

		// assign new current
		current = nextStage;
	}

	void render() { if (current) current->render(); }
	void update(float delta_time) { if (current) current->update(delta_time); }

	void onButtonPressed(eButtonId buttonId) { if (current) current->onButtonPressed(buttonId); }

	void onKeyDown(SDL_KeyboardEvent event) { if (current) current->onKeyDown(event); }
	void onMouseWheel(SDL_MouseWheelEvent event) { if (current) current->onMouseWheel(event); }
	void onMouseButtonDown(SDL_MouseButtonEvent event) { if (current) current->onMouseButtonDown(event); }
	void onMouseButtonUp(SDL_MouseButtonEvent event) { if (current) current->onMouseButtonUp(event); }
	void onResize(int width, int height) { if (current) current->onResize(width, height); }
};