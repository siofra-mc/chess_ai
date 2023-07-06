#include "triangle.h"

#include <iostream>

Triangle::Triangle(vec3s pos, vec3s clrs) {
	// We assume the triangle has 3 vertices and normals
	assert(pos.size() == 3);
	assert(clrs.size() == 3);

	// Default to identity
	projection = glm::mat4(1);

	// Copy vertex data into our object
	positions = pos;
	colors = clrs;
	indices = { 0, 1, 2 };
	
	// Generate the opengl objects
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object_positions);
	glGenBuffers(1, &vertex_buffer_object_colors);
	glGenBuffers(1, &element_buffer_object);
	
	glBindVertexArray(vertex_array_object);

	// Designate vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_positions);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);

	// Designate normals
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_colors);
	glBufferData(GL_ARRAY_BUFFER, clrs.size() * sizeof(glm::vec3), clrs.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);

	// Pass in indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Clean-up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


Triangle::~Triangle() {
	glDeleteVertexArrays (1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object_positions);
	glDeleteBuffers(1, &vertex_buffer_object_colors);
	glDeleteBuffers(1, &element_buffer_object);
}

void Triangle::draw(const glm::mat4 projection, GLuint shader) {
	glUseProgram(shader);

	// Add uniforms as needed here
	// ie projection

	//std::cerr << "Position of first triangle vertex: (" << positions[0].x << ", " << positions[0].y << ", " << positions[0].z << ") with color (" << colors[0].x << ", " << colors[0].y << ", " << colors[0].z << ")\n";
	//std::cerr << "Indices: " << indices[0] << ", " << indices[1] << ", " << indices[2] << std::endl;
	glBindVertexArray(vertex_array_object);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Cleanup
	glBindVertexArray(0);
	glUseProgram(0);
}