#include "world.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "framework/entities/entity_collider.h"
#include "framework/entities/entity_mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "game/game.h"
#include "game/stage.h"
#include "entities/entity_player.h"
#include "entities/entity_ai.h"

#include <algorithm>
#include <fstream>
#include <string>

World* World::instance = nullptr;

World::World()
{
	bgColor.set(0.2f, 0.2f, 0.2f, 1.f);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	// Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 20.f, -20.f), Vector3(0.f, 0.f, 0.f), Vector3::UP); //position the camera and point to 0,0,0
	camera->setPerspective(60.f, width / (float)height, 0.01f, 1000.f); //set the projection, we want to be perspective

	// Configure 2d camera to render UI elements
	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, width, height, 0, -1.f, 1.f);

	Material landscape_cubemap;
	landscape_cubemap.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs");
	landscape_cubemap.diffuse = new Texture();
	landscape_cubemap.diffuse->loadCubemap("landscape", {
			"data/textures/skybox/right.png",
			"data/textures/skybox/left.png",
			"data/textures/skybox/bottom.png",
			"data/textures/skybox/top.png",
			"data/textures/skybox/front.png",
			"data/textures/skybox/back.png"
	});

	player = new EntityPlayer();
	landscape = new EntityMesh(Mesh::Get("data/meshes/cubemap/cubemap.ASE"), landscape_cubemap, "landscape");

	parseScene("data/myscene.scene");

	std::sort(root.children.begin(), root.children.end(), [](const Entity* lhs, const Entity* rhs)
	{
		return static_cast<const EntityMesh*>(lhs)->material.shader < static_cast<const EntityMesh*>(rhs)->material.shader;
	});

	freeCam = false;

	// Add protection wall
	{
		Material wall_material;
		wall_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
		wall_material.Ks.set(0.f);

		wall_entity = new EntityCollider(Mesh::Get("data/meshes/wall/wall.obj"), wall_material, "wall");
		wall_entity->setLayer(eCollisionFilter::WALL);

		addEntity(wall_entity);
	}
}

void World::updateCamera(float delta_time)
{
	if (freeCam)
	{
		float mouse_speed = 5.0f;
		float speed = delta_time * mouse_speed; //the speed is defined by the delta_time so it goes constant

		//mouse input to rotate the cam
		if (Input::isMousePressed(SDL_BUTTON_RIGHT)) //is left button pressed?
		{
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, 1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(1.0f, 0.0f, 0.0f)));
		}

		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}
	else {

		// Get mouse deltas
		camera_yaw_target += Input::mouse_delta.x * camera_speed;
		camera_pitch_target += Input::mouse_delta.y * camera_speed;

		camera_pitch_target = clamp(camera_pitch_target, -M_PI * 0.2f, M_PI * 0.2f);

		camera_yaw += camera_yaw_target - camera_yaw * 0.5f;
		camera_pitch += camera_pitch_target - camera_pitch * 0.5f;

		Matrix44 mYaw;
		mYaw.setRotation(camera_yaw, Vector3::UP);
		Matrix44 mPitch;
		mPitch.setRotation(camera_pitch, Vector3(-1, 0, 0));
		Vector3 front = (mPitch * mYaw).frontVector();
		Vector3 eye;
		Vector3 center;

		eye = player->getGlobalMatrix() * Vector3(0.f, 3.0f, 0.3f);
		center = eye + front;

		camera->lookAt(
			eye,
			center,
			Vector3::UP
		);
	}
}

void World::checkCameraCollisions(Vector3& newEye)
{
	Vector3 origin = camera->center;
	Vector3 direction = (newEye - camera->center);
	Vector3 normal;

	testRayToScene(camera->center, direction.normalize(), newEye, normal, true, direction.length());
}

void World::render()
{
	// Render entity players
	player->render(camera);

	// Render all scene tree
	root.render(camera);

	// Render projectiles
	renderProjectiles();

	// Render transparent entities at the end
	for (auto transparent_entity : transparent_entities) {
		transparent_entity->render(camera);
	}

	transparent_entities.clear();
}

