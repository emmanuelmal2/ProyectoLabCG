#ifndef PARTICLES_H
#define PARTICLES_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <vector>

struct Particle {
	glm::vec3   p0;
	glm::vec3	position;
	glm::vec3	velocity;
	glm::vec3	acceleration;
	glm::vec3	force;
	float		mass;
	float		life;
	float		scale;
};

class Particles
{
public:
	std::vector<Particle> particles;

	Particles(unsigned int numParticles, glm::vec3 origin) {
		emitterCenter = origin;

		for (unsigned int i = 0; i < numParticles; i++) {
			Particle P;

			P.p0 = emitterCenter + glm::vec3(
				glm::linearRand(-1.0f, 1.0f), // mayor dispersión X
				0.0f,
				glm::linearRand(-1.0f, 1.0f)  // mayor dispersión Z
			);

			P.position = P.p0;

			P.velocity = glm::vec3(
				glm::linearRand(-0.6f, 0.6f),     // más ancho
				glm::linearRand(1.5f, 3.0f),      // más alto
				glm::linearRand(-0.6f, 0.6f)
			);

			P.acceleration = glm::vec3(0.0f);
			P.force = glm::vec3(0.0f);
			P.mass = 0.001f;
			P.life = glm::linearRand(4.0f, 6.0f);     // más tiempo de vida
			P.scale = glm::linearRand(0.4f, 0.6f);    // más grande

			particles.push_back(P);
		}
	}

	~Particles() {}

	void setGravity(glm::vec3 g) { gravity = g; }
	glm::vec3 getGravity() { return gravity; }

	void UpdatePhysics(float deltaTime) {
		for (int i = 0; i < (int)(particles.size()); i++) {
			Particle* P = &particles.at(i);

			if (P->life <= 0.0f) {
				// Reiniciar como nuevo humo desde el emisor
				P->p0 = emitterCenter + glm::vec3(
					glm::linearRand(-1.0f, 1.0f),
					0.0f,
					glm::linearRand(-1.0f, 1.0f)
				);

				P->position = P->p0;

				P->velocity = glm::vec3(
					glm::linearRand(-0.6f, 0.6f),
					glm::linearRand(1.5f, 3.0f),
					glm::linearRand(-0.6f, 0.6f)
				);

				P->life = glm::linearRand(4.0f, 6.0f);
				P->scale = glm::linearRand(0.4f, 0.6f);
			}

			P->force = P->mass * gravity;
			P->acceleration = P->force / P->mass;
			P->velocity += P->acceleration * deltaTime;
			P->position += P->velocity * deltaTime;

			P->life -= deltaTime;
			P->scale += deltaTime * 0.15f; // crecimiento visual más rápido
		}
	}

private:
	glm::vec3 gravity = glm::vec3(0.0f, -0.1f, 0.0f); // puedes subir a 0.0f si quieres que floten más
	glm::vec3 emitterCenter;
};

#endif
