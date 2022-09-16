#version 400 core
layout (location = 0) in vec3 aPos;       // 位置变量的属性位置值为 0 
//layout (location = 1) in vec3 aColor;     // 颜色变量的属性位置值为 1
layout (location = 1) in vec2 aTexCoord;  // 纹理变量的属性位置为 2

//out vec3 ourColor;  // 向片段着色器输出一个颜色
out vec2 TexCoord;
uniform mat4 v_Model;
uniform mat4 v_View;
uniform mat4 v_Projection;

void main()
{
    gl_Position = v_Projection * v_View * v_Model * vec4(aPos, 1.0);
    //ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
    TexCoord = aTexCoord;
}