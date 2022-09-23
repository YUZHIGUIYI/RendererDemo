#version 400 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

// �˴���
const float offset = 1.0 / 300.0;

void main()
{
    //vec3 col = texture(screenTexture, TexCoords).rgb;
    //FragColor = vec4(col, 1.0);
    // ֡����������һ����Ƭ����ɫ���н���ʹ��һ�д��룬������������������ɫ����
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    // ������Ƴ������г��˺ڰ׻��������е���ɫ��������ͼ��ҶȻ�
    //FragColor = texture(screenTexture, TexCoords);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);
    // ��������Ч�� - ������������ط�������ɫֵ���󲿷ֺ˽����е�Ȩ�ؼ�����֮��Ӧ�û����1
    // �񻯺ˣ����������Χ������Ԫ�أ���ÿ����ɫֵ
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // ģ����
//    float kernel[9] = float[](
//        1.0 / 16, 2.0 / 16, 1.0 / 16,
//        2.0 / 16, 4.0 / 16, 2.0 / 16,
//        1.0 / 16, 2.0 / 16, 1.0 / 16  
//    );
    // ��Ե��� - ����˸��������еı�Ե�����������������֣���ֻ����ͼ��ı߽�ʱ����
//    float kernel[9] = float[](
//        1,  1,  1,
//        1, -8,  1,
//        1,  1,  1
//    )

    vec3 sampleTex[9];
    for (int i = 0; i < 9; ++i)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; ++i)
    {
        col += sampleTex[i] * kernel[i];
    }

    FragColor = vec4(col, 1.0);
}   