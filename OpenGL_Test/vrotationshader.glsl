#version 150

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform vec3 theta;

void main() 
{
    // Compute the sines and cosines of theta for each of
    //   the three axes in one computation.
    vec3 angles = radians( theta );
    vec3 c = cos( angles );
    vec3 s = sin( angles );

    // Remember: these matrices are column-major
    
    // calculate the rotation over the x axis (x stays the same only y and z change)
    mat4 rx = mat4( 1.0,  0.0,  0.0, 0.0,
                    0.0,  c.x,  s.x, 0.0,
                    0.0, -s.x,  c.x, 0.0,
                    0.0,  0.0,  0.0, 1.0 );

    // calculate the rotation over the y axis (x stays the same only x and z change)
    mat4 ry = mat4( c.y, 0.0, -s.y, 0.0,
                    0.0, 1.0,  0.0, 0.0,
                    s.y, 0.0,  c.y, 0.0,
                    0.0, 0.0,  0.0, 1.0 );

    // Workaround for bug in ATI driver
    ry[1][0] = 0.0;
    ry[1][1] = 1.0;

    // calculate the rotation over the z axis (x stays the same only x and y change)
    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
                    s.z,  c.z, 0.0, 0.0,
                    0.0,  0.0, 1.0, 0.0,
                    0.0,  0.0, 0.0, 1.0 );

    // Workaround for bug in ATI driver
    rz[2][2] = 1.0;
    
    mat4 t = mat4(  1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    c.x, c.y, s.z, 1.0 );
    
    color = vColor;
    
    // apply to vertex
    gl_Position = rz * ry * rx * vPosition;
    //gl_Position = t * rz * ry * rx * vPosition;
    //gl_Position = vPosition;
} 
