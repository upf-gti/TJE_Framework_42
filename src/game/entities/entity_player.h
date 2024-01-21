#pragma once

#include "entity_mesh.h"
#include "framework/anim_states.h"

class EntityPlayer : public EntityMesh {

public:
	EntityPlayer() {};
	EntityPlayer(Mesh* mesh, const Material& material, const std::string& name = "");
	~EntityPlayer() {};

	AnimStates anim_states;

	// Vel in move the player
	Vector3 velocity;

	float walk_speed = 1.f;
	float jump_speed = 4.f;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
};