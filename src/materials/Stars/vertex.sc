$input a_color0, a_position, a_texcoord0
$output v_color0, v_pos, v_time

#include <bgfx_shader.sh>

#ifndef INSTANCING
  uniform vec4 ViewPositionAndTime;
#endif

void main() {
#ifndef INSTANCING
  vec3 pos = a_position;
  vec3 worldPos = (u_model[0] * vec4(pos, 1.0)).xyz;
  v_color0 = a_color0;
  v_pos = worldPos;
  v_time = ViewPositionAndTime.w;
  gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));
#else
  gl_Position = vec4(0.0,0.0,0.0,0.0);
#endif
}
