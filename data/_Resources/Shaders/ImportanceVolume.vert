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

layout (location = 0) in vec4 positionAttribute;
out vec3 position;
out vec3 direction;
out vec3 volumeExtentMin;
out vec3 volumeExtentMax;

uniform mat4 uniformModelScale;
uniform mat4 uniformModelRotation;
uniform mat4 uniformView;
uniform mat4 uniformProjection;
uniform mat4 uniformVolumeExtent;

uniform vec3 uniformCameraPos;
uniform vec3 uniformMirrorUVW;
uniform vec3 uniformVolumeExtentOffset;

void main()
{
	// Calculate vertex position afer volume clipping
	vec4 vertexPosition = (uniformVolumeExtent * positionAttribute) + vec4(uniformVolumeExtentOffset,0);
	vertexPosition =  clamp(vertexPosition, vec4(-0.5f,-0.5f,-0.5f,0), vec4(0.5f,0.5f,0.5f,1));

	// Output for rasterization
	gl_Position = uniformProjection * uniformView * uniformModelRotation * uniformModelScale * vertexPosition;

	// Start position for rays inclusive same clamping as above
	position = clamp((uniformVolumeExtent * positionAttribute).xyz + uniformVolumeExtentOffset, vec3(-0.5f,-0.5f,-0.5f), vec3(0.5f,0.5f,0.5f));

	// Direction in model space
	direction = position - (inverse(uniformModelRotation*uniformModelScale) * vec4(uniformCameraPos,1)).xyz;

	// Used geometry has extent from -0.5 to +0.5, one need 0..1
	position += 0.5;

	// Volume extent in model space (0..1)
	volumeExtentMin = (uniformVolumeExtent * vec4(-0.5,-0.5,-0.5,0)).xyz + 0.5 + uniformVolumeExtentOffset;
	volumeExtentMin = clamp(volumeExtentMin, vec3(0,0,0), vec3(1,1,1));
	volumeExtentMax = (uniformVolumeExtent * vec4(0.5,0.5,0.5,0)).xyz + 0.5 + uniformVolumeExtentOffset;
	volumeExtentMax = clamp(volumeExtentMax, vec3(0,0,0), vec3(1,1,1));

	// Mirror X
	if(uniformMirrorUVW.x == 1)
	{
		position.x = 1-position.x;
		direction.x = -direction.x;
	}

	// Mirror Y
	if(uniformMirrorUVW.y == 1)
	{
		position.y = 1-position.y;
		direction.y = -direction.y;
	}

	// Mirror Z
	if(uniformMirrorUVW.z == 1)
	{
		position.z = 1-position.z;
		direction.z = -direction.z;
	}
}
