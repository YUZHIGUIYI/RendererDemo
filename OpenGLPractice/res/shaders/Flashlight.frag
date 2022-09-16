#version 400 core
out vec4 FragColor; 

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light
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

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
//uniform float v_Mix;

void main()
{
    /*
    * �ֵ�Ͳ�۹� - model�ӽ�
    * ��������
    * ��������
    * outCutOff - ƽ��/����Ե
    */
    // �۹� - ƽ����Ե
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // �۹��ڲ����ռ���
    // ����˥��
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * distance * distance);
    // ������
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    ambient *= attenuation;

    // ������
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    diffuse *= attenuation;
    diffuse *= intensity;
    
    // �����
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * (texture(material.specular, TexCoord).rgb);
    specular *= attenuation;
    specular *= intensity;

    // �����(�Է���)
    vec3 emission = texture(material.emission, TexCoord).rgb;

    vec3 result = ambient + diffuse + specular + emission;

    FragColor = vec4(result, 1.0);
}

