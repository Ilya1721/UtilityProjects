#version 330 core

in vec2 vertexUV;
in vec4 offset0;
in vec4 offset1;
in vec4 offset2;

layout(location = 0) out vec2 fragColor;

uniform sampler2D frame;
uniform float threshold;
uniform float localContrastAdaptationFactor;

const vec3 LUMA = vec3(0.2126, 0.7152, 0.0722);

float getLuma(vec2 uv)
{
  return dot(textureLod(frame, uv, 0.0).rgb, LUMA);
}

void main()
{
  float L = getLuma(vertexUV);
  float Lleft = getLuma(offset0.xy);
  float Ltop = getLuma(offset0.zw);
  vec4 delta;
  delta.xy = abs(vec2(L) - vec2(Lleft, Ltop));
  vec2 edges = step(vec2(threshold), delta.xy);
  if (dot(edges, vec2(1.0)) == 0.0)
  {
    discard;
  }
  float Lright = getLuma(offset1.xy);
  float Lbottom = getLuma(offset1.zw);
  delta.zw = abs(vec2(L) - vec2(Lright, Lbottom));
  vec2 maxDelta = max(delta.xy, delta.zw);
  float Lleftleft = getLuma(offset2.xy);
  float Ltoptop = getLuma(offset2.zw);
  delta.zw = abs(vec2(Lleft, Ltop) - vec2(Lleftleft, Ltoptop));
  maxDelta = max(maxDelta, delta.zw);
  float finalDelta = max(maxDelta.x, maxDelta.y);
  edges *= step(finalDelta, localContrastAdaptationFactor * delta.xy);
  fragColor = edges;
}
