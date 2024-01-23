#include "entity_enemy.h"

EntityEnemy::EntityEnemy(Mesh* mesh, const Material& material, const std::string& name) :
	EntityCollider(mesh, material, name) {
}

void EntityEnemy::update(float elapsed_time)
{
}
