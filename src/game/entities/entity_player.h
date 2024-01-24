#pragma once

#include "framework/entities/entity_collider.h"
#include "framework/particles.h"

class EntityPlayer : public EntityCollider {

public:
	EntityPlayer();
	~EntityPlayer() {};

	// Vel in move the player
	Vector3 velocity;

	float walk_speed = 2.f;

	const float projectile_respawn_seconds = 0.5f;

	std::vector<Texture*> projectile_textures;
	Mesh* projectile_mesh = nullptr;
	EntityMesh* projectile_to_shoot = nullptr;
	float projectile_charge = 0.0f;
	float projectile_charge_progress = 0.0f;
	float projectile_respawn = projectile_respawn_seconds;

	ParticleEmitter* projectile_particles = nullptr;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float delta_time);

	void shoot();
};