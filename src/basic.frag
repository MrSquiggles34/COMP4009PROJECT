#version 150

uniform vec3 objectColor;

in vec3 fragPos;

out vec4 fragColor;
 
void main() {
	fragColor = vec4(objectColor, 1);
}
