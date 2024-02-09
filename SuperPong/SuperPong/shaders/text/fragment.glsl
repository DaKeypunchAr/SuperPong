#version 460 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D tex;
uniform vec4 color;

void main()
{
	vec4 sampled = vec4(texture(tex, texCoords).rgbr);
	fragColor = texture(tex, texCoords);
}