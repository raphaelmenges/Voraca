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

/*
 * VolumeProperties
 *--------------
 * Properties of a volume.
 *
 */

#ifndef VOLUMEPROPERTIES_H_
#define VOLUMEPROPERTIES_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

const GLfloat VOLUMEPROPERTIES_VALUE_OFFSET = 0.0f;
const GLfloat VOLUMEPROPERTIES_VALUE_OFFSET_MIN = -1.0f;
const GLfloat VOLUMEPROPERTIES_VALUE_OFFSET_MAX = 1.0f;
const GLfloat VOLUMEPROPERTIES_VALUE_SCALE = 1.0f;
const GLfloat VOLUMEPROPERTIES_VALUE_SCALE_MIN = 1.0f;
const GLfloat VOLUMEPROPERTIES_VALUE_SCALE_MAX = 100.0f;
const glm::vec3 VOLUMEPROPERTIES_EULER_ZXZ_ROTATION(0,0,0);
const glm::vec3 VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MIN(0,0,0);
const glm::vec3 VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MAX(360,360,360);
const glm::vec3 VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER(1,1,1);
const glm::vec3 VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MIN(0,0,0);
const glm::vec3 VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MAX(10,10,10);
const GLboolean VOLUMEPROPERTIES_MIRROR_X = GL_FALSE;
const GLboolean VOLUMEPROPERTIES_MIRROR_Y = GL_FALSE;
const GLboolean VOLUMEPROPERTIES_MIRROR_Z = GL_FALSE;
const GLboolean VOLUMEPROPERTIES_USE_LINEAR_FILTERING = GL_TRUE;

class VolumeProperties
{
public:
    VolumeProperties();
    ~VolumeProperties();

    void reset();

    glm::vec3 voxelScaleMultiplier;
    GLfloat valueOffset;
    GLfloat valueScale;
    glm::vec3 eulerZXZRotation;
    GLboolean mirrorX;
    GLboolean mirrorY;
    GLboolean mirrorZ;

    GLboolean useLinearFiltering;
};

#endif
