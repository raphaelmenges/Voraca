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

out float coord;

uniform mat4 uniformModel;
uniform mat4 uniformView;

uniform sampler1D uniformHistogram;

void main()
{
	coord = positionAttribute.x;
	vec4 position = vec4(positionAttribute.x, texture(uniformHistogram, coord).r * positionAttribute.y, positionAttribute.zw);
	position = uniformModel * position;
	position.x = clamp(position.x, 0, 1);
	position = uniformView * position;
	gl_Position = position;
	coord = coord * 0.25 + 0.1;
}
