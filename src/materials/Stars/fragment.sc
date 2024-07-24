$input v_color0, v_pos, v_time

#include <bgfx_shader.sh>

#ifndef INSTANCING
uniform vec4 StarsColor;
#endif

void main() {
#ifndef INSTANCING
  vec4 color = v_color0;
  color.rgb *= StarsColor.rgb * v_color0.a;
  color.rgb *= sin(v_pos.x+v_time + cos(v_pos.y))*cos(3.*v_pos.y+v_time*1.4);
  gl_FragColor = color;
#else
  gl_FragColor = vec4(0.0,0.0,0.0,0.0);
#endif
}

