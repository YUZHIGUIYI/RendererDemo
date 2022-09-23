#version 400 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

// 核处理
const float offset = 1.0 / 300.0;

void main()
{
    //vec3 col = texture(screenTexture, TexCoords).rgb;
    //FragColor = vec4(col, 1.0);
    // 帧缓冲运用其一，再片段着色器中仅仅使用一行代码，就能让整个场景的颜色反相
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    // 其二，移除场景中除了黑白灰以外所有的颜色，让整个图像灰度化
    //FragColor = texture(screenTexture, TexCoords);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);
    // 其三，核效果 - 从纹理的其他地方采样颜色值。大部分核将所有的权重加起来之后都应该会等于1
    // 锐化核，它会采样周围的所有元素，锐化每个颜色值
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // 模糊核
//    float kernel[9] = float[](
//        1.0 / 16, 2.0 / 16, 1.0 / 16,
//        2.0 / 16, 4.0 / 16, 2.0 / 16,
//        1.0 / 16, 2.0 / 16, 1.0 / 16  
//    );
    // 边缘检测 - 这个核高亮了所有的边缘，而暗化了其他部分，在只关心图像的边角时有用
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