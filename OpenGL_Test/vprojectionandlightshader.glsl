#version 150 

in  vec4 vPosition;
in  vec3 vNormal;
in  vec4 vColor;
out vec4 color;

uniform vec4 LightAmbient, LightDiffuse, LightSpecular;
uniform vec4 LightDiffuse2, LightSpecular2;
uniform vec4 MaterialAmbient, MaterialSpecular;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform vec4 LightPosition2;
uniform float Shininess;

void main() 
{
    // Transform vertex position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;
	vec3 lightPos = (ModelView * LightPosition).xyz;
	vec3 lightPos2 = (ModelView * LightPosition2).xyz;
	
    vec3 L = normalize( lightPos - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );
    
    vec3 L2 = normalize( lightPos2 - pos );
    vec3 H2 = normalize( L2 + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;

    // Compute terms in the illumination equation
    vec4 ambient = LightAmbient * MaterialAmbient;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*LightDiffuse*vColor;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * LightSpecular * MaterialSpecular;
    
    if( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 
    
    
    float Kd2 = max( dot(L2, N), 0.0 );
    vec4  diffuse2 = Kd2*LightDiffuse2*vColor;

    float Ks2 = pow( max(dot(N, H2), 0.0), Shininess );
    vec4  specular2 = Ks2 * LightSpecular2 * MaterialSpecular;
    
    if( dot(L2, N) < 0.0 ) {
        specular2 = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = Projection * ModelView * vPosition;
    
    
    vec4 tempColor1 = ambient + diffuse + specular;
    vec4 tempColor2 = ambient + diffuse2 + specular2;
    
    color = tempColor1 + tempColor2;
    color.a = 1.0;
} 
