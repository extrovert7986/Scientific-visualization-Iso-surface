#version 430
in vec3 LightIntensity;

out vec4 FragColor;

void main(){
    if(gl_FrontFacing){
        FragColor = vec4(LightIntensity,1.0);
    }
    else FragColor = mix(vec4(LightIntensity,1.0),vec4(1.0,0.0,0.0,1.0),0.3);
   
    // FragColor = vec4(1.0,1.0,1.0,1.0);
}