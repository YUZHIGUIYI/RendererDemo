#version 400 core
out vec4 FragColor; 

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material material;

uniform vec3 viewPos;

// �����Դ
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
// ���Դ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// �۹�Դ
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    /*
    * ���Դ - model�ӽ�
    * �����Դ
    * ���Դ
    * �۹�Դ
    */
    // ����
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // �������
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // ���Դ
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    // �۹�
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    // �����(�Է���)
    vec3 emission = texture(material.emission, TexCoord).rgb;
    result += emission;

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // �����Դ����
    vec3 lightDir = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(lightDir, normal), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * distance * distance);
    // �ϲ����
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse * specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // �۹� - ƽ����Ե
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // �۹��ڲ����ռ���
    // ����˥��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * distance * distance);
    // ������
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    ambient *= attenuation;
    ambient *= intensity;
    // ������
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    diffuse *= attenuation;
    diffuse *= intensity;
    
    // �����
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (texture(material.specular, TexCoord).rgb);
    specular *= attenuation;
    specular *= intensity;

    return (ambient + diffuse + specular);
}