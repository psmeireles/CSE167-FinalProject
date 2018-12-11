#version 330 core

in vec3 FragPos;
in vec3 FragNormal;

uniform vec3 viewPos;
uniform samplerCube skybox;

//Output variable
out vec4 color;

void main()
{
    
    //Arbitrary light properties (made up) - whitish
    vec3 light_color = vec3(0.9, 0.929, 0.929);
    vec3 light_vector = vec3(1.0, 1.0, -0.2);
    
    vec3 normal = normalize(FragNormal);
    vec3 normal_light = normalize(light_vector);
    
    // diffuse
    float brightness = max(dot(normal, normal_light),0.1);
    vec3 diffuse = brightness * light_color * 0.5;
    
    // specular
    vec3 normal_view = normalize(FragPos - viewPos);
    vec3 light_direction = -normal_light;
    vec3 normal_reflected = reflect(light_direction,normal);
    
    float specularFactor = max(dot(normal_reflected , normal_view),0.01);
    float damping = pow(specularFactor, 0.5);
    float reflectiveness = 0.1;
    vec3 specular = damping * reflectiveness * light_color;
    

    vec3 I = normalize(FragPos - viewPos);

    //https://www.quora.com/How-much-light-is-reflected-and-refracted-by-water-surface
    float refractRatio = 1.0/1.33;
    
    // Get reflection and refraction values
    vec3 reflection = reflect(I, normalize(FragNormal));
    vec3 refraction = refract(I, normalize(FragNormal), refractRatio);
    
    // Apply reflection/refraction to the skybox
    vec4 skyReflection = texture(skybox, reflection);
    vec4 skyRefraction = texture(skybox, refraction);
    

    // 50-50 mix of reflection and refraction
    float percent = 0.5;
    
    color = vec4(diffuse, 1.0) + vec4(specular, 1.0) + normalize(mix(skyReflection, skyRefraction, percent));

}

