#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 color;
out vec2 texCoord;

uniform float scale;
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	if (aPos.z == 1){
		color = vec3(1,0,0);
	}
	else if (aPos.z == 0) {
		color = vec3(1,1,1);
	}
	else {
		color = vec3(0,0,-aPos.z);
	}
	texCoord = aTex;
}