#pragma once

#include "framework/entities/entity_collider.h"

class Timer;

enum AITypes { AI_SHOOTER, AI_BREAKER, AI_GOOD_GUY };

#define ENEMY_HEALTH 4u

class EntityAI : public EntityCollider {

	float shooting_rate = 5.0f; // Seconds
	float shoot_timer	= 0.0f;

	float walk_speed	= 1.5f;
	bool  has_collided	= false;

	Timer* attack_timer = nullptr;

	uint8_t	health = ENEMY_HEALTH;

public:
	EntityAI() {};
	EntityAI(Mesh* mesh, const Material& material, uint8_t type = AI_SHOOTER, const std::string& name = "");;

	~EntityAI() {};

	uint8_t type = AI_SHOOTER;

	void update(float delta_time);

	// Behaviours

	float fov = 90.f;

	void lookAtTarget(const Vector3& target, float delta_time);
	void shoot(float delta_time);
	void moveTo(const Vector3& target, float delta_time);

	bool hurt();
};