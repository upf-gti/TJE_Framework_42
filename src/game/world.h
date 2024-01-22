#pragma once
#include "framework/entities/entity.h"
#include <map>

#include "graphics/mesh.h"

class Camera;
class EntityMesh;
class EntityPlayer;

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

	bool freeCam = false;

	void update(float seconds_elapsed);
	void updateCamera(float seconds_elapsed);

	// Scene
	std::map<std::string, sRenderData> meshes_to_load;
	bool parseScene(const char* filename);

	// AI
	std::vector<Vector3> waypoints;
	void addWayPointFromScreenPos(const Vector2& coord);

	// Collisions
	EntityMesh* collision_entity = nullptr;
	std::vector<Vector3> debug_col_positions;

	void checkCameraCollisions(Vector3& newEye);
	bool testRayToScene(Vector3 ray_origin, Vector3 ray_direction, Vector3& collision, Vector3& normal, bool get_closest = false, float max_ray_dist = 3.4e+38F, bool in_object_space = false);
};
