#pragma once 

#include "entity.h"
#include "framework/animation_manager.h"

class EntityMesh : public Entity {

public:
	EntityMesh() {};
	EntityMesh(Mesh* mesh, const Material& material, const std::string& name = "");
	~EntityMesh();

	// In case of indexed type meshes
	EntityMesh(int typeIndex);

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Material material;

	// Animations
	bool animated = false;
	AnimationManager anim;

	// Instancing
	bool isInstanced = false;
	std::vector<Matrix44> models;

	std::vector<sMeshLOD> meshLods;

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float delta_time);

	void addLOD(Mesh* mesh, float distance);
	void addInstance(const Matrix44& model);
};

