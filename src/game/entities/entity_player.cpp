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
	player_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	player_mat.diffuse = Texture::Get("data/meshes/player/survivorMaleB.png");
	player_mat.Ks = Vector3(0.0f);

	name = "player";
	mesh = Mesh::Get("data/meshes/player/player.obj");
	material = player_mat;

	// initProjectilesStuff();

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

	mesh->render(GL_TRIANGLES);

	// Disable shader
	material.shader->disable();

	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}

	/*if (projectile_respawn == projectile_respawn_seconds) {
		projectile_to_shoot->render(camera);
		World::get_instance()->transparent_entities.push_back(projectile_particles);
	}*/
}

void EntityPlayer::update(float delta_time)
{
	// Get the new player velocity

	float camera_yaw = World::get_instance()->camera_yaw;
	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3::UP);
	Vector3 front = mYaw.frontVector();		// The vector pointing to the front
	Vector3 right = mYaw.rightVector();		// The vector pointing to the right side
	Vector3 position = model.getTranslation();

	Vector3 move_dir;

	// WRITE HERE THE CODE TO MOVE THE PLAYER USING THE INPUT!
	// if (Input::isKeyPressed(SDL_SCANCODE_W)) {
	//		...
	// }
	
	float speed_mult = walk_speed;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
		speed_mult *= 3.0f;

	move_dir.normalize();
	move_dir *= speed_mult;

	velocity += move_dir;

	// Check collisions with the world entities

	//std::vector<sCollisionData> collisions;

	//for (auto e : World::get_instance()->root.children)
	//{
	//	EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
	//	if (ec != nullptr)
	//		ec->getCollisions(position + velocity * delta_time, collisions);
	//}

	//for (const sCollisionData& collision : collisions) {
	//	
	//	// If normal is pointing upwards, it means it's a floor collision
	//	float up_factor = collision.colNormal.dot(Vector3::UP);
	//	if (up_factor > 0.8) {
	//		continue;
	//	}
	//	
	//	// Move along wall when colliding
	//	Vector3 newDir = velocity.dot(collision.colNormal) * collision.colNormal;
	//	velocity.x -= newDir.x;
	//	velocity.z -= newDir.z;
	//}

	// Update player's position

	position += velocity * delta_time;

	// Decrease velocity when not moving
	velocity.x *= 0.5f;
	velocity.z *= 0.5f;

	model.setTranslation(position);
	model.rotate(camera_yaw, Vector3::UP);

	// updateProjectilesStuff();
}

void EntityPlayer::initProjectilesStuff()
{
	/*Material projectile_material;
	projectile_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	projectile_textures = {
		Texture::Get("data/textures/attacks/assembly.png"),
		Texture::Get("data/textures/attacks/assert.png"),
		Texture::Get("data/textures/attacks/javascript.png"),
		Texture::Get("data/textures/attacks/shaders.png"),
		Texture::Get("data/textures/attacks/templates.png"),
		Texture::Get("data/textures/attacks/visualstudio.png"),
		Texture::Get("data/textures/attacks/heap_corruption.png"),
		Texture::Get("data/textures/attacks/exe_not_found.png")
	};

	projectile_mesh = new Mesh();
	projectile_mesh->createQuad(0, 0, 0.25, 0.25, false);

	projectile_to_shoot = new EntityMesh(projectile_mesh, projectile_material, "projectile");
	projectile_to_shoot->material.diffuse = projectile_textures[rand() % projectile_textures.size()];

	projectile_to_shoot->model.setTranslation(model.getTranslation() + model.frontVector() + Vector3(0.0, 3.0, 1.5));

	projectile_particles = new ParticleEmitter();
	projectile_particles->setTexture(projectile_to_shoot->material.diffuse);
	projectile_particles->setMaxParticles(600);
	projectile_particles->setEmitPosition(Vector3(0.1, 0.0, 0.0));
	projectile_particles->setEmitVelocity(Vector3(0.0, 0.0, 0.075));
	projectile_particles->setRandomFactor(0.04f);

	std::vector<float> sizes = {
		0.001f,
		0.01f,
		0.0f
	};

	projectile_particles->setSizesCurve(sizes);*/
}

