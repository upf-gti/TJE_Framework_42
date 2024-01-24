#pragma once
#include "framework/entities/entity.h"
#include <map>

#include "graphics/mesh.h"

class Camera;
class EntityMesh;
class EntityPlayer;
class EntityCollider;

#define MAX_HEALTH 10

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
	float camera_pitch = 0.7f;
	float camera_yaw_target = 0.f;
	float camera_pitch_target = 0.7f;
	float camera_speed = 0.001f;

	bool freeCam = false;

	void render();

	void update(float delta_time);
	void updateCamera(float delta_time);

	// Scene
	std::map<std::string, sRenderData> meshes_to_load;
	bool parseScene(const char* filename);
	void addEntity(Entity* entity);

	// AI
	std::vector<Vector3> waypoints;

	// Collisions
	EntityMesh* collision_entity = nullptr;
	void checkCameraCollisions(Vector3& newEye);
	bool testRayToScene(Vector3 ray_origin, Vector3 ray_direction, Vector3& collision, Vector3& normal, bool get_closest = false, float max_ray_dist = 3.4e+38F, bool in_object_space = false);

	// Game

	EntityCollider* wall_entity = nullptr;
	uint16_t		wall_health = MAX_HEALTH;

	void updateWall(const float delta_time);

	void hitTheWall() {
		wall_health--;
	}

	struct Projectile {
		EntityCollider* collider = nullptr;
		Vector3 velocity;
		float mass = 0.0f;
		float radius = 0.0f;
		uint8_t mask = 0;
	} ;

	std::vector<Projectile> projectiles;
	void addProjectile(const Vector3& origin, const Vector3& velocity, uint8_t flag);
	void renderProjectiles();
	void updateProjectiles(float delta_time);
	void onProjectileCollision(EntityCollider* collider, int projectile_index);
};
