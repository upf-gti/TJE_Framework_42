#include <cassert>
#include <algorithm>
#include "particles.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "framework/camera.h"
#include "camera.h"
#include "utils.h"
#include "input.h"

ParticleEmitter::ParticleEmitter()
{
	particles.resize(max_particles);
}

void ParticleEmitter::setTexture(const char* filename)
{
	texture = Texture::Get(filename);
}

void ParticleEmitter::setTexture(Texture* texture)
{
	this->texture = texture;
}

void ParticleEmitter::setEmitPosition(const Vector3& position)
{
	emit_position = position;
}

void ParticleEmitter::setEmitVelocity(const Vector3& velocity)
{
	emit_velocity = velocity;
}

void ParticleEmitter::setColorsCurve(std::vector<Vector4>& colors)
{
	this->colors = colors;
}

void ParticleEmitter::setSizesCurve(std::vector<float>& sizes)
{
	this->sizes = sizes;
}

void ParticleEmitter::setMaxParticles(int max_particles)
{
	this->max_particles = max_particles;
}

void ParticleEmitter::setRandomFactor(float random_factor)
{
	this->random_factor = random_factor;
}

Vector3 ParticleEmitter::getEmitPosition()
{
	return emit_position;
}

Vector3 ParticleEmitter::getEmitVelocity()
{
	return emit_velocity;
}

float ParticleEmitter::getRandomFactor()
{
	return random_factor;
}

void ParticleEmitter::render(Camera* camera)
{
	assert(texture && "No texture in emitter!");

	// Mesh to pack all particle vertices
	Mesh quad;

	if (sort_distance)
	{
		std::sort(particles.begin(), particles.end(), [&](const sParticle& a, const sParticle& b) {
			return a.position.distance(camera->eye) > b.position.distance(camera->eye);
		});
	}

	// {0, 0}	, {0.5, 0}
	// {0, 0.5}	, {0.5, 0.5}
	std::vector<Vector2> uv_offsets;
	for (int j = 0; j < texture_grid; ++j)
		for (int i = 0; i < texture_grid; ++i)
			uv_offsets.push_back( Vector2(i / (float)texture_grid, 1.f - j / (float)texture_grid) );

	for (sParticle& p : particles)
	{
		if (!p.active)
			continue;

		Vector3 pos = p.position;
		pos = model * pos;

		float nt = clamp(p.ttl / max_ttl, 0.f, 1.f);

		Vector4 color = sample<Vector4>(nt, colors.size(), &colors[0]);
		float size = sample<float>(nt, sizes.size(), &sizes[0]);
		
		Vector3 right = camera->getLocalVector(Vector3(1, 0, 0));
		Vector3 top = camera->getLocalVector(Vector3::UP);

		/*Matrix44 rot;
		Vector3 front = camera->getLocalVector(Vector3(0, 0, 1));
		rot.setRotation(nt, front);
		right = rot.rotateVector(right);
		top = rot.rotateVector(top);*/

		int grid_frames = texture_grid * texture_grid;
		int cloud_index = animated_texture ? nt * grid_frames : p.id % grid_frames;
		float d_uvs = 1.f / texture_grid;
		Vector2 uv_offset = uv_offsets[cloud_index];

		quad.vertices.push_back( pos + (top - right) * size );
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top - right) * size);
		quad.uvs.push_back(Vector2(0, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (top + right) * size);
		quad.uvs.push_back(Vector2(1, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
	}

	if (!quad.vertices.size())
		return;

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	if (additive_blending)
	{
		glBlendFunc(GL_ONE, GL_ONE);
	}
	else
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glDepthMask(false);

	drawMesh(Camera::current, Matrix44(), &quad, Vector4::WHITE, texture, Shader::Get("data/shaders/basic.vs", "data/shaders/particle.fs"));

	glDepthMask(true);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void ParticleEmitter::update(float delta_time)
{
	emit_timer += delta_time;
	if (emit_timer > emit_rate && active_particles < max_particles)
	{
		emit();
	}

	for (sParticle& p : particles)
	{
		if (!p.active)
			continue;

		Vector3 random_velocity = Vector3(random(random_factor, -random_factor * 0.5f), random(random_factor, -random_factor * 0.5f), random(random_factor, -random_factor * 0.5f));
		 p.position += (p.velocity + random_velocity) * delta_time;
		 p.ttl += delta_time;

		// Kill particle if run out of time
		if (p.ttl > max_ttl) {
			p.active = false;
			active_particles--;
		}
	}
}

void ParticleEmitter::clearParticles()
{
	particles.clear();
	particles.resize(max_particles);
	emit_timer = 0.0f;
	active_particles = 0;
}

void ParticleEmitter::emit()
{
	emit_timer = 0.f;

	// Search slot and activate particle
	for (sParticle& p : particles)
	{
		if (p.active)
			continue;

		p.id = last_id++;
		p.ttl = 0.f;
		p.active = true;
		p.position = emit_position;
		p.velocity = emit_velocity;
		active_particles++;
		break;
	}
}