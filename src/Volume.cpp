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

#include "Volume.h"

Volume::Volume()
{
    pivot = VOLUME_PIVOT;
}

Volume::~Volume()
{
    glDeleteTextures(1, &importanceVolumeTextureHandle);
    glDeleteTextures(1, &textureHandle);
    glDeleteTextures(1, &histogramTextureHandle);
    free(pRawData);
}

void Volume::init(
        GLint handle,
        std::string name,
        GLint textureHandle,
        glm::vec3 volumeResolution,
        glm::vec3 voxelScale,
        VolumeValueResolution valueResolution,
        GLubyte* pRawData)
{
    this->handle = handle;
    this->name = name;
    this->textureHandle = textureHandle;
    this->volumeResolution = volumeResolution;
    this->voxelScale = voxelScale;
    this->valueResolution = valueResolution;
    this->pRawData = pRawData;

    // Some logging for information
    LogInfo("Volume Resolution: " + UT::to_string(this->volumeResolution.x) +  " x " + UT::to_string(this->volumeResolution.y) +  " x " + UT::to_string(this->volumeResolution.z));

    if(this->valueResolution == VOLUME_8BIT)
    {
        LogInfo("Value Resolution: 8 BIT");
    }
    else if(this->valueResolution == VOLUME_16BIT)
    {
        LogInfo("Value Resolution: 16 BIT");
    }

    // Calculate rendering scale from input data
    renderingScale = scaleToMaximumOne(volumeResolution * voxelScale);

    // Create importance volume
    createImportanceVolume();

    // Create histogram
    createHistogram();

    LogInfo("Volume creation done");
}

VolumeValueResolution Volume::getValueResolution() const
{
    return valueResolution;
}

glm::vec3 Volume::getVolumeResolution() const
{
    return volumeResolution;
}

glm::vec3 Volume::getVoxelScale() const
{
    return voxelScale;
}

glm::vec3 Volume::getRenderingScale() const
{
    return renderingScale;
}

VolumeProperties Volume::getProperties() const
{
    return properties;
}

void Volume::setProperties(VolumeProperties properties)
{
    // Choose filtering
    if(properties.useLinearFiltering != this->properties.useLinearFiltering)
    {
        glBindTexture(GL_TEXTURE_3D, textureHandle);

        if(properties.useLinearFiltering)
        {
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }

        glBindTexture(GL_TEXTURE_3D, 0);
    }

    // Set properties
    this->properties = properties;
}

void Volume::setPivot(glm::vec3 pivot)
{
    // Clamping to voxel coordinates
    pivot = glm::clamp(pivot * volumeResolution, glm::vec3(0,0,0), volumeResolution-1.0f);
    pivot = glm::floor(pivot);

    GLdouble valueOfVoxel;
    GLuint positionInVolume =
        static_cast<GLuint>(pivot.x
        + pivot.y*volumeResolution.x
        + pivot.z*volumeResolution.y*volumeResolution.x);

    if(valueResolution == VOLUME_8BIT)
    {
        GLubyte value = pRawData[positionInVolume];
        valueOfVoxel = static_cast<GLdouble>(value) / 255.0;
    }
    else if(valueResolution == VOLUME_16BIT)
    {
        GLushort value = reinterpret_cast<GLushort*>(pRawData)[positionInVolume];
        valueOfVoxel = static_cast<GLdouble>(value) / 65535.0;
    }

    this->pivot = static_cast<GLfloat>(valueOfVoxel);
}

GLuint Volume::getTextureHandle() const
{
    return textureHandle;
}

GLuint Volume::getImportanceVolumeTextureHandle() const
{
    return importanceVolumeTextureHandle;
}

GLuint Volume::getHistogramTextureHandle() const
{
    return histogramTextureHandle;
}

GLint Volume::getHandle() const
{
    return handle;
}

GLfloat Volume::getPivot() const
{
    return pivot;
}

std::string Volume::getName() const
{
    return name;
}

