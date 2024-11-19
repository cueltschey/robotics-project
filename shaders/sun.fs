 // Frag Shader for the sun
// Author - Sithila Singhabahu, 2018
// Project - Assignment - Solar System
#version 330
precision highp float;
// Variables for animation
uniform float pauto;
uniform float qauto;
uniform float rauto;
uniform float sauto;
/************************/
/*  Support Functions   */
/************************/
/// Noise function, sourced externally - see link.
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author: Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashima/webgl-noise
//
vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}
vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}
vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}
// Classic Perlin noise
float cnoise(vec2 P)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}
// Classic Perlin noise, periodic variant
float pnoise(vec2 P, vec2 rep)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}
/**********************************************/
#define SPLINELENGTH 7
vec4 spline (float value, vec4 cp [SPLINELENGTH])
{
  vec4 cp0, cp1, cp2, cp3;
  float t;
  if (value < 0.5)
  {
    if (value < 0.25)
      {
        cp0 =cp[0];
        cp1 =cp[1];
        cp2 =cp[2];
        cp3 =cp[3];
        t = value - 0.;
      }
    else
      {
        cp0 =cp[1];
        cp1 =cp[2];
        cp2 =cp[3];
        cp3 =cp[4];
        t = value - 0.25;
      }
  }
  else
  {
    if (value < 0.75)
      {
        cp0 =cp[2];
        cp1 =cp[3];
        cp2 =cp[4];
        cp3 =cp[5];
        t = value - 0.5;
      }
    else
      {
        cp0 =cp[3];
        cp1 =cp[4];
        cp2 =cp[5];
        cp3 =cp[6];
        t = value - 0.75;
      }
  }
  t = t * 4.;
  vec4 result = 0.5 * ((2.0 * cp1) + (-1.0 * cp0 + cp2) * t + (2.0 * cp0 - 5.0 * cp1 + 4.0 * cp2 - cp3) * t * t + (-1.0 * cp0 + 3.0 * cp1 - 3.0 * cp2 + cp3) * t * t * t);
  return result;
}
#define SFACTOR 0.5
#define TFACTOR 3.
#define BUMP_AMPLITUDE (20.0/100.0)
#define BUMP_FREQUENCY (5.0)
#define PI 3.14159
in VertexData
{
    vec3 normal;
    vec4 colour;
    vec2 st;
    vec4 EyeSpaceLightPosition;     
    vec4 EyeSpacePosition;          
    vec4 EyeSpaceObjectPosition;
    vec4 ShadowCoord;   
    float height;
    float terrx;
    float terry;            
} frag;
out vec4 out_fragmentColour;
// The dimensions (resolution) of the drawing buffer
const float RESOLUTION = 1024.0;
uniform vec2 u_resolution = vec2(RESOLUTION, RESOLUTION);
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
// Fragment shader code
void main()
{
  // The gl_FragCoords is in the range of the number of pixels
  // in the viewing window. Resize these such that they are
  vec2 st = gl_FragCoord.xy/u_resolution.xy;
  vec2 pos = (vec2(0.5)-st)*2;
  // Set up the texture coordinates
  float s = frag.st.x;
  float t = 1 - frag.st.y;  // Fix inverted texture (vertical axis)
  // Used to build the fragment colour
  vec4 colour = vec4 (0.0, 0.0, 0.0, 1.0);
  // Set up colours for sun surface.
  vec4 whitey = vec4 (1.0, 0.70, 0.1, 1.0);
  vec4 yellow = vec4 (0.9, 0.50, 0.0, 1.0);
  vec4 yelora = vec4 (0.9, 0.40, 0.0, 1.0);
  vec4 orange = vec4 (0.9, 0.30, 0.0, 1.0);
  vec4 redora = vec4 (0.9, 0.20, 0.0, 1.0);
  vec4 red = vec4 (1.0, 0.10, 0.0, 1.0);
  vec4 greora = vec4 (0.47, 0.43, 0.0, 1.0);
  vec4 oragre = vec4 (0.69, 0.37, 0.0, 1.0);
  vec4 black = vec4 (0.1, 0.1, 0.1, 1.0);
  vec4 baserange [SPLINELENGTH];
  baserange[0] = oragre;
  baserange[1] = oragre;
  baserange[2] = greora;
  baserange[3] = oragre;
  baserange[4] = redora;
  baserange[5] = red;
  baserange[6] = red;
  vec4 spotrange [SPLINELENGTH];
  spotrange [0] = oragre;
  spotrange [1] = black;
  spotrange [2] = whitey;
  spotrange [3] = black;
  spotrange [4] = black;
  spotrange [5] = black;
  spotrange [6] = yelora;
  vec4 markrange [SPLINELENGTH];
  markrange [0] = yellow;
  markrange [1] = red;
  markrange [2] = black;
  markrange [3] = red;
  markrange [4] = oragre;
  markrange [5] = red;
  markrange [6] = yellow;
  // Procedural Surface shading
  float label = 5;
  vec4 cs = vec4 (0.5, 0.25, 0.05, 1.0);
  float i = (1. + cnoise (vec2 (SFACTOR * sin (2.0 * PI * (s- pauto*0.004)) + PI + label, TFACTOR * t + 1.414 + label))) / 2.;
  cs = spline (i, baserange);
  vec4 spotcolour = spline (i*sin(pauto*0.008),spotrange);
  vec4 markcolour = spline (i, markrange);
 
  float darken = (1. + cnoise (vec2 (150. * ((s+ pauto*0.00008) + label), 150. * (t + label)))) / 2.;
  darken = pow (darken, 2.5);
  if(darken > 0.33)
  {
    cs = mix (cs, spotcolour, (darken- 0.33) * 1.5);
  }
  darken = (1. + pnoise (vec2 (30.0 * (s + label), 60.0 * (t + label)), vec2 (10., 10.))) / 2.;
  darken = pow (darken, 5);     
  darken = 0.5 + 0.5 * sin (PI * (darken - 0.5));
  cs = mix (cs, markcolour, darken);
  // Base texture panning over time
  vec4 BaseTexture = vec4 ( texture2D( texture1, vec2(s+pauto*0.00005,t)));
  // Combine all effects
  out_fragmentColour = BaseTexture+ clamp(cs, 0,0.5) + (vec4 (0.8, 0.2, 0.00, 1.0) * 0.2* vec4 ( texture2D( texture1, vec2(s+pauto*0.0005,t+pauto*0.0008))));
}
