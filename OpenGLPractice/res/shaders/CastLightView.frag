#version 400 core
out vec4 FragColor; 

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float v_Mix;

void main()
{
    /*
    * ∑Î œπ‚’’ - view ”Ω«
    */
    FragColor = mix(texture(texture1, TexCoord), 
        texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), v_Mix);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos);  // the viewer is always at (0.0, 0.0, 0.0) in view-space
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = ambient + diffuse + specular;

    //FragColor = vec4(result, 1.0) * vec4(1.0, 0.5, 0.3, 1.0);
    FragColor = FragColor * vec4(result, 1.0);
}