#version 400 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

const float NEAR = 0.1f;
const float FAR = 50.0f;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;  // back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // store the LinearizeDepth into alpha
    // gPosition.a = LinearizeDepth(gl_FragCoord.z);
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95);
}