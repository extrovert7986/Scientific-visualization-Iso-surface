#version 430

layout (location = 0)in vec3 vertexPosition;
layout (location = 1)in vec3 vertexNormal;

out vec3 LightIntensity;

struct LightInfo{
    vec4 position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
};
uniform LightInfo Light;

struct MaterialInfo{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shiness;
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

void getEyeSpace(out vec3 norm, out vec4 position){
    norm = normalize((ModelViewMatrix * vec4(vertexNormal,0.0)).xyz);
    position = ModelViewMatrix*vec4(vertexPosition,1.0);
}

vec3 phongModel(vec4 position ,vec3 norm){
    vec3 l = normalize(vec3(Light.position-position));
    vec3 v = normalize(-position.xyz);
    vec3 r = normalize(reflect(-l,norm));
    vec3 ambient = Light.La * Material.Ka;
    float vDotN = max(dot(v,norm),0.0);
    vec3 diffuse = Light.Ld * Material.Kd * vDotN;
    vec3 spec = vec3(0.0);
    if(vDotN>0.0)
        spec = Light.Ls*Material.Ks*pow(max(dot(r,v),0.0),Material.shiness);
    return ambient+diffuse+spec;

}

void main(){
    vec3 eyeNorm;
    vec4 eyePosition;

    getEyeSpace(eyeNorm,eyePosition);
    LightIntensity = phongModel(eyePosition,-eyeNorm);

    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertexPosition,1.0);
}