#include "framework/camera.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "game/entities/entity_ai.h"
#include "game/entities/entity_player.h"

EntityAI::EntityAI(Mesh* mesh, const Material& material, uint8_t type, const std::string& name) :
	EntityCollider(mesh, material, name) {

	setLayer(eCollisionFilter::ENEMY);

	this->type = type;

	animated = true;

	if (animated)
	{
		this->material.shader = Shader::Get("data/shaders/skinning.vs", material.shader->getFSName().c_str());

		// Set new texture since skanims doesnt support loading mtl..
		if (!mesh->materials.empty())
			this->material.diffuse = mesh->materials.begin()->second.Kd_texture;

		this->mesh = Mesh::Get("data/meshes/character.MESH");

		anim.playAnimation(type == AI_SHOOTER ? 
			"data/animations/crouch.skanim" : "data/animations/walk.skanim");
	}
}

void EntityAI::update(float delta_time)
{
	World* world = World::get_instance();

	if (animated) {
		anim.update(delta_time);
	}

	// If hit, wait for the animation to end..

	if (was_hit) {

		if (hit_timer.update(delta_time)) {
			hit_timer.reset();
			was_hit = false;

			if (has_collided) {
				attack_timer.set(1.3f);
			}
		}

		return;
	}

	// In other case, do each case:

	if (type == AI_SHOOTER)
	{
		shoot(delta_time);
	}
	else if (type == AI_BREAKER)
	{
		if (has_collided) {
			if (attack_timer.update(delta_time)) {
				float new_time = animated ? anim.getCurrentAnimation()->duration : 3.0f;
				attack_timer.set(new_time); // Attack once per second
				World::get_instance()->hitTheWall();
			}

		} else {
			moveTo(world->player->getGlobalMatrix().getTranslation(), delta_time);
		}
	}
	// GOOD GUY
	else
	{
		// Follow A to B path??
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

void EntityAI::moveTo(const Vector3& target, float delta_time)
{
	World* world = World::get_instance();

	// Set rotation first...

	lookAtTarget(target, delta_time);

	// Nothing to do if he's already there..

	if (has_collided)
		return;

	// Get the final pos and check if it's viable to move

	Matrix44 new_transform = model;
	new_transform.translate(walk_speed * delta_time);

	Vector3 center = new_transform.getTranslation();

	for (auto e : World::get_instance()->root.children)
	{
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (!ec || !(ec->getLayer() & eCollisionFilter::WALL))
			continue;

		Vector3 colPoint;
		Vector3 colNormal;

		// Check collisions with WALLS

		if (ec->mesh->testSphereCollision(ec->model, center, sphereRadius, colPoint, colNormal)) {
			has_collided = true;
			if (animated) {
				anim.playAnimation("data/animations/punch.skanim");
				attack_timer.set(1.3f);
			}
			break;
		}
	}

	// Move to the target position!	

	if (!has_collided) {
		model.translate(Vector3(0.0f, 0.0f, walk_speed * delta_time));
	}
}

void EntityAI::onProjectileCollision(const Projectile& p)
{
	anim.playAnimation("data/animations/hit.skanim", false);
	hit_timer.set(anim.getCurrentAnimation()->duration);
	was_hit = true;
}