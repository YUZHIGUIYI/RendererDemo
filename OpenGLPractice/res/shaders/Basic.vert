#version 400 core
layout (location = 0) in vec3 aPos;       // λ�ñ���������λ��ֵΪ 0 
//layout (location = 1) in vec3 aColor;     // ��ɫ����������λ��ֵΪ 1
layout (location = 1) in vec2 aTexCoord;  // �������������λ��Ϊ 2

//out vec3 ourColor;  // ��Ƭ����ɫ�����һ����ɫ
out vec2 TexCoord;
uniform mat4 v_Model;
uniform mat4 v_View;
uniform mat4 v_Projection;

void main()
{
    gl_Position = v_Projection * v_View * v_Model * vec4(aPos, 1.0);
    //ourColor = aColor; // ��ourColor����Ϊ���ǴӶ�����������õ���������ɫ
    TexCoord = aTexCoord;
}