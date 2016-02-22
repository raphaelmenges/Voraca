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

in vec3 position;
in vec3 direction;
in vec3 volumeExtentMin;
in vec3 volumeExtentMax;
out vec4 fragmentColor;

uniform sampler3D uniformImportanceVolume;

const float stepSize = 0.008;
const float innerIterations = 5;

/** Calculate ray length */
float rayLength(vec3 pos, vec3 dir, vec3 volumeExtMin, vec3 volumeExtMax)
{
	// Initialization
	float rayLength = 100000;
	float t;

	if(dir.z != 0)
	{
		// Front
		t = (volumeExtMin.z - pos.z) / dir.z;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}

		// Back
		t = (volumeExtMax.z - pos.z) / dir.z;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}
	}

	if(dir.x != 0)
	{
		// Left
		t = (volumeExtMin.x - pos.x) / dir.x;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}

		// Right
		t = (volumeExtMax.x - pos.x) / dir.x;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}
	}

	if(dir.y != 0)
	{
		// Top
		t = (volumeExtMin.y - pos.y) / dir.y;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}

		// Button
		t = (volumeExtMax.y - pos.y) / dir.y;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}
	}

	return rayLength;
}

/** Main function */
void main()
{
	// Basic raycasting variables (all vectors are model spaced)
	vec3 dir = normalize(direction);
	vec3 pos = position;
	float src;
	float dst = 0;
	float currRayLength = 0;
	float maxRayLength = rayLength(pos, dir, volumeExtentMin, volumeExtentMax);
	float outerIterations = 2.0/(stepSize*innerIterations);

	// Sample along the ray
	for(int i = 0; i < outerIterations; i++)
	{
		for(int j = 0; j < innerIterations; j++)
		{
			// Get value from volume
			src = texture(uniformImportanceVolume, pos).r;

			// Compositing (MIP / X-Ray Effect)
			dst = max(dst, src);

			// Prepare for next sample
			pos += dir*stepSize;
			currRayLength += stepSize;
		}

		// Still inside volume?
		if(currRayLength >= maxRayLength)
		{
			break;
		}
	}

	// Output
	fragmentColor = vec4(dst, dst, dst, 1);
}
