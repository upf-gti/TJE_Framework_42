#pragma once

#include "entity_mesh.h"

class EntityCollider : public EntityMesh {

	void getCollisionsWithModel(const Matrix44& m, const Vector3& center, std::vector<sCollisionData>& collisions);

public:
	EntityCollider() {};
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "") :
		EntityMesh(mesh, material, name) {};
	~EntityCollider() {};

	int layer = eCollisionFilter::SCENARIO;

	bool isStatic = true;
	float sphereRadius = 0.75f;
	float playerHeight = 1.4f;

	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, eCollisionFilter filter = ALL);
};
