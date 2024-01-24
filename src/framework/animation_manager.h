#pragma once
#include "framework.h"
#include "animation.h"

enum ePlayerStates { PLAYER_IDLE, PLAYER_WALK, PLAYER_RUN };

class AnimationManager {

public:

	AnimationManager() {};

	void update(float delta_time);

	void playAnimation(const char* path, bool loop = true);
	void stopAnimation();

	void addAnimationState(const char* path, int state);
	void goToState(int state, float time = 0.f);

	Animation* getCurrentAnimation() { return current_animation; };
	Skeleton& getCurrentSkeleton();

private:

	const char* last_animation_loop = nullptr;
	Animation* current_animation = nullptr;
	std::map<int, Animation*> animation_states;

	float time = 0.0f;

	bool playing_loop = true;

	int current_state = -1;
	int target_state = -1;

	Skeleton blended_skeleton;
	
	float transition_counter = 0.f;
	float transition_time = 0.f;
};