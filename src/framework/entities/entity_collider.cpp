#include "entity_collider.h"

#include "graphics/mesh.h"

void EntityCollider::getCollisionsWithModel(const Matrix44& m, const Vector3& center, std::vector<sCollisionData>& collisions)
{
	Vector3 colPoint;
	Vector3 colNormal;

	// Check wall collisions
	if (mesh->testSphereCollision(m, center, sphereRadius, colPoint, colNormal)) {
		collisions.push_back({ colPoint, colNormal.normalize(), center.distance(colPoint) });
	}

	// Check grounded
	if (mesh->testRayCollision(m, center, Vector3(0, -1, 0), colPoint, colNormal, playerHeight + 0.15f)) {
		collisions.push_back({ colPoint, colNormal.normalize(), center.distance(colPoint) });
	}
}

void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, eCollisionFilter filter)
{
	Vector3 character_center = target_position + Vector3(0.f, playerHeight, 0.f);

	if (!(layer & filter))
		return;

	if (!isInstanced)
	{
		getCollisionsWithModel(model, character_center, collisions);
	}
	else
	{
		for (int i = 0; i < models.size(); ++i)
		{
			getCollisionsWithModel(models[i], character_center, collisions);
		}
	}
}