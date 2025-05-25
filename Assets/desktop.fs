#version 330 core

layout(location=0) out vec4 f_color;

in vec2 f_uv;
in vec4 f_col;

uniform uint u_texture_available;
uniform sampler2D u_texture;

void main()
{

    vec4 color = texture2D(u_texture, f_uv).rgba;
    if (u_texture_available == uint(0))
    {
        color = vec4(1);
    }

    if (color.a == 0)
        discard;

    f_color = f_col * color;
}