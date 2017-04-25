#version 330 core
in vec3 ourColor;
in vec2 TexCoords;
in vec3 Normal;
out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 lightDirection;

uniform bool texture;
uniform bool texture_lambert;
uniform bool normals;
uniform bool lambert;
uniform bool red;
uniform bool albedo;
uniform bool p_error;
uniform bool shadows;

float diffuse = 0;

void main()
{
    vec3 normalVector = normalize(Normal);
    vec3 rgb_normal = normalVector*.5 +.5;
    vec3 albedo3 = vec3(ourColor.x,ourColor.x,ourColor.x);
    vec3 shadows3 = vec3(ourColor.y,ourColor.y,ourColor.y);
    float error = ourColor.z;

    if( dot(lightDirection, normalVector) > 0.0)
    {
       diffuse = dot(lightDirection, normalVector);
    }

    vec4 texcolor = texture2D(ourTexture,TexCoords);
    vec4 customBlue = vec4(0.0,0.0,1.0,1.0);
    vec4 customRed = diffuse*vec4(1.0,0.0,0.0,1.0);
    vec4 texnormcolor = diffuse*vec4(texcolor.x,texcolor.y,texcolor.z,1.0);
    vec4 lambertcolor = diffuse*vec4(albedo3,1.0);
    vec4 albedocolor = vec4(albedo3,1.0);
    vec4 shadowcolor = vec4(shadows3,1.0);
    vec4 errorcolor = vec4(1.0,1.0,1.0,1.0) - max(0,error)*vec4(1.0,1.0,0.0,0.0) - max(0,-error)*vec4(0.0,1.0,1.0,0.0);
    vec4 normalcolor = vec4(rgb_normal,1.0);

    color = int(texture)*texcolor + int(texture_lambert)*texnormcolor +
            int(lambert)*lambertcolor + int(normals)*normalcolor +
            int(albedo)*albedocolor + int(shadows)*customBlue +
            int(p_error)*errorcolor + int(red)*customRed;
}
