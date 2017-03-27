#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 normal;	// The color variable has attribute position 1
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoords;

uniform mat3 N;
uniform mat4 MVP;

out vec3 ourColor; // Output a color to the fragment shader
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    ourColor = color; // Set ourColor to the input color we got from the vertex data
    Normal = normalize( N*normal );
    TexCoords = texcoords;
}
