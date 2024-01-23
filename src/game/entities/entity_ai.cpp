#include "framework/camera.h"
#include "game/entities/entity_ai.h"
#include "game/entities/entity_player.h"
#include "game/world.h"

EntityAI::EntityAI(Mesh* mesh, const Material& material, const std::string& name) :
	EntityCollider(mesh, material, name) {

	setLayer(eCollisionFilter::ENEMY);
}

void EntityAI::update(float delta_time)
{
	World* world = World::get_instance();

	if (type == AI_SHOOTER)
	{
		shoot(delta_time);
	}
	else if (type == AI_BREAKER)
	{
		
	}
	// GOOD GUY
	else
	{

	}
}

void EntityAI::lookAtTarget(const Vector3& target, float delta_time)
{
	Vector3 origin = model.getTranslation();
	origin.y = 0.f;
	float delta_yaw = model.getYawRotationToAimTo(target);
	model.rotate(delta_yaw * delta_time * 4.0f, Vector3::UP);
}

void EntityAI::shoot(float delta_time)
{
	World* world = World::get_instance();

	Vector3 projectile_offset = Vector3(0.0f, 3.0f, 0.0f);
	Vector3 target = world->player->getGlobalMatrix().getTranslation() + projectile_offset;

	lookAtTarget(target, delta_time);

	// Check if we can shoot ..

	shoot_timer += delta_time;

	if (shoot_timer < shooting_rate)
		return;

	// Shoot!

	shoot_timer = 0.0f;

	Vector3 origin = model.getTranslation() + projectile_offset;
	Vector3 direction = (target - origin);

	// Get projectile direction and speed (combined in velocity)

	float speed = 2.f;
	Vector3 velocity = direction * speed;

	// Generate entity to shoot

	world->addProjectile(origin, velocity, eCollisionFilter::PLAYER | eCollisionFilter::SCENARIO);
}
