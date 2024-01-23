#include "ai_behavior.h"
#include "game/world.h"
#include "graphics/mesh.h"
#include "game/entities/entity_player.h"
#include "game/entities/entity_enemy.h"

#include <iostream>

void AIBehavior::update(float seconds_elapsed)
{
    if (state == SEARCH)
    {
        searchTarget(seconds_elapsed);

        if (canSeeTarget()) {
            std::cout << "target spotted!" << std::endl;
            state = ATTACK;
        }
        else {
            searchTarget(seconds_elapsed);
        }
    }
    else if (state == ATTACK)
    {
        World* world = World::get_instance();
        Vector3 target = world->player->model.getTranslation();

        lookAtTarget(target, seconds_elapsed);

        if (!canSeeTarget()) {
            std::cout << "target missed!" << std::endl;
            state = SEARCH;
        }
        if (hasAmmo()) {
            shoot();
        }
        else {
            std::cout << "ran out of ammo, fly you fool!" << std::endl;
            state = FLEE;
        }
    }
    else if (state == FLEE) {
        state = SEARCH;
    }
}

bool AIBehavior::canSeeTarget()
{
    World* world = World::get_instance();
    Matrix44 target = world->player->model;

    Vector3 origin = ref->model.getTranslation();
    Vector3 toTarget = target.getTranslation() - origin;
    float distance = toTarget.length();
    toTarget = toTarget.normalize();
    Vector3 enemyFront = ref->model.frontVector();

    // First step: Vision Cone
    // 0.5 will be 90º and 0 180º
    float angle = 1.f - clamp(ref->fov * DEG2RAD / M_PI, 0.f, 1.f);
    if (toTarget.dot(enemyFront) > angle) {

        Vector3 collision;
        Vector3 normal;

        // Check obstacles
        return !world->testRayToScene(origin, toTarget, collision, normal, false, distance);
    }

    return false;
}

void AIBehavior::searchTarget(float seconds_elapsed)
{
    if (path.size() && path.size() == World::get_instance()->waypoints.size())
    {
        Vector3 origin = ref->model.getTranslation();
        origin.y = 0.f;
        Vector3 target = Vector3(path[wayPointIndex].position.x, 0.f, path[wayPointIndex].position.z);

        float toTargetLength = (target - origin).length();

        lookAtTarget(target, seconds_elapsed);
        ref->model.translate(0.f, 0.f, seconds_elapsed * 2.f);

        if (toTargetLength < 0.1f) {

            if (walk_forwards && wayPointIndex + 1 == path.size()) {
                walk_forwards = false;
            }

            if (!walk_forwards && wayPointIndex - 1 < 0) {
                walk_forwards = true;
            }

            wayPointIndex += (walk_forwards ? 1 : -1);
        }
    }
    else
    {
        if (World::get_instance()->waypoints.size() < 2)
            return;

        path.clear();

        const std::vector<Vector3>& points = World::get_instance()->waypoints;
        std::vector<WayPoint> wp_nodes;
        wp_nodes.resize(points.size());

        for (int i = 0; i < points.size() - 1; ++i)
        {
            wp_nodes[i].position = points[i];
            wp_nodes[i + 1].position = points[i + 1];
            wp_nodes[i].addLink(&wp_nodes[i + 1], (points[i] - points[i + 1]).length());
        }
       
        // Create the PathFinder and PathAlgorithm stuff
        PathFinder<WayPoint> p;
        std::vector<WayPoint*> solution;

        p.setStart(wp_nodes[0]);
        p.setGoal(wp_nodes[wp_nodes.size() - 1]);

        bool r = p.findPath<Dijkstra>(solution);

        if (r) // path found
        {
            std::cout << "Solution (" << solution.size() - 1 << " steps) :" << std::endl;
            for (const auto& wp : solution) {
                std::cout << wp->position.x << ", " << wp->position.y << ", " << wp->position.z << " -> ";
                path.push_back(wp->position);
            }
            std::cout << std::endl;
        }
        else
            std::cerr << "No path was found, sorry." << std::endl;
    }
}

void AIBehavior::lookAtTarget(const Vector3& target, float seconds_elapsed)
{
    Vector3 origin = ref->model.getTranslation();
    origin.y = 0.f;
    float delta_yaw = ref->model.getYawRotationToAimTo(target);
    ref->model.rotate(delta_yaw * seconds_elapsed * 4, Vector3(0.f, 1.f, 0.f));
}

bool AIBehavior::hasAmmo()
{
    return ammo > 0;
}

void AIBehavior::shoot()
{
    //ammo--;
}
