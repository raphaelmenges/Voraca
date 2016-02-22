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

#include "VolumeProperties.h"

VolumeProperties::VolumeProperties()
{
	reset();
}

VolumeProperties::~VolumeProperties()
{
}

void VolumeProperties::reset()
{
	voxelScaleMultiplier = VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER;
	valueOffset = VOLUMEPROPERTIES_VALUE_OFFSET;
	valueScale = VOLUMEPROPERTIES_VALUE_SCALE;
	eulerZXZRotation = VOLUMEPROPERTIES_EULER_ZXZ_ROTATION;
	mirrorX = VOLUMEPROPERTIES_MIRROR_X;
	mirrorY = VOLUMEPROPERTIES_MIRROR_Y;
	mirrorZ = VOLUMEPROPERTIES_MIRROR_Z;
	useLinearFiltering = VOLUMEPROPERTIES_USE_LINEAR_FILTERING;
}