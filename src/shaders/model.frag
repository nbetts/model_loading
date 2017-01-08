#version 330 core

struct Material {
  sampler2D diffuse1;
  sampler2D specular1;
  float shininess;
};

struct Light {
  vec4 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

in Data {
  vec4 position;
  vec3 normal;
  vec2 textureCoords;
  noperspective vec3 wireframeDistance;
} fragment;

out vec4 colour;

uniform Material material;
uniform Light light;
uniform vec4 viewPosition;
uniform vec4 wireframeColour;
uniform bool isWireframeEnabled;
uniform bool areFacesEnabled;

void main()
{
  vec3 lightDirection;
  bool wireframeEnabled = isWireframeEnabled;

  if (light.position.w == 0.0f) {
    lightDirection = vec3(normalize(light.position));
  } else {
    lightDirection = vec3(normalize(light.position - fragment.position));
  }
  
  vec3 viewDirection = vec3(normalize(viewPosition - fragment.position));
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);
  
  float diffuseStrength = max(dot(fragment.normal, lightDirection), 0.0f);
  float specularStrength = pow(max(dot(fragment.normal, halfwayDirection),
                                   0.0f), material.shininess);

  vec3 ambient  = light.ambient  * vec3(texture(material.diffuse1,
                                   fragment.textureCoords));
  vec3 diffuse  = light.diffuse  * (vec3(texture(material.diffuse1,
                                   fragment.textureCoords)) * diffuseStrength); 
  vec3 specular = light.specular * (vec3(texture(material.specular1,
                                   fragment.textureCoords)) * specularStrength);

  float dist = length(light.position - fragment.position);
  float attenuation = 1.0f / (light.constant + light.linear * dist + 
                              light.quadratic * (dist * dist));

  ambient  *= attenuation; 
  diffuse  *= attenuation;
  specular *= attenuation;

  vec4 baseColour = vec4(ambient + diffuse + specular, 1.0f);

  if (!areFacesEnabled) {
    wireframeEnabled = true;
  }

  if (wireframeEnabled) {
    vec3 fragPosition = smoothstep(vec3(0.0f),
                        fwidth(fragment.wireframeDistance),
                               fragment.wireframeDistance);

    float interpolatedColour = min(min(fragPosition.x, fragPosition.y),
                                   fragPosition.z);
    
    float alpha = areFacesEnabled ? baseColour.a : 1.0f - interpolatedColour;

    if (areFacesEnabled) {
      colour = vec4(mix(wireframeColour.rgb, baseColour.rgb,
                        interpolatedColour), alpha);
    } else {
      // colour = vec4(mix(baseColour.rgb, baseColour.rgb,
      colour = vec4(mix(wireframeColour.rgb, baseColour.rgb,
                        interpolatedColour), alpha);
    }
  } else {
    // colour = baseColour;
  }
}
