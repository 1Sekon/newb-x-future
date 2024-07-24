#ifndef NXFCUSTOM_H
#define NXFCUSTOM_H

vec3 HorizonMask(vec3 bmd, vec3 zenith, vec3 viewDir)
{
  float f = 0.7*smoothstep(-0.5, 1.0, viewDir.y);
  return mix(bmd, zenith, f*f);
}

// tint rounded cloud
vec3 getCloudTint1(float rainFactor, vec3 FOG_COLOR) {
  vec3 col = mix(mix(NXF_DAY_CLOUD2_TINT, NXF_DAWN_CLOUD2_TINT, duskD(FOG_COLOR)), NXF_NIGHT_CLOUD2_TINT, nightD(FOG_COLOR));
  
  col = mix(col*(1.0+0.5*rainFactor), NXF_RAIN_CLOUD2_TINT*min(FOG_COLOR.g, 0.26)*13.2, rainFactor);
  
  return col;
}

float hash1(vec2 p){
  vec2 v = vec2(3.1415926*1453.0,exp(1.)*3054.0);
  return fract(sin(dot(p,v)*0.1)*4323.0);
}

vec3 nxfEndSpiral(vec3 pos, float t, vec3 horizonCol)
{
  vec2 nPos = pos.xz;
  nPos = vec2((atan2(nPos.y,nPos.x)) + t*0.6, sqrt(nPos.x*nPos.x + nPos.y*nPos.y));

  float shape = pow(1.0-nPos.y, 10.0)*10.0;
  shape = sin((nPos.x + pow(nPos.y, 0.2)*10.0) * 6.0) + shape - nPos.y*2.;
  vec3 col = horizonCol*max(0.0,shape)*0.7;
  
  return HorizonMask(vec3(0.0, 0.0, 0.0), col, pos);
}

void drawMeteor(inout vec3 col, in vec2 uv, vec2 startP, vec2 endP, float linWidth, float mask, vec2 sp, vec2 ep){

   uv *= 250.0;
   vec2 lineDir = endP - startP;
   vec2 fragDir = uv - startP;

   float lineCoe = clamp(dot(lineDir,fragDir)/dot(lineDir,lineDir),0.,1.0);

   vec2 projDir = lineCoe*lineDir;

   vec2 fragToLineDir = fragDir - projDir;

   float dis = length(fragToLineDir);
   float disToTail = length(projDir);
   dis = linWidth / dis;

   col += dis*vec3(0.8, 0.8, 0.8)*(disToTail*disToTail*disToTail)*mask;

}

void drawMeteors(inout vec3 col, vec2 uv, float time, float mask){

    vec2 dir = normalize(vec2(-1.0, -0.5));
    vec2 mv  = -dir*cos(mod(time*1.2,3.1415926))*90.0;
    vec2 sp  = vec2(10.0+100.0*hash1(vec2_splat(floor(time*1.2/3.1415926))),10.0);
    vec2 ep  = sp+dir*5.0;

    drawMeteor(col, uv, sp+mv, ep+mv, 0.0005, mask, sp, ep);

}

#endif