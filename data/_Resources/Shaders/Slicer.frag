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

in vec2 texCoord;
out vec4 fragmentColor;

uniform sampler3D uniformVolume;
uniform int uniformDirection;
uniform float uniformPosition;
uniform vec2 uniformVolumeValueInformation;

void main()
{
	vec3 values;

	if(uniformDirection == 0)
	{
		// X-Direction
		values = texture(uniformVolume, vec3(uniformPosition, texCoord.yx)).rrr;

	}
	else if(uniformDirection == 1)
	{
		// Y-Direction
		values = texture(uniformVolume, vec3(texCoord.x, uniformPosition, texCoord.y)).rrr;
	}
	else
	{
		// Z-Direction
		values = texture(uniformVolume, vec3(texCoord.xy, uniformPosition)).rrr;
	}

	fragmentColor = vec4(max(values * uniformVolumeValueInformation.y + uniformVolumeValueInformation.x, vec3(0,0,0)), 1);
}
