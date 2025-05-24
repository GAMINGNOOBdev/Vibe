#version 330 core

layout(location=0) in vec2 v_uv;
layout(location=1) in uint v_color;
layout(location=2) in vec3 v_pos;

uniform mat4 u_projection;
uniform mat4 u_modelview;

out vec2 f_uv;
out vec4 f_col;

void extract_color_from_int32(in uint color, out vec4 colorvec)
{
    float r = float((v_color & uint(0x000000FF))) / 255.f;
    float g = float((v_color & uint(0x0000FF00)) >> uint(8)) / 255.f;
    float b = float((v_color & uint(0x00FF0000)) >> uint(16)) / 255.f;
    float a = float((v_color & uint(0xFF000000)) >> uint(24)) / 255.f;
    colorvec = vec4(r,g,b,a);
}

void main()
{
    f_uv = v_uv;
    extract_color_from_int32(v_color, f_col);
    gl_Position = u_projection * u_modelview * vec4(v_pos, 1.0f);
}