void World::update(float delta_time)
{
	updateCamera(delta_time);

	root.update(delta_time);

	player->update(delta_time);

	updateProjectiles(delta_time);

	updateWall(delta_time);

	updateEnemySpawner(delta_time);

	// Delete pending entities
	for (auto e : entities_to_destroy) {
		root.removeChild(e);
		delete e;
	}

	entities_to_destroy.clear();
} 

bool World::testRayToScene(Vector3 ray_origin, Vector3 ray_direction, Vector3& collision, Vector3& normal, bool get_closest, float max_ray_dist, bool in_object_space)
{
	float closest_dist = INFINITY;
	Vector3 tmpCol;
	bool has_collided = false;

	auto resolve_closest = [&](EntityCollider* ec) {

		// std::cout << ec->name << std::endl;

		has_collided = true;
		float new_distance = tmpCol.distance(ray_origin);
		if (new_distance < closest_dist) {
			collision = tmpCol;
			closest_dist = new_distance;
		}
	};

	for (auto e : root.children)
	{
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
		if (!ec) continue;

		if (!ec->isInstanced) {// Single model

			bool result = ec->mesh->testRayCollision(e->model, ray_origin, ray_direction, tmpCol, normal, max_ray_dist, in_object_space);
			if (result)
			{
				if (!get_closest) return true;
				resolve_closest(ec);
			}
		}
		else { // Instanced mesh
			for (int i = 0; i < ec->models.size(); ++i)
			{
				if (!ec->mesh->testRayCollision(ec->models[i], ray_origin, ray_direction, tmpCol, normal, max_ray_dist, in_object_space))
					continue;

				if (!get_closest) return true;
				resolve_closest(ec);
			}
		}
	}

	return has_collided;
}

