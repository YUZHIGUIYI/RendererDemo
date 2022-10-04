#version 400 core
out vec4 FragColor;

//in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

//uniform sampler2D texture1;
uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{    
    //FragColor = texture(texture1, TexCoords);
    // 反射
//    vec3 I = normalize(Position - cameraPos);
//    vec3 R = reflect(I, normalize(Normal));
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);
    // 折射 - 斯涅尔定律 - 玻璃材质
    // 提供法向量、观察方向、两个材质之间的折射率
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}