void EntityPlayer::updateProjectilesStuff()
{
	// Shoot projectiles

	//if (projectile_respawn < projectile_respawn_seconds) {
	//	projectile_respawn -= delta_time;

	//	if (projectile_respawn <= 0.0f) {
	//		projectile_respawn = projectile_respawn_seconds;
	//	}
	//}

	//if (projectile_respawn == projectile_respawn_seconds) {
	//	// Update projectile
	//	Vector3 camera_eye = World::get_instance()->camera->eye;
	//	Vector3 camera_front = (World::get_instance()->camera->center - camera_eye).normalize();
	//	Vector3 projectile_position = camera_eye + (1.0f - projectile_charge) * camera_front + Vector3(0.0, -projectile_charge * 0.1f, 0.0);

	//	projectile_to_shoot->model.setTranslation(projectile_position);
	//	float yaw = projectile_to_shoot->model.getYawRotationToAimTo(camera_eye);
	//	projectile_to_shoot->model.rotate(yaw, Vector3::UP);
	//	projectile_to_shoot->model.rotate(projectile_charge, Vector3(1.0, 0.0, 0.0));

	//	projectile_particles->model.setTranslation(projectile_position);

	//	Matrix44 particles_rotation;
	//	particles_rotation.rotate(delta_time * 6.0f, Vector3::UP);

	//	Vector3 particles_position = particles_rotation.rotateVector(projectile_particles->getEmitPosition());
	//	Vector3 particles_velocity = particles_rotation.rotateVector(projectile_particles->getEmitVelocity());
	//	projectile_particles->setEmitPosition(particles_position);
	//	projectile_particles->setEmitVelocity(particles_velocity);

	//	projectile_particles->update(delta_time);

	//	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
	//		projectile_charge_progress += delta_time * 0.8f;

	//		projectile_charge = easeInCubic(projectile_charge_progress);

	//		float random_factor = projectile_particles->getRandomFactor() + delta_time;

	//		random_factor = clamp(random_factor, 0.01f, 0.4f);

	//		projectile_particles->setRandomFactor(random_factor);

	//		projectile_charge_progress = clamp(projectile_charge_progress, 0.0f, 1.0f);
	//		projectile_charge = clamp(projectile_charge, 0.0f, 0.75f);
	//	}
	//}

	//if (Input::wasKeyReleased(SDL_SCANCODE_SPACE)) {
	//	shoot();
	//	projectile_charge_progress = 0.0f;
	//	projectile_charge = 0.0f;
	//	projectile_respawn -= delta_time;
	//	projectile_particles->clearParticles();
	//	projectile_particles->setRandomFactor(0.04f);
	//	projectile_particles->setEmitPosition(Vector3(0.1, 0.0, 0.0));
	//	projectile_particles->setEmitVelocity(Vector3(0.0, 0.0, 0.075));
	//	projectile_to_shoot->material.diffuse = projectile_textures[rand() % projectile_textures.size()];
	//	projectile_particles->setTexture(projectile_to_shoot->material.diffuse);
	//}
}

void EntityPlayer::shoot()
{
	//World* world = World::get_instance();

	//Vector3 camera_eye = World::get_instance()->camera->eye;
	//Vector3 camera_front = (World::get_instance()->camera->center - camera_eye).normalize();

	//Vector3 origin = projectile_to_shoot->model.getTranslation();
	//Vector3 direction = camera_front;

	//// Get projectile direction and speed (combined in velocity)

	//float speed = 25.f * (projectile_charge + 0.5f);
	//Vector3 velocity = direction * speed;
	//
	//// Generate entity to shoot

	//world->addProjectile(projectile_to_shoot->model, velocity, eCollisionFilter::ENEMY | eCollisionFilter::SCENARIO | eCollisionFilter::WALL, 
	//	projectile_mesh, projectile_to_shoot->material.diffuse, projectile_charge_progress * 10.f);
}