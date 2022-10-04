#version 400 core
out vec4 FragColor;

struct PointLight 
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;

uniform vec3 viewPos;
uniform samplerCube skybox;

// 点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result;
    // 点光源
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // 反射 - 物体反射周围环境
    // 提供观察方向向量和物体的法向量以计算反射向量
    // 使用漫反射height进行叠加？
    vec3 R = reflect(-viewDir, norm);
    vec3 textureSkybox = texture(texture_specular1, TexCoords).rgb;
    textureSkybox = textureSkybox * texture(skybox, R).rgb;
    FragColor = vec4(result, 1.0) + vec4(textureSkybox, 1.0);

    
//    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(lightDir, normal), 0.0);
    // 镜面光着色
    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 64);
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * distance * distance);
    // 合并结果
    vec3 textureDiffuse = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = light.ambient * textureDiffuse;
    vec3 diffuse = light.diffuse * diff * textureDiffuse;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}