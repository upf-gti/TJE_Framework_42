#pragma once

#include "framework/entities/entity_mesh.h"

enum eEnemyStates { ENEMY_IDLE, ENEMY_WALK, ENEMY_RUN, ENEMY_SHOOT };

class EntityEnemy : public EntityMesh {

public:
	EntityEnemy() {};
	EntityEnemy(Mesh* mesh, const Material& material, const std::string& name = "");;

	~EntityEnemy() {};

	// Cone Fov of the enemy in deg
	float fov = 90.f;

	// Methods overwritten from base class
	void update(float elapsed_time);
};