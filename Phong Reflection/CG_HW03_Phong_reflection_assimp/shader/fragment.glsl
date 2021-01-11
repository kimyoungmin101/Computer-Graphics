#version 120                  // GLSL 1.20

uniform vec3 u_view_position_wc;      // the camera position in the world coordinate

uniform vec3 u_light_position_wc;     // the light position in the world coordinate
uniform vec4 u_light_ambient;         // L_a
uniform vec4 u_light_diffuse;         // L_d
uniform vec4 u_light_specular;        // L_s

uniform vec4 u_material_ambient;      // k_a
uniform vec4 u_material_diffuse;      // k_d
uniform vec4 u_material_specular;     // k_s
uniform float u_material_shininess;   // alpha

varying vec3 v_position_wc;   // per-vertex position in the world coordinate system (per-vertex output)
varying vec3 v_normal_wc;     // per-vertex normal in the world coordinate system (per-vertex output)


vec4 calc_color()
{
  vec4  color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

  vec3 n_wc = normalize(v_normal_wc);

  vec3 l_wc = normalize(u_light_position_wc - v_position_wc);
  vec3 Inc_wc = - l_wc;
  vec3 r_wc = reflect(Inc_wc, n_wc);

  vec3 v_wc = normalize(u_view_position_wc - v_position_wc);

  // ambient color
  color += u_material_ambient * u_light_ambient;

  // diffuse color
  float ndotl = max(0.0, dot(n_wc, l_wc));
  color += ndotl * u_material_diffuse * u_light_diffuse;

  // specular color
  float rdotv = max(0.0, dot(r_wc, v_wc));
  color += pow(rdotv, u_material_shininess) * u_material_specular * u_light_specular;

  return  color;
}


void main()
{
	gl_FragColor = calc_color();
}
