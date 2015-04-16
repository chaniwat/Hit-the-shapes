/*#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform float spriteAlpha;

void main()
{    
    color = vec4(spriteColor, spriteAlpha) * texture(image, TexCoords);
}*/

#version 330 core
in vec2 TexCoords;
out vec4 color;

const mat3 rgb2yiq = mat3(0.299, 0.587, 0.114, 0.595716, -0.274453, -0.321263, 0.211456, -0.522591, 0.311135);
const mat3 yiq2rgb = mat3(1.0, 0.9563, 0.6210, 1.0, -0.2721, -0.6474, 1.0, -1.1070, 1.7046);

uniform sampler2D image;
uniform vec3 spriteColor;
uniform float spriteAlpha;
uniform float hue;

void main()
{
    vec4 loadcolor = texture(image, TexCoords);
    vec3 yColor = rgb2yiq * loadcolor.rgb; 

    float originalHue = atan(yColor.b, yColor.g);
    float finalHue = originalHue + hue;

    float chroma = sqrt(yColor.b*yColor.b+yColor.g*yColor.g);

    vec3 yFinalColor = vec3(yColor.r, chroma * cos(finalHue), chroma * sin(finalHue));
    vec4 outputColor = vec4(yiq2rgb*yFinalColor, 1.0);

    outputColor.a = loadcolor.a;

    color = vec4(spriteColor, spriteAlpha) * outputColor;
}