#pragma once
#include "includes.h"
#include "framework.h"
#include "framework/entities/entity.h"

class Texture;

struct sParticle {

	int id;
	Vector3 position;
	Vector3 velocity;
	float ttl;
	float rotation;
	bool active = false;
};

class ParticleEmitter : Entity {

	int max_particles = 300;
	int active_particles = 0;
	std::vector<sParticle> particles;

	// Properties of the emitter
	int last_id = 0;
	float emit_rate = 2.3f;
	float emit_timer = 0.f;

	// Properties of the particles
	float max_ttl = 3.f;
	std::vector<Vector4> colors;
	std::vector<float> sizes;
	Texture* texture = nullptr;
	int texture_grid = 2;
	bool additive_blending = false;
	bool animated_texture = true;	// animation tiles
	bool sort_distance = true;

	template<typename T>
	T sample(float time, int n, T* data) {
		// Convert range 0..1 to 0..n-1
		float time_in_table = time * (n - 1);
		// if time_in_table = 1.2.      entry = 1., amount = 0.2
		float entry;
		float amount = modf(time_in_table, &entry);
		return data[(int)entry] * (1 - amount) + data[(int)entry + 1] * amount;
	}

	void emit();

public:
	
	ParticleEmitter();
	
	void render();
	void update(float seconds_elapsed);
	void setTexture( const char* filename );
};