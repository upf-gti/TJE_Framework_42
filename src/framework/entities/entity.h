#pragma once

#include "framework/includes.h"
#include "framework/framework.h"

class Mesh;
class Shader;
class Texture;
class World;
class Camera;

enum eCollisionFilter {
	NONE = 0,
	WALL = 1 << 0,
	SCENARIO = 1 << 1,
	PLAYER = 1 << 2,
	ENEMY = 1 << 3,
	ALL = 0xFF
};

enum eButtonId {
	NoButton,
	PlayButton,
	OptionsButton,
	ExitButton
	//...
};

struct sEntityType {

	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	Shader* shader = nullptr;
};

struct sMeshLOD {
	Mesh* mesh = nullptr;
	float distance = 0.0f;
};

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
	float distance = 0.f;
};

struct Material {
	Shader* shader = nullptr;
	Vector4 color = Vector4(1.f);

	Texture* diffuse = nullptr;
	Texture* normals = nullptr;

	Vector3 Ka = Vector3(0.1f);
	Vector3 Kd = Vector3(1.0f);
	Vector3 Ks = Vector3(0.2f);

	bool transparent = false;
	float tiling = 1.f;
};

struct sRenderData {
	Material material;
	std::vector<Matrix44> models;
};

class Entity {

public:

	static sEntityType entityTypes[32];

	Entity() {}; 			// Constructor
	virtual ~Entity() {}; 	// Destructor

	// Indexed types
	int type = -1;

	std::string name;
	Matrix44 model;

	Entity* parent = nullptr;
	std::vector<Entity*> children;

	float distance(Entity* e);

	void addChild(Entity* child);
	void removeChild(Entity* child);

	// Methods that should be overwritten
	// by derived classes 
	virtual void render(Camera* camera);
	virtual void update(float delta_time);

	// Some useful methods
	Matrix44 getGlobalMatrix();
};
