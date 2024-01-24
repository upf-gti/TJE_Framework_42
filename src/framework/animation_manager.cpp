#include "animation_manager.h"

#include "game/game.h"

void AnimationManager::update(float delta_time)
{
    time += delta_time;

    // Single animation playing..
    if (current_animation)
    {
        current_animation->assignTime(time, playing_loop);

        // Set previous loop in case there's any.. if not, leave action pose
        if (!playing_loop && time >= current_animation->duration && last_animation_loop) {
            playAnimation(last_animation_loop);
        }

        return;
    }

    // Completely in target state
    animation_states[current_state]->assignTime(time);

    // Transitioning to target state
    if (target_state != -1) {

        animation_states[target_state]->assignTime(time);

        transition_counter += delta_time;

        if (transition_counter >= transition_time) {
            current_state = target_state;
            target_state = -1;
            return;
        }

        blendSkeleton(
            &animation_states[current_state]->skeleton,  
            &animation_states[target_state]->skeleton, 
            transition_counter / transition_time,
            &blended_skeleton);
    }
}

void AnimationManager::playAnimation(const char* path, bool loop)
{
    current_animation = Animation::Get(path);
    time = 0.0f;
    playing_loop = loop;

    if (loop) {
        last_animation_loop = path;
    }
}

void AnimationManager::stopAnimation()
{
    current_animation = nullptr;
    last_animation_loop = nullptr;
}

void AnimationManager::addAnimationState(const char* path, int state)
{
    animation_states[state] = Animation::Get(path);
}

void AnimationManager::goToState(int state, float time)
{
    // Stop first any possible animation that is being played..
    stopAnimation();

    if (time == 0.0f) {
        current_state = state;
        return;
    }

    if (target_state == state) {
        return;
    }

    transition_counter = 0.0f;
    transition_time = time;
    target_state = state;
}

Skeleton& AnimationManager::getCurrentSkeleton()
{
    if (target_state != -1) {
        return blended_skeleton;
    }

    return current_animation ? current_animation->skeleton : animation_states[current_state]->skeleton;
}
