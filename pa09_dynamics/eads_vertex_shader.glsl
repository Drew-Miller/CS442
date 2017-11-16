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
//
// Note that "towardsCamera" has been replaced by the following:
//

// If true, we're using an orthographic projection. If false, we're
// using a perspective transform.
uniform int useOrthographic;

// This is the equivalent of "towardsCamera", but it should only be
// used if "useOrthographic" is true.
uniform vec3 orthographicTowards;

// When using perspective (i.e. useOrthographic is false), the camera
// is at this position, so the "towards" vector is this value minus
// `vertexPosition`.
uniform vec3 cameraPosition;

// light properties
uniform int nLights;
uniform struct Light {
    vec3 irradiance;
    vec3 towards;
} light[10]; // in later GLSLs, this size can vary

//
// Note that `viewMatrix` has been replaced (in this shader only) by
// the following two matrices:

// This transforms `vertexPosition` into NDC for drawing purposes.
uniform mat4 modelViewProjectionMatrix;

// This transforms `vertexPosition` into world coordinates for
// lighting computation.
uniform mat4 worldMatrix;

uniform mat3 normalMatrix;    // transforms normals into world

// per-vertex inputs
in vec4 vertexPosition;
in vec3 vertexNormal;

smooth out vec4 interpolatedColor;
const float EPSILON = 1.0e-5; // for single precision

vec3 getRadiance(vec3 worldNormal, vec3 towardsCamera)
//
// ASSIGNMENT (PA09)
//
// To see how easy it is to create a function in GLSL, extract the
// loop in main() that computes the radiance (given `worldNormal` and
// `towardsCamera`) and make it this separate function which returns
// the emitted and reflected radiance.  (We'll make use of this
// function in a surprising way in the next PA.)
//
{
    vec3 radiance = emmitance;

    for(int i = 0; i < nLights; i++){

      vec3 reflectivity = ambientReflectivity;
      vec3 towardsLight = light[i].towards;
      vec3 irradiance = light[i].irradiance;

      // get the normal vec for the world and the light s.t. we can determine
      // the camera vec
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

    return radiance; // to be replaced
}

void main(void)
{
    //
    // Copy your previous (PA08) solution here.
    //

    //
    // ASSIGNMENT (PA09)
    //
    // Replace the code that did the light calculation here with an
    // expression that makes a single call to getRadiance(). This
    // should not change the image in any way.
    //

    vec4 worldPosition4 = worldMatrix * vertexPosition;

    // set the 'towardsCamera' value
    vec3 towardsCamera;

    if(useOrthographic == 1)
      towardsCamera = orthographicTowards;
    else{
      // get the convert the vec4 worldPosition to vec3
      vec3 worldPosition3 = vec3(worldPosition4.xyz);
      towardsCamera = cameraPosition - worldPosition3;
    }

    // normalize
    towardsCamera = normalize(towardsCamera);
    vec3 worldNormal = normalize(normalMatrix * vertexNormal);

    // use a function call instead of the loop
    vec3 radiance = getRadiance(worldNormal, toawrdsCamera);

    interpolatedColor = vec4(radiance, 1);;

#if 0 // debug
    interpolatedColor = vec4(vertexNormal, 1);
#endif

    // the position transform is trivial
    gl_Position = modelViewProjectionMatrix * vertexPosition;
}
