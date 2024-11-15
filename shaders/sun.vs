 // Sun Vertex Shader
// Author - Sithila Singhabahu, 2018
// Project - Assignment - Solar System
#version 330 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexST;
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
float terrain (float x, float y, float levelsIntensity, float amplitudeIntensity, float freaquencyIntensity)
{
    int levels = int ((levelsIntensity + 2) * 3);
    float amplitudeFactor = pow (3, (amplitudeIntensity+1)/2.0f);
    float frequencyFactor = pow (3, (freaquencyIntensity+1.01f)/2.0f);
    float v = 0;
    float frequency = 1;
    float amplitude = 1;
    for (int i = 0; i < levels; i++)
    {
        v = v + amplitude * cnoise (
                  vec2 (frequency * x, 2*frequency * y));
        frequency = frequency * frequencyFactor;
        amplitude = amplitude * amplitudeFactor;
    }
    return v;
}
vec3 terrainNormal (float x, float y, float levelsIntensity, float amplitudeIntensity, float freaquencyIntensity)
{
    float delta = 0.5;
    vec3 A = vec3 (delta, terrain (x + delta, y, levelsIntensity, amplitudeIntensity, freaquencyIntensity) - terrain (x, y, levelsIntensity, amplitudeIntensity, freaquencyIntensity), 0);
    vec3 B = vec3 (0, terrain (x, y + delta, levelsIntensity, amplitudeIntensity, freaquencyIntensity) - terrain (x, y, levelsIntensity, amplitudeIntensity, freaquencyIntensity), delta);
    return normalize (cross (B,A));
}
// Variables
uniform vec4 LightPosition;
uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ShadowMatrix;
uniform float WorldSpeed;
// Variables for animation
uniform float pauto;
uniform float qauto;
uniform float rauto;
uniform float sauto;
uniform float randomFloat_1;
out VertexData
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
} VertexOut;
// Vertex sahder code
void main()
{
    vec4    Position;
    vec3    Normal;
    float levelsI = 0.0f;
    float amplitudeI = 0.0f;
    float freaquencyI = 0.0f;
    Position = vec4 (VertexPosition, 1);
    // Calculate the normal matrix and apply to normal vector...
    mat4 normalMatrix = transpose(inverse(ViewMatrix * WorldMatrix));
    Normal = VertexNormal;
    VertexOut.normal = (normalMatrix * vec4(Normal, 0)).xyz;
    VertexOut.colour = vec4(1.0,0.0,1.0,1.0);    // Testing - hard code colour to BLUE;
    VertexOut.st = VertexST;                    // Texture mapping coordinate as passed in
    // Populate  other important quantities, also in eye
    // coordinates. The position of the light.
    VertexOut.EyeSpaceLightPosition =  ViewMatrix * WorldMatrix * LightPosition;
    VertexOut.EyeSpacePosition = ViewMatrix * WorldMatrix * Position;
    VertexOut.EyeSpaceObjectPosition = ViewMatrix * WorldMatrix * vec4 (0, 0, 0, 1);
    VertexOut.ShadowCoord = ProjectionMatrix * ShadowMatrix * WorldMatrix * Position;
    VertexOut.ShadowCoord = VertexOut.ShadowCoord / VertexOut.ShadowCoord.w;
    VertexOut.ShadowCoord = (VertexOut.ShadowCoord + vec4 (1.,1.,1.,0.)) *
                               vec4 (1./2., 1./2., 1./2., 1);
    // Procedural Surface deformation 
    VertexOut.terrx = VertexPosition.x;
    VertexOut.terry = VertexPosition.z;
    // Deformation Offset over time  
    float OffsetIntensity = cos(pauto * 0.007) + cos(qauto * 0.0025 ) + cos(-qauto * 0.0025 + 0.0015);
    // Frequency of procedural deformation changed over time
    freaquencyI = 0.005* (freaquencyI + cos(pauto * 0.0005) - cos(-qauto * 0.00025 + 0.00015));
    VertexOut.height = terrain(VertexOut.terrx, VertexOut.terry, levelsI, amplitudeI, freaquencyI);
    Position.y = Position.y + 0.01*OffsetIntensity*((levelsI + 1.01f) * 0.1f * VertexOut.height * VertexOut.normal.y);
    Position.x = Position.x + 0.02*OffsetIntensity*((levelsI + 1.01f) * 0.1f * VertexOut.height * VertexOut.normal.x);
    Position.z = Position.z + 0.015*OffsetIntensity*((levelsI + 1.01f) * 0.1f * VertexOut.height * VertexOut.normal.z);
    // End of Procedural deformation
    // Populate the requried output of the Vertex Shader
    gl_Position = ProjectionMatrix * ViewMatrix * WorldMatrix * Position;
}
