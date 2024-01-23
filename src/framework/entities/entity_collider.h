#pragma once

#include "entity_mesh.h"

class EntityCollider : public EntityMesh {

	void getCollisionsWithModel(const Matrix44& m, const Vector3& center, std::vector<sCollisionData>& collisions);
	
	int layer = eCollisionFilter::SCENARIO;

public:
	EntityCollider() {};
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "") :
		EntityMesh(mesh, material, name) {};
	~EntityCollider() {};

	bool isStatic = true;
	float sphereRadius = 1.0f;
	float playerHeight = 0.0f;

	int getLayer() { return layer; }
	void setLayer(int layer) { this->layer = layer; }

	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, eCollisionFilter filter = ALL);
};
