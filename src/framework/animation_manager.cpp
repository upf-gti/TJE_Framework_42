#include "animation_manager.h"

#include "game/game.h"

void AnimationManager::update(float delta_time)
{
    time += delta_time;

    // Single animation playing, no states..
    if (current_animation)
    {
        // Set previous loop in case there's any.. if not, leave action pose
        if (!playing_loop && time >= (current_animation->duration - transition_time) 
            && last_animation_loop && !target_animation) {
            
            playAnimation(last_animation_loop, true, false);
        }

        current_animation->assignTime(time, playing_loop);
    }

    if (target_animation) {

        float f = target_animation->duration / current_animation->duration;
        if (f < 1.0f) f = 1.0f;

        current_animation->assignTime(time, playing_loop);
        target_animation->assignTime(time * f, playing_loop);

        transition_counter += delta_time;

        if (transition_counter >= transition_time) {
            current_animation = target_animation;
            target_animation = nullptr;
            return;
        }

        blendSkeleton(
            &current_animation->skeleton,
            &target_animation->skeleton,
            transition_counter / transition_time,
            &blended_skeleton);
    }
}

void AnimationManager::playAnimation(const char* path, bool loop, bool reset_time)
{
    if (current_animation) {
        target_animation = Animation::Get(path);
    } else {
        current_animation = Animation::Get(path);
    }

    transition_counter = 0.0f;
    transition_time = 0.35f;
    playing_loop = loop;

    if (reset_time) {
        time = 0.0f;
    }

    if (loop) {
        last_animation_loop = path;
    }
}

void AnimationManager::stopAnimation()
{
    current_animation = nullptr;
    target_animation = nullptr;
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

void AnimationManager::updateStates(float delta_time)
{
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

Skeleton& AnimationManager::getCurrentSkeleton()
{
    if (target_animation || (target_state != -1)) {
        return blended_skeleton;
    }

    if (current_state!= -1) {
        return animation_states[current_state]->skeleton;
    }
    else {
        return current_animation->skeleton;
    }
}
