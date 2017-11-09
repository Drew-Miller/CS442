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
    // Modify the code that sets a single light in PA05 to allow up to
    // 10 lights. In a loop, set all components of the uniform `light`
    // structs (see "eads_vertex_shader.glsl") for each light, as well
    // as the uniform int `nLights`.
    //
    // To enable selective lighting, include the following code in
    // the loop:
    //
    //     // If the light hedgehog is being drawn, set all irradiances
    //     // except that of the light being drawn (`iLight`) to black.
    //     if (controller.lightHedgehogIndex == LIGHT_HEDGEHOG_DISABLED
    //             || iLight == controller.lightHedgehogIndex)
    //         setUniform(nameBuffer, light->irradiance);
    //     else
    //         setUniform(nameBuffer, blackColor);
    //
    // `iLight` is the index of the light (from 0 to `nLights`-1).
    // `nameBuffer` is the (string) name of the irradiance of the
    // light being set, e.g. "light[1].irradiance" for `iLight` = 1.
    // If the light hedgehog is being drawn, this will darken all but
    // light `iLight`.
    //
    // Suggestion: Assert that there are no more than 10 lights (see
    // the shader source).
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