bool World::parseScene(const char* filename)
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors

	Material enemy0_mat;
	enemy0_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	enemy0_mat.Ks.set(0.f);

	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		if (mesh_name[0] == '#')
			continue;

		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, nothing to do here
		if (render_data.models.empty())
			continue;


		Material mat = render_data.material;
		EntityMesh* new_entity;

		size_t enemy_0 = data.first.find("@enemy_0");
		size_t enemy_1 = data.first.find("@enemy_1");
		if (enemy_0 != std::string::npos) {
			Mesh* mesh = Mesh::Get("data/meshes/enemy_0/enemy_0.obj");
			new_entity = new EntityAI(mesh, enemy0_mat, AI_SHOOTER);
		} else
		if (enemy_1 != std::string::npos) {
			Mesh* mesh = Mesh::Get("data/meshes/enemy_1/enemy_1.obj");
			new_entity = new EntityAI(mesh, enemy0_mat, AI_BREAKER);
		}
		else {
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
		}

		// Add entity to scene root
		addEntity(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

void World::addEntity(Entity* entity)
{
	root.addChild(entity);
}

void World::removeEntity(Entity* entity)
{
	entities_to_destroy.push_back(entity);
}

// GAME METHODS

void World::updateWall(const float delta_time)
{
	float hp = wall_health / ((float)MAX_HEALTH);
	wall_entity->model.setTranslation(Vector3(0.0f, lerp(-1.0f, 0.0f, hp), 0.0f));

	if (hp <= 0.0f)
	{
		// Game lost!!
		StageManager::get_instance()->goTo("menuStage");
	}
}

void World::hitTheWall(uint16_t damage)
{
	wall_health -= damage;
	// std::cout << "Zombie hit the wall" << std::endl;
}

void World::updateEnemySpawner(float delta_time)
{
	if (!enemy_spawner_timer.update(delta_time))
		return;

	Material enemy_material;
	enemy_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	enemy_material.Ks.set(0.f);

	Mesh* mesh = Mesh::Get("data/meshes/enemy_1/enemy_1.obj");

	// Spawn enemies TODO: choose between enemy type
	for (uint32_t i = 0u; i < enemy_spawn_count; i++) {

		EntityAI* new_enemy = new EntityAI(mesh, enemy_material, random() > 0.5f ? AI_BREAKER : AI_SHOOTER);

		// Select a random position inside a 5.0 radius
		Vector2 position;
		position.random(10.0f);
		position = position - 5.0f;
		Vector2 pos_direction = position;
		pos_direction.normalize();

		// Push it away the spawn radius of the center
		position = position + (pos_direction * safe_from_spawn_radius);

		new_enemy->model.translate(Vector3(position.x, 0.0f, position.y));

		addEntity(new_enemy);
	}

	// Limit the spawn increase to 5 enemies at max
	enemy_spawn_count = std::min(enemy_spawn_count + 1u, 5u);
	enemy_spawner_timer.set(enemy_spawner_frequency);
}

void World::addProjectile(const Matrix44& model, const Vector3& velocity, uint8_t flag, Mesh* mesh, Texture* texture, float damage)
{
	Material projectile_material;
	projectile_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	projectile_material.diffuse = texture;

	EntityCollider* entity_to_shoot = new EntityCollider(mesh, projectile_material, "projectile");
	entity_to_shoot->model = model;

	Projectile p;

	p.collider = entity_to_shoot;
	p.velocity = velocity;
 	p.radius = p.collider->mesh->radius;
	p.damage = damage;
	p.mask = flag;

	projectiles.push_back(p);

	world_audio_player.play("data/sounds/throw.wav");
}

void World::renderProjectiles()
{
	for (int i = 0; i < projectiles.size(); ++i)
	{
		Projectile& p = projectiles[i];
		p.collider->render(camera);
	}
}

void World::updateProjectiles(float delta_time)
{ 
	for (int i = 0; i < projectiles.size(); ++i)
	{
		Projectile& p = projectiles[i];

		// Move and apply gravity to projectile

		p.velocity.y -= 9.8f * delta_time;

		p.collider->model.translateGlobal(p.velocity * delta_time);

		// Decrease velocity in XZ 

		//p.velocity.x = lerp(p.velocity.x, 0.0f, 0.1f * delta_time);
		//p.velocity.z = lerp(p.velocity.z, 0.0f, 0.1f * delta_time);

		// Check collisions

		for (auto e : root.children)
		{
			EntityCollider* ec = dynamic_cast<EntityCollider*>(e);
			if (!ec)
				continue;

			if (!(ec->getLayer() & p.mask))
				continue;

			Vector3 colPoint;
			Vector3 colNormal;

			if (ec->mesh->testSphereCollision(ec->model, p.collider->model.getTranslation(), p.radius, colPoint, colNormal)) {
				onProjectileCollision(ec, i);
				return;
			}
		}

		// Check player collision in case of ENEMY projectiles

		if (p.mask & eCollisionFilter::PLAYER)
		{
			EntityCollider* player_collider = dynamic_cast<EntityCollider*>(player);

			Vector3 colPoint;
			Vector3 colNormal;

			if (player_collider->mesh->testSphereCollision(player_collider->model, p.collider->model.getTranslation(), p.radius, colPoint, colNormal)) {
      				onProjectileCollision(player_collider, i);
			}
		}
	}
}

void World::onProjectileCollision(EntityCollider* collider, int projectile_index)
{
	Projectile& p = projectiles[projectile_index];

	// In case of enemies, make stuff on collide

	EntityAI* ai_entity = dynamic_cast<EntityAI*>(collider);
	EntityCollider* player_entity = dynamic_cast<EntityCollider*>(collider);
	if (ai_entity) {
		ai_entity->onProjectileCollision(p);
	} else if (player_entity) {
		hitTheWall(1u);
	}

	// Delete projectile

	delete p.collider;
	projectiles.erase(projectiles.begin() + projectile_index);
}
