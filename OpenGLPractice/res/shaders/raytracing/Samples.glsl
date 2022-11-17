vec2 unit_squre_sample(float i)
{
	/*uint sx = i + 152;
	uint sy = i * 100 + 317;
	return vec2(rand(sx), rand(sy));*/
	float x = random(i + 10);
	float y = random(i + 10000);
	return vec2(x, y);
}

vec3 unit_sphere_sample(inout uint state)
{
    float d, x, y, z;
    do 
    {
        x = rand(state) * 2.0 - 1.0;
        state += 1;
        y = rand(state) * 2.0 - 1.0;
        state += 3;
        z = rand(state) * 2.0 - 1.0;
        d = x * x + y * y + z * z;
    } while (d > 1.0);

    return vec3(x, y, z);
}

// Xi should use unit_square_sample() get
vec3 cos_weighted_hemisphere(vec2 Xi, float e) 
{
    float cos_phi = cos(2.0f * PI * Xi.x);
    float sin_phi = sin(2.0f * PI * Xi.x);

    float cos_theta = pow((1.0f - Xi.y), 1.0f / (e + 1.0));
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    // Generation Z based sphere
    float pu = sin_theta * cos_phi;
    float pv = sin_theta * sin_phi;
    float pw = cos_theta;
    vec3 H = vec3(pu, pv, pw);
    return H;
}

// align the input vector to Normal
vec3 sample_on_normal(vec3 inputSample, vec3 N) 
{
    // from tangent-space vector to world-space sample vector
    //vec3  up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 up = vec3(0.000001f, 1.0, 0);
    up = normalize(up);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    bitangent = normalize(bitangent);
    vec3 sampleVec = tangent * inputSample.x + bitangent * inputSample.y + N * inputSample.z;
    sampleVec = normalize(sampleVec);
    return sampleVec;
}