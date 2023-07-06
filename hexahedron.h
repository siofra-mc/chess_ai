#pragma once

#include "util.h"

class Hexahedron {
	// Graphical Components    
	GLuint vertex_array_object;
	GLuint vertex_buffer_object_positions;
	GLuint vertex_buffer_object_normals;
	GLuint element_buffer_object;
	glm::mat4 model_projection;
	glm::vec3 color;
	
	vec3s positions;
	vec3s normals;
	uints position_indices;

public:
	Hexahedron(const glm::vec3 vertex_minimum=vec3(-1.f), const glm::vec3 vertex_maximum=vec3(1.f));
	~Hexahedron();

	void draw(const glm::mat4, GLuint);
};