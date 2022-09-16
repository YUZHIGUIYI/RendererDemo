#version 400 core
layout (location = 0) in vec3 aPos;       // λ�ñ���������λ��ֵΪ 0 
layout (location = 1) in vec2 aTexCoord;  // �������������λ��Ϊ 1
layout (location = 2) in vec3 aNormal;    // ��������������λ��Ϊ 2

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
    Normal = mat3(transpose(inverse(v_Model))) * aNormal;  // �����������㿪���ܴ�
    FragPos = vec3(v_Model * vec4(aPos, 1.0f));
}