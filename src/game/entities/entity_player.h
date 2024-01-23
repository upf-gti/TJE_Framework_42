#pragma once

#include "framework/entities/entity_mesh.h"
#include "framework/animation_manager.h"

class EntityPlayer : public EntityMesh {

public:
	EntityPlayer() {};
	EntityPlayer(Mesh* mesh, const Material& material, const std::string& name = "");
	~EntityPlayer() {};

	// AnimationManager anim_states;

	// Vel in move the player
	Vector3 velocity;

	float walk_speed = 2.f;
	float jump_speed = 4.f;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);

	void shoot();
};