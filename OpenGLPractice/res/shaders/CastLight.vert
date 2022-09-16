#version 400 core
layout (location = 0) in vec3 aPos;       // 位置变量的属性位置值为 0 
layout (location = 1) in vec2 aTexCoord;  // 纹理变量的属性位置为 1
layout (location = 2) in vec3 aNormal;    // 法线向量的属性位置为 2

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform mat4 v_Model;
uniform mat4 v_View;
uniform mat4 v_Projection;

void main()
{
    gl_Position = v_Projection * v_View * v_Model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    //Normal = aNormal;
    Normal = mat3(transpose(inverse(v_Model))) * aNormal;  // 矩阵求逆运算开销很大
    FragPos = vec3(v_Model * vec4(aPos, 1.0f));
}