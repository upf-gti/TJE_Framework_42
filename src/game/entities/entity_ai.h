#pragma once

#include "framework/entities/entity_collider.h"
#include "framework/utils.h"
#include "game/world.h"

class Timer;

enum AITypes { AI_SHOOTER, AI_BREAKER, AI_FLYING };

#define ENEMY_HEALTH 2u

class EntityAI : public EntityCollider {

	float walk_speed	= 1.5f;
	bool  has_collided	= false;
	bool  was_hit		= false;

	Timer hit_timer;

	uint8_t	health = ENEMY_HEALTH;

public:
	EntityAI() {};
	EntityAI(Mesh* mesh, const Material& material, uint8_t type = AI_SHOOTER, const std::string& name = "");;

	~EntityAI() {};

	uint8_t type = AI_SHOOTER;
	
	float fov = 90.f;

	void update(float delta_time);

	// Behaviours

	void lookAtTarget(const Vector3& target, float delta_time);
	void moveTo(const Vector3& target, float delta_time);

	void shoot();
	bool onProjectileCollision(const Projectile& p);
	bool doDamage();
};