#include "entity.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "framework/camera.h"
#include "framework/input.h"
#include "game/world.h"
#include "game/game.h"
#include <algorithm>
#include "game/stage.h"
#include "framework/animation.h"
#include "game/entities/entity_enemy.h"

sEntityType Entity::entityTypes[32];

Matrix44 Entity::getGlobalMatrix()
{
	if (parent)
		return model * parent->getGlobalMatrix();
	return model;
}

void Entity::render(Camera* camera)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}
}

void Entity::update(float elapsed_time)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->update(elapsed_time);
	}
}

float Entity::distance(Entity* e)
{
	return model.getTranslation().distance(e->model.getTranslation());
}

void Entity::addChild(Entity* child)
{
	if (child->parent) {
		std::cerr << "Child has already a parent, remove it first!" << std::endl;
		return;
	}

	// Checks if it's already a child
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end()) {
		std::cerr << "Entity is already one of the children!" << std::endl;
		return;
	}

	child->parent = this;
	children.push_back(child);
}

void Entity::removeChild(Entity* child)
{
	// Checks if it's a child
	auto it = std::find(children.begin(), children.end(), child);
	if (it == children.end()) {
		std::cerr << "Entity is not a child!!" << std::endl;
		return;
	}

	children.erase(it);
	child->parent = nullptr;
}
