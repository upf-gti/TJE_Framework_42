#include "entity_mesh.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "framework/camera.h"
#include "game/game.h"

#include <algorithm>

EntityMesh::EntityMesh(Mesh* mesh, const Material& material, const std::string& name)
{
	this->mesh = mesh;
	this->name = name;

	this->material = material;
}

EntityMesh::EntityMesh(int typeIndex)
{
	type = typeIndex;
}

EntityMesh::~EntityMesh()
{

}

void EntityMesh::render(Camera* camera)
{
	const Matrix44& globalMatrix = getGlobalMatrix();

	if (!mesh) return;

	// Compute bounding sphere center in world coords
	Vector3 sphere_center = globalMatrix * mesh->box.center;
	Vector3 halfsize = globalMatrix * mesh->box.halfsize;

	// Discard objects whose bounding sphere is not inside the camera frustum
	if ((!isInstanced && !camera->testBoxInFrustum(sphere_center, halfsize) ||
		camera->eye.distance(sphere_center) > 5000.0f))
		return;

	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? "data/shaders/instanced.vs" : "data/shaders/basic.vs", "data/shaders/phong.fs");
	}

	if (material.transparent) {
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Enable shader
	material.shader->enable();

	// Upload uniforms
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

	if (!isInstanced) material.shader->setUniform("u_model", globalMatrix);

	// By default values
	material.shader->setUniform("u_Ka", material.Ka);
	material.shader->setUniform("u_Kd", material.Kd);
	material.shader->setUniform("u_Ks", material.Ks);

	Mesh* currentLod = mesh;
	float distance = camera->eye.distance(globalMatrix.getTranslation());
	for (int i = 0; i < meshLods.size(); ++i) {
		if (distance > meshLods[i].distance) {
			// Since meshLods is ordered (a > b) we can stop iterating
			currentLod = meshLods[i].mesh;
			break;
		}
	}

	if (isInstanced)
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	else if(animated)
		mesh->renderAnimated(GL_TRIANGLES, &anim.getCurrentSkeleton());
	else
		currentLod->render(GL_TRIANGLES);

	// Disable shader
	material.shader->disable();

	if (material.transparent) {
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}

	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}
}

void EntityMesh::update(float delta_time)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->update(delta_time);
	}
}

void EntityMesh::addLOD(Mesh* mesh, float distance)
{
	meshLods.push_back({ mesh, distance });

	std::sort(meshLods.begin(), meshLods.end(), [](const sMeshLOD& lod1, const sMeshLOD& lod2) {
		return lod1.distance > lod2.distance;
	});
}

void EntityMesh::addInstance(const Matrix44& model)
{
	models.push_back(model);
}