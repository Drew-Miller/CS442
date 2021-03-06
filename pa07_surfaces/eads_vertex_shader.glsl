#version 330

//
// This is the fundamental "EADS" (Emissive-Ambient-Diffuse-Specular)
// shader that is a somewhat simplified version of the traditional
// OpenGL shading model.
//

// material properties
uniform vec3 emittance;
uniform vec3 ambientReflectivity;
uniform vec3 maximumDiffuseReflectivity;
uniform vec3 maximumSpecularReflectivity;
uniform float specularExponent;

// camera propert(y|ies)
uniform vec3 towardsCamera;

// light properties
uniform int nLights;
uniform struct Light {
    vec3 irradiance;
    vec3 towards;
} light[10]; // in later GLSLs, this size can vary

uniform mat4 viewMatrix;      // transforms vertices into NDC

uniform mat3 normalMatrix;    // transforms normals into world

// per-vertex inputs
in vec4 vertexPosition;
in vec3 vertexNormal;

smooth out vec4 interpolatedColor;
const float EPSILON = 1.0e-5; // for single precision

void main(void)
{
    //
    // ASSIGNMENT (PA06)
    //
    // Modify your previous (PA05) solution to allow for multiple
    // lights using the lights[] array of structs, of which there are
    // `nLights`.  This means summing the contribution to
    // `reflectivity` of every light in a for-loop. The loop should
    // contain the contents of the "All other light sources"
    // pseudocode above. Be sure to set emittance only once, though
    // (before entering the loop, say), as it is intrinsic to the
    // object and does not come from a light.
    //

    // set this value before the loop
    vec3 radiance = emittance;

    for(int i = 0; i < nLights; i++){

      vec3 reflectivity = ambientReflectivity;
      vec3 towardsLight = light[i].towards;
      vec3 irradiance = light[i].irradiance;

      // get the normal vec for the world and the light s.t. we can determine
      // the camera vec
      vec3 worldNormal = normalize(vertexNormal * normalMatrix);
      vec3 towardsLight_ = normalize(towardsLight);

      float nDotL = dot(worldNormal, towardsLight_);

      if(nDotL > 0.0){
        reflectivity += (nDotL * maximumDiffuseReflectivity);

        vec3 h = normalize(towardsCamera + towardsLight_);

        float nDotH = dot(worldNormal, h);

        if(nDotH > 0.0){
          reflectivity += (maximumSpecularReflectivity * pow(nDotH, specularExponent));
        }
      }

      // add the reflectivity from that light source
      radiance += (irradiance * reflectivity);
    }

    // after the loop, radiance should be properly calculated
    interpolatedColor = vec4(radiance, 1);

    interpolatedColor = vec4(radiance, 1);
#if 0 // debug
    interpolatedColor = vec4(vertexNormal, 1);
#endif

    // the position transform is trivial
    gl_Position = viewMatrix * vertexPosition;
}
