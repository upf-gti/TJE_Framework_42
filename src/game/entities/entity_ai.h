#pragma once

#include "framework/entities/entity_collider.h"

enum AITypes { AI_SHOOTER, AI_BREAKER, AI_GOOD_GUY };

class EntityAI : public EntityCollider {

	float shooting_rate = 5.0f; // Seconds
	float shoot_timer	= 0.0f;

	float walk_speed	= 4.0f;

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
};