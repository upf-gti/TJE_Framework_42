#pragma once

#include "framework/entities/entity_collider.h"

class EntityPlayer : public EntityCollider {

public:
	EntityPlayer();
	~EntityPlayer() {};

	// Vel in move the player
	Vector3 velocity;

	float walk_speed = 2.f;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float delta_time);

	void shoot();
};