#pragma once
#include "framework/entities/entity.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "framework/audio.h"

#include <map>

class Camera;
class EntityMesh;
class EntityPlayer;
class EntityCollider;

#define MAX_HEALTH 20.0f

struct Projectile {
	EntityCollider* collider = nullptr;
	Vector3 velocity;
	float mass = 0.0f;
	float radius = 0.0f;
	float damage = 1.0f;
	uint8_t mask = 0;
};

class World {

	static World* instance;

public:

	static World* get_instance() {
		if (instance != nullptr)
			return instance;
		instance = new World();
		return instance;
	}

	World();

	Vector4 bgColor;
	Entity root;

	EntityPlayer* player = nullptr;
	EntityMesh* landscape = nullptr;

	Camera* camera = nullptr; //our global camera
	Camera* camera2D = nullptr; //our global camera

	float camera_yaw = 0.f;
	float camera_pitch = 0.f;
	float camera_yaw_target = 0.f;
	float camera_pitch_target = 0.0f;
	float camera_speed = 0.001f;

	void render();

	void update(float delta_time);
	void updateCamera(float delta_time);

	// Scene

	std::map<std::string, sRenderData> meshes_to_load;
	bool parseScene(const char* filename);
	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

	std::vector<Entity*> entities_to_destroy;
	std::vector<Entity*> transparent_entities;

	// Game
	Audio world_audio_player;

	EntityCollider* wall_entity = nullptr;
	float			wall_health = MAX_HEALTH;

	// Enemy spawner
	Timer enemy_spawner_timer;
	uint16_t enemy_spawn_count = 1u;
	float enemy_spawner_frequency = 10.0f;

	const float safe_from_spawn_radius = 15.0f;

	void updateWall(float delta_time);
	void hitTheWall(float damage);

	void updateEnemySpawner(float delta_time);

	std::vector<Projectile> projectiles;

	void addProjectile(const Matrix44& model, const Vector3& velocity, uint8_t flag, Mesh* mesh, Texture* texture = nullptr, float damage = 1.0f);
	void renderProjectiles();
	void updateProjectiles(float delta_time);
	void onProjectileCollision(EntityCollider* collider, int projectile_index);
};
