#version 120                  // GLSL 1.20

uniform mat4 u_PVM;           // Proj * View * Model
uniform mat4 u_M;             // Model

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;      // per-vertex normal 

varying vec3 v_position_wc;   // per-vertex position in the world coordinate system (per-vertex output)
varying vec3 v_normal_wc;     // per-vertex normal in the world coordinate system (per-vertex output)

void main()
{
  v_position_wc = (u_M * vec4(a_position, 1.0f)).xyz;
  v_normal_wc   = normalize((u_M * vec4(a_normal, 0.0f)).xyz);

  gl_Position = u_PVM * vec4(a_position, 1.0f);
}