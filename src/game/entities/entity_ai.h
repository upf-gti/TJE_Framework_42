#pragma once

#include "framework/entities/entity_collider.h"

enum AITypes { AI_SHOOTER, AI_BREAKER, AI_GOOD_GUY };

class EntityAI : public EntityCollider {

public:
	EntityAI() {};
	EntityAI(Mesh* mesh, const Material& material, const std::string& name = "");;

	~EntityAI() {};

	uint8_t type = AI_SHOOTER;

	float fov = 90.f;

	void update(float delta_time);

	// Behaviours

	float shooting_rate = 5.0f; // Seconds
	float shoot_timer	= 0.0f;

	void lookAtTarget(const Vector3& target, float delta_time);
	void shoot(float delta_time);
};