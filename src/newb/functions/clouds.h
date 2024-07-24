#ifndef CLOUDS_H
#define CLOUDS_H

#include "noise.h"

// simple clouds 2D noise
float cloudNoise2D(vec2 p, highp float t, float rain) {
  t *= NL_CLOUD1_SPEED;
  p += t;
  p.x += sin(p.y*0.4 + t);

  vec2 p0 = floor(p);
  vec2 u = p-p0;
  u *= u*(3.0-2.0*u);
  vec2 v = 1.0-u;

  // rain transition
  vec2 d = vec2(
    NL_CLOUD1_DENSITY+0.09+0.17*rain,
    NL_CLOUD1_DENSITY+0.089+0.17*rain*rain);

  return v.y*(randt(p0,d)*v.x + randt(p0+vec2(1.0,0.0),d)*u.x) +
         u.y*(randt(p0+vec2(0.0,1.0),d)*v.x + randt(p0+vec2(1.0,1.0),d)*u.x);
}

// simple clouds
vec4 renderCloudsSimple(vec3 pos, highp float t, float rain, vec3 zenithCol, vec3 horizonCol, vec3 horizonEdgeCol, vec3 FOG_COLOR) {
  pos.xz *= NL_CLOUD1_SCALE;

  float cloudAlpha = cloudNoise2D(pos.xz, t, rain);
  float cloudShadow = cloudNoise2D(pos.xz*0.91, t, rain);

  vec4 color = vec4((getCloudTint1(rain, FOG_COLOR)), cloudAlpha);

  color.rgb += horizonEdgeCol;
  color.rgb *= 1.0 - 0.8*cloudShadow*step(0.0, pos.y);

  color.rgb += zenithCol*0.8;
  color.rgb *= 1.0 - 0.4*rain;

  return color;
}

// rounded clouds

// rounded clouds 3D density map
float cloudDf(vec3 pos, float rain) {
  vec2 p0 = floor(pos.xz);
  vec2 u = smoothstep(0.999*NL_CLOUD2_SHAPE, 1.0, pos.xz-p0);

  // rain transition
  vec2 t = vec2(
    NXF_CLOUD2_AMOUNT+0.1001+0.2*rain,
    NXF_CLOUD2_AMOUNT+0.1+0.2*rain*rain);

  float n = mix(
    mix(randt(p0, t),randt(p0+vec2(1.0,0.0), t), u.x),
    mix(randt(p0+vec2(0.0,1.0), t),randt(p0+vec2(1.0,1.0), t), u.x),
    u.y
  );

  // round y
  float b = 1.0 - 1.9*smoothstep(NL_CLOUD2_SHAPE, 2.0 - NL_CLOUD2_SHAPE, 1.8*abs(pos.y-0.5));
  return smoothstep(0.2, 1.0, n * b);
}

vec4 renderClouds(vec3 vDir, vec3 vPos, float rain, float time, vec3 fogCol, vec3 skyCol, vec3 FOG_COLOR) {

  float height = 7.0*mix(NL_CLOUD2_THICKNESS, NL_CLOUD2_RAIN_THICKNESS, rain);

  // scaled ray offset
  vec3 deltaP;
  deltaP.y = 1.0;
  deltaP.xyz = (NL_CLOUD2_SCALE*height)*vDir/(0.02+0.98*abs(vDir.y));
  //deltaP.xyz = (NL_CLOUD2_SCALE*height)*vDir.xyz;
  deltaP.y = abs(deltaP.y);

  // local cloud pos
  vec3 pos;
  pos.y = 0.0;
  pos.xz = NL_CLOUD2_SCALE*(vPos.xz + vec2(1.0,0.5)*(time*NL_CLOUD2_VELOCIY));
  pos += deltaP;

  deltaP /= -float(NL_CLOUD2_STEPS);

  // alpha, gradient
  vec2 d = vec2(0.0,1.0);
  for (int i=1; i<=NL_CLOUD2_STEPS; i++) {
    float m = cloudDf(pos, rain);

    d.x += m;
    d.y = mix(d.y, pos.y, m);
    pos += deltaP;
  }
  //d.x *= vDir.y*vDir.y;
  d.x = pow(d.x/(float(NL_CLOUD2_STEPS)/NL_CLOUD2_DENSITY + d.y), 5.0);
  d.x = smoothstep(0.2, 0.4, d.x);

  if (vPos.y > 0.0) { // view from bottom
    d.y = 1.0 - d.y;
  }

  d.y = 1.0 - 1.06*d.y*d.x;

  vec4 col = vec4(skyCol*0.6, d.x*NXF_CLOUD2_OPACITY);
  col.rgb += (getCloudTint1(rain, FOG_COLOR) + 1.5*fogCol)*d.y;
  col.rgb *= 1.0 - 0.5*rain;

  return col;
}

// aurora is rendered on clouds layer
// 1Sekon : "Finally!! it's get fixed! (aurora turn white when in the clouds layer before)"
#ifdef NL_AURORA
vec4 renderAurora(vec3 p, float t, float rain, vec3 FOG_COLOR) {
  t *= NL_AURORA_VELOCITY;
  p.xz *= NL_AURORA_SCALE;
  p.xz += 0.05*sin(p.x*6.0 + t);

  float d0 = sin(p.x*0.1 + 0.1*t + sin(p.z*0.15));
  float d1 = sin(p.z*0.1 - 0.1*t + sin(p.x*0.17));
  float d3 = cos(p.z*0.2 - 0.1*t + sin(p.x*0.16));
  float d2 = cos(p.z*0.1 + 1.0*cos(d3 + d0 + d1*1.0) + d1*2.0 + d0*1.0 * d3*1.3);
  d0 *= d0; d1 *= d1; d2 *= d2; d3 *= d3;
  d2 = d0/(1.0 + d2/NL_AURORA_WIDTH);

  float mask = (1.0-0.8*rain)*max(1.0 - 4.0*max(FOG_COLOR.b, FOG_COLOR.g), 0.0);
  return vec4(NL_AURORA*mix(mix(NL_AURORA_COL1, NL_AURORA_COL2,d1), NXF_AURORA_COL3, d3),1.0)*d2*mask;
}
#endif

#endif
