#version 330 core

layout(location=0) in vec2 v_uv;
layout(location=1) in uint v_color;
layout(location=2) in vec3 v_pos;

uniform mat4 u_projection;
uniform mat4 u_modelview;

out vec2 f_uv;

void main()
{
    f_uv = v_uv;
    gl_Position = u_projection * u_modelview * vec4(v_pos, 1.0f);
}
