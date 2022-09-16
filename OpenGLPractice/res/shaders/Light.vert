#version 400 core
layout (location = 0) in vec3 aPos;       // λ�ñ���������λ��ֵΪ 0 

uniform mat4 v_Model;
uniform mat4 v_View;
uniform mat4 v_Projection;


void main()
{
    gl_Position = v_Projection * v_View * v_Model * vec4(aPos, 1.0);
}
