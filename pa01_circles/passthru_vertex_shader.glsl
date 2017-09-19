#version 330

in vec4 vertexPosition;
out vec4 interpolatedColor; // interpolation happens *after* this shader

void main(void)
{
    gl_Position = vertexPosition;
    interpolatedColor = vec4(1.0, 1.0, 1.0, 1.0); // not really interpolated
}
