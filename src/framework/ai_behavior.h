#pragma once
#include "framework.h"
#include "extra/pathfinder/PathFinder.h"
#include "extra/pathfinder/Dijkstra.h"

enum eActions { SEARCH, ATTACK, FLEE };

class EntityEnemy;

class WayPoint : public DijkstraNode
{
public:

    WayPoint() {}

    WayPoint(const Vector3& position)
    {
        this->position = position;
    }

    void addLink(WayPoint* waypoint, float distance) {
        addChild(waypoint, distance);
        waypoint->addChild(this, distance);
    }

    Vector3 position;
};

class AIBehavior {

public:

	AIBehavior(EntityEnemy* e) { ref = e; };
	void update(float delta_time);

	bool canSeeTarget();
	void searchTarget(float delta_time);
	void lookAtTarget(const Vector3& target, float delta_time);
	bool hasAmmo();
	void shoot();

private:
	EntityEnemy* ref = nullptr;

	// Waypoints
	bool walk_forwards = true;
	std::vector<WayPoint> path;
	int wayPointIndex = 0;

	// Machine state props
	eActions state = SEARCH;
	int ammo = 200;
};