#version 330 core

in vec2 currentUV;

out vec4 fragColor;

uniform sampler2D prevFrame;
uniform sampler2D currFrame;
uniform sampler2D prevDepthMap;
uniform sampler2D currDepthMap;
uniform sampler2D motionVectors;

uniform bool isFirstFrame = true;
uniform vec2 screenSize;

float maxWeight = 0.8;
float minWeight = 0.05;
float maxSpeedPixels = 64.0;

vec3 getTexelColor(vec2 uv) 
{
    return texture(currFrame, uv).rgb;
}

vec3 varianceClipping(vec3 history)
{
    vec2 texelSize = 1.0 / screenSize;
    vec3 mean = vec3(0.0);
    vec3 meanSquare = vec3(0.0);
    for (int y = -1; y <= 1; ++y)
    {
      for (int x = -1; x <= 1; ++x)
      {
        vec3 color = texture(currFrame, currentUV + vec2(x, y) * texelSize).rgb;
        mean += color;
        meanSquare += color * color;
      }
    }
    mean /= 9.0;
    meanSquare /= 9.0;
    vec3 variance = max(meanSquare - mean * mean, vec3(0.0));
    vec3 sigma = sqrt(variance);
    const float gamma = 2.0;
    vec3 minColor = mean - gamma * sigma;
    vec3 maxColor = mean + gamma * sigma;
    return clamp(history, minColor, maxColor);
}

void main()
{
  vec2 motionUV = texture(motionVectors, currentUV).xy;
  float speedPixels = length(motionUV * screenSize);
  float motionFactor = clamp(1.0 - speedPixels / maxSpeedPixels, 0.0, 1.0);
  float blendFactor = mix(minWeight, maxWeight, motionFactor);
  vec2 prevUV = currentUV - motionUV;
  float currentDepth = texture(currDepthMap, currentUV).r;
  float prevDepth = texture(prevDepthMap, prevUV).r;
  bool sameDepth = abs(currentDepth - prevDepth) < currentDepth * 0.01;
  bool validPrevUV = prevUV.x >= 0.0 && prevUV.x <= 1.0 && prevUV.y >= 0.0 && prevUV.y <= 1.0;
  bool acceptHistory = !isFirstFrame && validPrevUV && sameDepth;
  float historyWeight = acceptHistory ? blendFactor : 0.0;
  vec4 currentColor = texture(currFrame, currentUV);
  vec4 historyColor = texture(prevFrame, prevUV);
  historyColor.rgb = varianceClipping(historyColor.rgb);
  fragColor = mix(currentColor, historyColor, historyWeight);
}