void Volume::rename(std::string name)
{
    this->name = name;
}

glm::vec3 Volume::scaleToMaximumOne(glm::vec3 value)
{
    GLfloat maximum = glm::max(value.x, value.y);
    maximum = glm::max(maximum, value.z);

    return (value/maximum);
}

 void Volume::createImportanceVolume()
 {
    // Calculate resolution of importance volume
    GLuint xDim =  static_cast<GLuint>(volumeResolution.x/VOLUME_IMPORTANCE_VOLUME_DOWNSCALE);
    xDim = xDim > 1 ? xDim : 1;
    GLuint yDim =  static_cast<GLuint>(volumeResolution.y/VOLUME_IMPORTANCE_VOLUME_DOWNSCALE);
    yDim = yDim > 1 ? yDim : 1;
    GLuint zDim =  static_cast<GLuint>(volumeResolution.z/VOLUME_IMPORTANCE_VOLUME_DOWNSCALE);
    zDim = zDim > 1 ? zDim : 1;

    LogInfo("Resolution of importance volume: " + UT::to_string(xDim) + " x " + UT::to_string(yDim) + " x " + UT::to_string(zDim));

    // Some variabels which are needed
    GLfloat minVariance = std::numeric_limits<GLfloat>::max();
    GLfloat maxVariance = std::numeric_limits<GLfloat>::min();
    GLfloat variance, mean, valuesSum, squaredValuesSum, valueOfVoxel;
    GLuint i,j,k, valueCount;
    GLuint absolutePositionInBlock;

    GLuint rawDataSize = static_cast<GLuint>(volumeResolution.x*volumeResolution.y*volumeResolution.z);

    // Calc block sizes represented by one voxel of importance volume
    GLuint xBlockSize = static_cast<GLuint>(volumeResolution.x/xDim);
    GLuint yBlockSize = static_cast<GLuint>(volumeResolution.y/yDim);
    GLuint zBlockSize = static_cast<GLuint>(volumeResolution.z/zDim);

    // Create vector for variances
    std::vector<GLfloat> importanceVolumeData(xDim *  yDim * zDim);

    // Calculate variance per voxel of importance volume
    for(GLuint x = 0; x < xDim; x++)
    {
        for(GLuint y = 0; y < yDim; y++)
        {
            for(GLuint z = 0; z < zDim; z++)
            {
                valuesSum = 0;
                squaredValuesSum = 0;
                valueCount = 0;

                for(i = 0; i < xBlockSize; i++)
                {
                    for(j = 0; j < yBlockSize; j++)
                    {
                        for(k = 0; k < zBlockSize; k++)
                        {
                            // Absolute position in block in original volume (8 or 16BIT?????)
                            absolutePositionInBlock = static_cast<GLuint>(x * xBlockSize + i
                                                + (y * yBlockSize + j) * volumeResolution.x
                                                + (z * zBlockSize + k) * volumeResolution.x*volumeResolution.y);

                            // Don't go behind border of used memory
                            if(absolutePositionInBlock >= rawDataSize)
                            {
                                continue;
                            }

                            // Get raw values
                            if(valueResolution == VOLUME_8BIT)
                            {
                                GLubyte value = pRawData[absolutePositionInBlock];
                                valueOfVoxel = static_cast<GLfloat>(value);
                            }
                            else if(valueResolution == VOLUME_16BIT)
                            {
                                GLushort value = reinterpret_cast<GLushort*>(pRawData)[absolutePositionInBlock];
                                valueOfVoxel = static_cast<GLfloat>(value);
                            }

                            // Sum up values
                            valuesSum += static_cast<GLfloat>(valueOfVoxel);

                            // Sum up squared values
                            squaredValuesSum += static_cast<GLfloat>(valueOfVoxel * valueOfVoxel);

                            // Increment counter
                            valueCount++;
                        }
                    }
                }

                // Calculate mean of block
                mean = valuesSum/valueCount;

                // Calculate variance
                variance = (1.0f/valueCount) * squaredValuesSum - (mean*mean);

                // Save variance in vector
                importanceVolumeData[x + xDim*y + xDim*yDim*z] = variance;

                // New min or max variance found?
                minVariance = variance < minVariance ? variance : minVariance;
                maxVariance = variance > maxVariance ? variance : maxVariance;
            }
        }
    }

    // Normalize importance volume's values
    GLfloat range = maxVariance - minVariance;
    GLfloat importanceValue;

    for(GLuint x = 0; x < xDim; x++)
    {
        for(GLuint y = 0; y < yDim; y++)
        {
            for(GLuint z = 0; z < zDim; z++)
            {
                importanceValue = (importanceVolumeData[x + xDim*y + xDim*yDim*z] - minVariance)/range;
                importanceVolumeData[x + xDim*y + xDim*yDim*z] = glm::pow(importanceValue, VOLUME_IMPORTANCE_VOLUME_VALUE_POWER_CORRECTION);

            }
        }
    }

    // Fill texture
    glGenTextures(1, &importanceVolumeTextureHandle);
    glBindTexture(GL_TEXTURE_3D, importanceVolumeTextureHandle);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    if(VOLUME_IMPORTANCE_VOLUME_LINEAR_FILTERING)
    {
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, xDim, yDim, zDim, 0, GL_RED, GL_FLOAT, reinterpret_cast<GLfloat*> (&(importanceVolumeData[0])));
    glBindTexture(GL_TEXTURE_3D, 0);
 }

 void Volume::createHistogram()
 {
    GLdouble valueOfVoxel;
    GLuint voxelCount = static_cast<GLuint>(volumeResolution.x * volumeResolution.y * volumeResolution.z);
    GLuint bucket;
    GLdouble bucketSize = (1.0/static_cast<GLdouble>(VOLUME_HISTOGRAMM_BUCKET_COUNT-1));

    std::vector<GLuint> histogram(VOLUME_HISTOGRAMM_BUCKET_COUNT);

    // Create histogram
    for(GLuint i = 0; i < voxelCount; i++)
    {
        if(valueResolution == VOLUME_8BIT)
        {
            GLubyte value = pRawData[i];
            valueOfVoxel = static_cast<GLdouble>(value) / 255.0;
        }
        else if(valueResolution == VOLUME_16BIT)
        {
            GLushort value = reinterpret_cast<GLushort*>(pRawData)[i];
            valueOfVoxel = static_cast<GLdouble>(value) / 65535.0;
        }

        bucket = static_cast<GLuint>(valueOfVoxel / bucketSize);
        histogram[bucket] = histogram[bucket]+1;
    }

    // Get fullest bucket
    GLfloat maxBucket = 1;
    for(GLuint i = 0; i < VOLUME_HISTOGRAMM_BUCKET_COUNT; i++)
    {
        if(histogram[i] > maxBucket)
        {
            maxBucket = static_cast<GLfloat>(histogram[i]);
        }
    }

    // Create relative histogram
    std::vector<GLfloat> relativeHistogram(VOLUME_HISTOGRAMM_BUCKET_COUNT);
    GLfloat relativeValue;

    for(GLuint i = 0; i < VOLUME_HISTOGRAMM_BUCKET_COUNT; i++)
    {
        relativeValue = static_cast<GLfloat>(histogram[i])/maxBucket;

        // Adjust it with power
        relativeValue = glm::pow(relativeValue, VOLUME_HISTOGRAMM_VALUE_POWER_CORRECTION);

        relativeHistogram[i] = relativeValue;
    }

    // Fill texture
    glGenTextures(1, &histogramTextureHandle);
    glBindTexture(GL_TEXTURE_1D, histogramTextureHandle);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, VOLUME_HISTOGRAMM_BUCKET_COUNT, 0, GL_RED, GL_FLOAT, reinterpret_cast<GLfloat*> (&(relativeHistogram[0])));
    glBindTexture(GL_TEXTURE_1D, 0);

 }
