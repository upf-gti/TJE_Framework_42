#include "entity_player.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/game.h"
#include "game/world.h"

EntityPlayer::EntityPlayer()
{
	Material player_mat;
	player_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	player_mat.diffuse = Texture::Get("data/meshes/player/survivorMaleB.png");
	player_mat.Ks = Vector3(0.0f);

	name = "player";
	mesh = Mesh::Get("data/meshes/player/player.obj");
	material = player_mat;

	// Make the character animated

	// animated = true;

	if (animated)
	{
		mesh = Mesh::Get("data/meshes/character.MESH");

		player_mat.shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");

		anim.playAnimation("data/animations/idle.skanim");
	}

	setLayer(eCollisionFilter::PLAYER);
}

void EntityPlayer::render(Camera* camera)
{
	if (!mesh) return;

	const Matrix44& globalMatrix = getGlobalMatrix();

	// Compute bounding sphere center in world coords
	Vector3 sphere_center = globalMatrix * mesh->box.center;
	Vector3 halfsize = globalMatrix * mesh->box.halfsize;

	// Discard objects whose bounding sphere is not inside the camera frustum
	if (!camera->testBoxInFrustum(sphere_center, halfsize) ||
		camera->eye.distance(sphere_center) > 5000.0f)
		return;

	if (!material.shader) {
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	}

	// Enable shader
	material.shader->enable();

	// Upload uniforms
	material.shader->setUniform("u_model", globalMatrix);
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_background_color", Vector4(0.1, 0.1, 0.1, 1.f));
	material.shader->setUniform("u_camera_position", camera->eye);
	material.shader->setUniform("u_light_color", Vector3(1.f, 1.f, 1.f));
	material.shader->setUniform("u_light_position", Vector3(50.0f, 100.0f, 0.0f));
	material.shader->setUniform("u_tiling", material.tiling);
	material.shader->setUniform("u_time", Game::instance->time);

	material.shader->setUniform("u_maps", Vector2(!!material.diffuse, !!material.normals));

	if (material.diffuse) material.shader->setUniform("u_texture", material.diffuse, 0);
	if (material.normals) material.shader->setUniform("u_normals_texture", material.normals, 1);

	material.shader->setUniform("u_Ka", material.Ka);
	material.shader->setUniform("u_Kd", material.Kd);
	material.shader->setUniform("u_Ks", material.Ks);

	if(animated)
		mesh->renderAnimated(GL_TRIANGLES, &anim.getCurrentSkeleton());
	else
		mesh->render(GL_TRIANGLES);

	// Disable shader
	material.shader->disable();

	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}
}

void EntityPlayer::update(float delta_time)
{
	// DEBUG --------------
	if (animated) {
		anim.update(delta_time);
	}
	// ---------------------

	if (World::get_instance()->freeCam)
		return;

	// Get the new player velocity

	float camera_yaw = World::get_instance()->camera_yaw;
	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3::UP);
	Vector3 front = mYaw.frontVector();
	Vector3 right = mYaw.rightVector();
	Vector3 position = model.getTranslation();

	Vector3 move_dir;

	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))
		move_dir += front;
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))
		move_dir -= front;

	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT))
		move_dir += right;
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT))
		move_dir -= right;

	float speed_mult = walk_speed;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
		speed_mult *= 3.0f;

	move_dir.normalize();
	move_dir *= speed_mult;

	//float move_speed = move_dir.length();
	//if (move_speed < 0.1f)
	//	anim_states.goToState(ePlayerStates::PLAYER_IDLE, 0.5f);
	//else if (move_speed < 2.0f)
	//	anim_states.goToState(ePlayerStates::PLAYER_WALK, 0.5f);
	//else
	//	anim_states.goToState(ePlayerStates::PLAYER_RUN, 0.5f);

	velocity += move_dir;

	// Check collisions with the world entities

	std::vector<sCollisionData> collisions;

	for (auto e : World::get_instance()->root.children)
	{
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (ec != nullptr)
			ec->getCollisions(position + velocity * delta_time, collisions);
		// else es otra cos que no sea entity collider
	}

	for (const sCollisionData& collision : collisions) {
		
		// If normal is pointing upwards, it means it's a floor collision
		float up_factor = collision.colNormal.dot(Vector3::UP);
		if (up_factor > 0.8) {
			continue;
		}
		
		// Move along wall when colliding
		Vector3 newDir = velocity.dot(collision.colNormal) * collision.colNormal;
		velocity.x -= newDir.x;
		velocity.z -= newDir.z;
	}

	// Update player's position

	position += velocity * delta_time;

	// Decrease velocity when not moving
	velocity.x *= 0.5f;
	velocity.z *= 0.5f;

	model.setTranslation(position);
	model.rotate(camera_yaw, Vector3::UP);

	// Update animation system

	if (animated) {
		anim.update(delta_time);
	}

	// Shoot projectiles

	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE))
		shoot();
}

void EntityPlayer::shoot()
{
	World* world = World::get_instance();

	Vector3 origin = world->camera->eye;
	Vector3 direction = (world->camera->center - origin);

	// Get projectile direction and speed (combined in velocity)

	float speed = 12.f;
	Vector3 velocity = direction * speed;
	
	// Generate entity to shoot

	world->addProjectile(origin, velocity, eCollisionFilter::ENEMY | eCollisionFilter::SCENARIO);
}