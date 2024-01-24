#include "framework/camera.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "game/entities/entity_ai.h"
#include "game/entities/entity_player.h"

std::vector<std::string> EntityAI::dudas_audios = {
	"data/sounds/students/ayuda.wav",
	"data/sounds/students/carrera.wav",
	"data/sounds/students/crashea.wav",
	"data/sounds/students/duda.wav",
	"data/sounds/students/puntero.wav",
};

std::vector<std::string> EntityAI::gracias_audios = {
	"data/sounds/students/gracias.wav",
	"data/sounds/students/facil.wav",
};

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
			"data/animations/throw.skanim" : "data/animations/walk.skanim");

		// Add animation callbacks

		anim.addCallback("data/animations/punch.skanim", [&](float t) {
			World::get_instance()->hitTheWall(2);
			World::get_instance()->world_audio_player.play(dudas_audios[rand() % dudas_audios.size()].c_str(), false);
		}, 1.0f); // Using SECONDS as trigger indicator

		anim.addCallback("data/animations/throw.skanim", [&](float t) {
			shoot();
			World::get_instance()->world_audio_player.play(dudas_audios[rand() % dudas_audios.size()].c_str(), false);
		}, 60); // Using KEYFRAMES as trigger indicator

		anim.addCallback("data/animations/death.skanim", [&](float t) {
			World::get_instance()->removeEntity(this);
			World::get_instance()->world_audio_player.play(gracias_audios[rand() % gracias_audios.size()].c_str(), false);
		}, 1.f);
	}

	projectile_mesh = new Mesh();
	projectile_mesh->createQuad(0, 0, 0.25, 0.25, false);
}

EntityAI::~EntityAI()
{
	anim.destroy();
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
		}

		return;
	}

	if (type == AI_SHOOTER)
	{
		World* world = World::get_instance();

		Vector3 projectile_offset = Vector3(0.0f, 3.0f, 0.0f);
		Vector3 target = world->player->getGlobalMatrix().getTranslation() + projectile_offset;

		lookAtTarget(target, delta_time);
	}
	else if (type == AI_BREAKER)
	{
		if (!has_collided) {
			moveTo(world->player->getGlobalMatrix().getTranslation(), delta_time);
		}
	}
	else
	{
		assert(0 && "SELECT AI TYPE!");
	}
}

void EntityAI::lookAtTarget(const Vector3& target, float delta_time)
{
	Vector3 origin = model.getTranslation();
	origin.y = 0.f;
	float delta_yaw = model.getYawRotationToAimTo(target);
	model.rotate(delta_yaw * delta_time * 4.0f, Vector3::UP);
}

void EntityAI::shoot()
{
	World* world = World::get_instance();

	Vector3 target = world->player->getGlobalMatrix().getTranslation() + Vector3(0.0f, 3.0f, 0.0f);

	// Shoot!

	Vector3 offset(-0.8f, 3.0f, 1.1f);
	Vector3 origin = model * offset;
	Vector3 direction = (target - origin);

	// Get projectile direction and speed (combined in velocity)

	float speed = 2.f;
	Vector3 velocity = direction * speed;

	// Generate entity to shoot

	Matrix44 m = model;
	m.translate(offset);

	world->addProjectile(m, velocity, eCollisionFilter::PLAYER | eCollisionFilter::SCENARIO, 
		projectile_mesh, Texture::Get("data/meshes/projectiles/question_mark.png"));
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
	hit_timer.set(anim.getCurrentAnimation()->duration);
	was_hit = true;

	doDamage(p.damage);
}

void EntityAI::doDamage(float damage)
{
	health -= damage;

	bool is_dead = health <= 0.0f;

	anim.playAnimation(is_dead ? "data/animations/death.skanim" : "data/animations/hit.skanim", false);
}