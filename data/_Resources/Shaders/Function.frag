/**************************************************************************
 * Voraca 0.97 (VOlume RAy-CAster)
 **************************************************************************
 * Copyright (c) 2016, Raphael Philipp Menges
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************/

in float coord;
out vec4 fragmentColor;

uniform sampler1D uniformColorAlpha;
uniform sampler1D uniformAmbientSpecular;
uniform sampler1D uniformAdvanced;
uniform int uniformVisualizationState;
uniform vec2 uniformValueRange;
uniform float uniformOpacity;

void main()
{

	vec4 colorAlphaValue = vec4(texture(uniformColorAlpha, coord)).rgba;
	vec4 ambientSpecularValue = vec4(texture(uniformAmbientSpecular, coord)).rgba;
	vec4 advancedValue = vec4(texture(uniformAdvanced, coord)).rgba;

	vec3 finalColor;

	if(uniformVisualizationState == 0)
	{
		finalColor = colorAlphaValue.rgb;
	}
	else if(uniformVisualizationState == 1)
	{
		finalColor = ambientSpecularValue.rrr;
	}
	else if(uniformVisualizationState == 2)
	{
		finalColor = ambientSpecularValue.ggg;
	}
	else if(uniformVisualizationState == 3)
	{
		finalColor = ambientSpecularValue.bbb;
	}
	else if(uniformVisualizationState == 4)
	{
		finalColor = ambientSpecularValue.aaa;
	}
	else if(uniformVisualizationState == 5)
	{
		finalColor = advancedValue.rrr;
	}
	else if(uniformVisualizationState == 6)
	{
		finalColor = advancedValue.ggg;
	}
	else if(uniformVisualizationState == 7)
	{
		finalColor = advancedValue.bbb;
	}
	else if(uniformVisualizationState == 8)
	{
		finalColor = advancedValue.aaa;
	}

	fragmentColor = vec4((finalColor.rgb - uniformValueRange.x) / (uniformValueRange.y-uniformValueRange.x), uniformOpacity);
}
