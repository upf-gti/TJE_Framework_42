#pragma once
#include "framework.h"
#include "animation.h"

enum ePlayerStates { PLAYER_IDLE, PLAYER_WALK, PLAYER_RUN };

class AnimStates {

public:

	AnimStates() {};

	void update(float seconds_elapsed);
	void addAnimationState(const char* path, int state);
	void goToState(int state, float time = 0.f);

	Skeleton& getCurrentSkeleton();

private:

	std::map<int, Animation*> animation_states;

	int current_state = -1;
	int target_state = -1;

	Skeleton blended_skeleton;
	
	float transition_counter = 0.f;
	float transition_time = 0.f;
};