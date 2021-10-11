#include "EBO.h"

EBO::EBO(GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &ID);//initialize buffer at reference address VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);//set VBO as current buffer to pull geometry data from
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);//push geometry data from verticies into buffer
}

void EBO::getData(GLuint* indices, GLsizeiptr size) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);//set VBO as current buffer to pull geometry data from
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);//push geometry data from verticies into buffer
}

void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}
void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void EBO::Delete() {
	glDeleteBuffers(1, &ID);
}