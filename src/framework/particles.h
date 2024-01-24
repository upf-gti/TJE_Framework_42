#pragma once
#include "includes.h"
#include "framework.h"
#include "framework/entities/entity.h"

class Texture;
class Camera;

struct sParticle {

	int id;
	Vector3 position;
	Vector3 velocity;
	float ttl;
	float rotation;
	bool active = false;
};

class ParticleEmitter : public Entity {

	int max_particles = 300;
	int active_particles = 0;
	std::vector<sParticle> particles;

	// Properties of the emitter
	int last_id = 0;
	float emit_rate = 0.01f;
	float emit_timer = 0.f;

	Vector3 emit_position = {};
	Vector3 emit_velocity = Vector3(0.0f, 0.2f, 0.0f);
	float random_factor = 0.0f;

	// Properties of the particles
	float max_ttl = 3.f;
	std::vector<Vector4> colors = { Vector4(1.0f, 1.0f, 1.0f, 1.0f) };
	std::vector<float> sizes = { 0.5f };
	Texture* texture = nullptr;
	int texture_grid = 2;
	bool additive_blending = false;
	bool animated_texture = false;	// animation tiles
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
	
	void render(Camera* camera) override;
	void update(float delta_time);

	void clearParticles();

	void setTexture( const char* filename );
	void setTexture( Texture* texture );

	void setEmitPosition(const Vector3& position);
	void setEmitVelocity(const Vector3& velocity);

	void setColorsCurve(std::vector<Vector4>& colors);
	void setSizesCurve(std::vector<float>& sizes);

	void setMaxParticles(int max_particles);

	void setRandomFactor(float random_factor);

	Vector3 getEmitPosition();
	Vector3 getEmitVelocity();
	float getRandomFactor();

};