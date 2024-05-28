// Sets the GLSL's version and compatibility mode
#version 330 core

#define NR_LIGHT_POINTS 20

// Input variables
in vec4 vColour;
in vec3 vNormal;
in vec2 vTex;
in vec3 vFragmentPosition;

// Output variables
out vec4 oFragment;

// Structs
struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightDirection
{
    Light base;
    vec3 direction;
};

struct LightPoint
{
    Light base;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

struct LightSpot
{
    Light base;
    vec3 position;
    vec3 direction;
    float inner;
    float outer;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float intensity;
    float shininess;
};

// Uniforms
uniform vec3 uViewPosition;
uniform Material uMaterial;

uniform int uMaxLightPoints;
uniform LightDirection uLightDirectional;
uniform LightPoint uLightPoint[NR_LIGHT_POINTS];
uniform LightSpot uLightSpot;

// Function's prototypes
vec4 LightingDirectional(LightDirection light, vec3 normal, vec3 viewDirection);
vec4 LightingPoint(LightPoint light, vec3 normal, vec3 viewDirection, vec3 fragmentPosition);
vec4 LightingSpot(LightSpot light, vec3 normal, vec3 viewDirection, vec3 fragmentPosition);

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDirection = normalize(uViewPosition - vFragmentPosition);
    vec4 result = LightingDirectional(uLightDirectional, normal, viewDirection);

    for (int i = 0; i < uMaxLightPoints; i++)
        result += LightingPoint(uLightPoint[i], normal, viewDirection, vFragmentPosition);

    result += LightingSpot(uLightSpot, normal, viewDirection, vFragmentPosition);

    // Final fragment colour
    oFragment = result;
}

vec4 LightingDirectional(LightDirection light, vec3 normal, vec3 viewDirection)
{
    // Ambient
    vec4 ambient = vec4(light.base.ambient, 1.0) * texture(uMaterial.diffuse, vTex) * vColour;

    // Diffuse
    vec3 direction = normalize(light.direction);
    float diffuseFactor = max(dot(normal, direction), 0.0);
    vec4 diffuse = vec4(light.base.diffuse, 1.0) * diffuseFactor * (texture(uMaterial.diffuse, vTex) * vColour);

    // Specular
    vec3 reflectDirection = reflect(-direction, normal);
    float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
    vec4 specular = vec4(light.base.specular, 1.0) * specularFactor * (texture(uMaterial.specular, vTex).r * uMaterial.intensity);

    // Output
    return ambient + diffuse + specular;
}

vec4 LightingPoint(LightPoint light, vec3 normal, vec3 viewDirection, vec3 fragmentPosition)
{
    // Ambient
    vec4 ambient = vec4(light.base.ambient, 1.0) * texture(uMaterial.diffuse, vTex) * vColour;
    
    // Diffuse
    vec3 direction = normalize(light.position - fragmentPosition);
    float diffuseFactor = max(dot(normal, direction), 0.0);
    vec4 diffuse = vec4(light.base.diffuse, 1.0) * diffuseFactor * (texture(uMaterial.diffuse, vTex) * vColour);

    // Specular
    vec3 reflectDirection = reflect(-direction, normal);
    float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
    vec4 specular = vec4(light.base.specular, 1.0) * specularFactor * (texture(uMaterial.specular, vTex).r * uMaterial.intensity);

    float lightDistance = length(light.position - fragmentPosition);
    float lightAttenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

    ambient  *= lightAttenuation; 
    diffuse  *= lightAttenuation;
    specular *= lightAttenuation;

    // Output
    return ambient + diffuse + specular;
}

vec4 LightingSpot(LightSpot light, vec3 normal, vec3 viewDirection, vec3 fragmentPosition)
{

    // Ambient
    vec4 ambient = vec4(light.base.ambient, 1.0) * texture(uMaterial.diffuse, vTex) * vColour;
    
    // Diffuse
    vec3 direction = normalize(light.position - vFragmentPosition);
    float diffuseFactor = max(dot(normal, direction), 0.0);
    vec4 diffuse = vec4(light.base.diffuse, 1.0) * diffuseFactor * (texture(uMaterial.diffuse, vTex) * vColour);

    // Specular
    vec3 reflectDirection = reflect(-direction, normal);
    float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);
    vec4 specular = vec4(light.base.specular, 1.0) * specularFactor * (texture(uMaterial.specular, vTex).r * uMaterial.intensity);

    float theta = dot(light.direction, -direction);
    float intensity = clamp((theta - light.outer) / light.inner, 0.0, 1.0);

    diffuse  *= intensity;
    specular *= intensity;

    // Output
    return ambient + diffuse + specular;
}