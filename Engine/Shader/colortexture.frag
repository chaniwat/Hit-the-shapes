#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform float spriteAlpha;

uniform int isTextured;

void main()
{    
    if (isTextured > 0)
    {
        color = vec4(spriteColor, spriteAlpha) * texture(image, TexCoords);
    }
    else
    {
        color = vec4(spriteColor, spriteAlpha);
    }
}