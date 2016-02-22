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
 * Volume
 *--------------
 * Holds volume data, texture data, histogram and
 * importance volume.
 *
 */

#ifndef VOLUME_H_
#define VOLUME_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <limits>

#include "Logger.h"
#include "VolumeProperties.h"
#include "Utilities.h"

const GLint VOLUME_IMPORTANCE_VOLUME_DOWNSCALE = 4;
const GLboolean VOLUME_IMPORTANCE_VOLUME_LINEAR_FILTERING = GL_TRUE;
const GLuint VOLUME_HISTOGRAMM_BUCKET_COUNT = 256;
const GLfloat VOLUME_HISTOGRAMM_VALUE_POWER_CORRECTION = 0.25f;
const GLfloat VOLUME_IMPORTANCE_VOLUME_VALUE_POWER_CORRECTION = 0.5f;
const GLfloat VOLUME_PIVOT = 0.5f;

enum VolumeValueResolution
{
    VOLUME_8BIT, VOLUME_16BIT
};

class Volume
{
public:
    Volume();
    ~Volume();

    /** Friends of this class */
    friend class VolumeCreator;

    void init(
        GLint handle,
        std::string name,
        GLint textureHandle,
        glm::vec3 volumeResolution,
        glm::vec3 voxelScale,
        VolumeValueResolution valueResolution,
        GLubyte* pRawData);

    /** Get value resolution */
    VolumeValueResolution getValueResolution() const;

    /** Get volume resolution */
    glm::vec3 getVolumeResolution() const;

    /** Get voxel scale */
    glm::vec3 getVoxelScale() const;

    /** Get rendering scale */
    glm::vec3 getRenderingScale() const;

    /** Get properties */
    VolumeProperties getProperties() const;

    /** Set properties */
    void setProperties(VolumeProperties properties);

    /** Set pivot */
    void setPivot(glm::vec3 pivot);

    /** Returns texture handle */
    GLuint getTextureHandle() const;

    /** Returns importance volume texture handle */
    GLuint getImportanceVolumeTextureHandle() const;

    /** Returns histogram texture handle */
    GLuint getHistogramTextureHandle() const;

    /** Returns handle */
    GLint getHandle() const;

    /** Get position of pivot in values */
    GLfloat getPivot() const;

    /** Returns name */
    std::string getName() const;

    /** Renaming */
    void rename(std::string name);

protected:
    /** Scales vec3 per component, maximum per component is one */
    glm::vec3 scaleToMaximumOne(glm::vec3 value);

    /** Create importance volume by using variance of values */
    void createImportanceVolume();

    /** Creates histogram */
    void createHistogram();

    /** Basics */
    GLint handle;
    std::string name;
    GLfloat pivot;

    /** Handle to OpenGL texture */
    GLuint textureHandle;

    /** Volume resolution */
    glm::vec3 volumeResolution;

    /** Scale of voxels */
    glm::vec3 voxelScale;

    /** 8Bit oder 16Bit depth */
    VolumeValueResolution valueResolution;

    /** Raw data is saved for saving as xml */
    GLubyte* pRawData;

    /** Handle to texture of importance volume */
    GLuint importanceVolumeTextureHandle;

    /** Handle to texture of histogram */
    GLuint histogramTextureHandle;

    /** Rendering scale */
    glm::vec3 renderingScale;

    /** Properties */
    VolumeProperties properties;
};

#endif
