#version 150

uniform mat4 modelViewProjectionMatrix;
uniform mat4 worldMatrix;

in vec4 position;

out vec3 fragPos;

void main() {
    fragPos = vec3((worldMatrix * position).xyz);
    gl_Position = modelViewProjectionMatrix * worldMatrix * position;
}
