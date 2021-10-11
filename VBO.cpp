#include "VBO.h"

VBO::VBO(GLfloat* verticies, GLsizeiptr size) {
	glGenBuffers(1, &ID);//initialize buffer at reference address VBO
	glBindBuffer(GL_ARRAY_BUFFER, ID);//set VBO as current buffer to pull geometry data from
	glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);//push geometry data from verticies into buffer
}

void VBO::getData(GLfloat* verticies, GLsizeiptr size) {
	glBindBuffer(GL_ARRAY_BUFFER, ID);//set VBO as current buffer to pull geometry data from
	glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_STATIC_DRAW);//push geometry data from verticies into buffer
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}
void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}