#include "hexahedron.h"

Hexahedron::Hexahedron(const glm::vec3 vertex_minimum, const glm::vec3 vertex_maximum) {
	model_projection = glm::mat4(1.f);

	color = glm::vec3(1.f, 0.f, 1.f);
	
	positions = {
		glm::vec3(vertex_minimum.x, vertex_minimum.y, vertex_minimum.z), // 0,0,0
		glm::vec3(vertex_maximum.x, vertex_minimum.y, vertex_minimum.z), // 1,0,0
		glm::vec3(vertex_minimum.x, vertex_maximum.y, vertex_minimum.z), // 0,1,0
		glm::vec3(vertex_maximum.x, vertex_maximum.y, vertex_minimum.z), // 1,1,0
		glm::vec3(vertex_minimum.x, vertex_minimum.y, vertex_maximum.z), // 0,0,1
		glm::vec3(vertex_maximum.x, vertex_minimum.y, vertex_maximum.z), // 1,0,1
		glm::vec3(vertex_minimum.x, vertex_maximum.y, vertex_maximum.z), // 0,1,1
		glm::vec3(vertex_maximum.x, vertex_maximum.y, vertex_maximum.z), // 1,1,1
	};

	//normals = {
	//	glm::vec3(0,0, 1), // front
	//	glm::vec3(0,0,-1), // back
	//	glm::vec3(-1,0,0), // left
	//	glm::vec3( 1,0,0), // right
	//	glm::vec3(0, 1,0), // top
	//	glm::vec3(0,-1,0)  // bottom
	//};

	position_indices = {
		4, 5, 6, 5, 6, 7, // front
		0, 1, 2, 1, 2, 3, // back
		0, 2, 4, 2, 4, 6, // left
		1, 3, 5, 3, 5, 7, // right
		2, 3, 6, 3, 6, 7, // top
		0, 1, 4, 1, 4, 5  // bottom
	};

	// Generate gl objects 
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object_positions);
	glGenBuffers(1, &vertex_buffer_object_normals);
	glGenBuffers(1, &element_buffer_object);

	glBindVertexArray(vertex_array_object);

	// Fill in positions buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_positions);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);

	//// Fill in normals buffer
	//glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_normals);
	//glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);

	// Set the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(undigned int), indices.data(), GL_STATIC_DRAW);

	// Clean-up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Hexahedron::~Hexahedron() {
	glDeleteBuffers(1, &vertex_buffer_object_positions);
	glDeleteBuffers(1, &vertex_buffer_object_normals);
	glDeleteBuffers(1, &element_buffer_object);
	glDeleteVertexArrays(1, &vertex_array_object);
}

void Hexahedron::draw(const glm::mat4 projection, GLuint shader) {
	glUseProgram(shader);

	glBindVertexArray(vertex_array_object);
	
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}