in vec3 vertexPosition;
in vec3 vertexColor;

uniform mat4 mvp;

out vec3 color;

void main()
{
    color = vertexColor;
    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
