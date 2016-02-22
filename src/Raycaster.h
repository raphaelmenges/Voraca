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
 * Raycaster
 *--------------
 * Owner of raycasting shader. Is owned by RcManager
 * and filled with information by Renderer.
 *
 */

#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <vector>

#include "Logger.h"
#include "Shader.h"
#include "RaycasterProperties.h"
#include "VolumeCreator.h"
#include "Primitives.h"
#include "VolumeProperties.h"
#include "Utilities.h"

const GLboolean RAYCASTER_USE_ERT = GL_TRUE;
const GLboolean RAYCASTER_USE_LOCAL_ILLUMINATION = GL_FALSE;
const GLboolean RAYCASTER_USE_SHADOWS = GL_FALSE;
const GLboolean RAYCASTER_USE_JITTERING = GL_TRUE;
const GLboolean RAYCASTER_USE_EXTENT_PRESERVING_JITTERING = GL_FALSE;
const GLboolean RAYCASTER_USE_ESS = GL_TRUE;
const GLboolean RAYCASTER_USE_GRADIENT_ALPHA_MULTIPLIER = GL_TRUE;
const GLboolean RAYCASTER_USE_FRESNEL_ALPHA_MULTIPLIER = GL_TRUE;
const GLboolean RAYCASTER_USE_REFLECTION_COLOR_MULTIPLIER = GL_TRUE;
const GLboolean RAYCASTER_USE_EMISSION_COLOR_MULTIPLIER = GL_TRUE;
const GLboolean RAYCASTER_USE_NORMALS_OF_CLASSIFIED_DATA = GL_FALSE;
const GLboolean RAYCASTER_USE_EXTENT_AWARE_NORMALS = GL_FALSE;
const GLboolean RAYCASTER_USE_PREINTEGRATION = GL_FALSE;
const GLboolean RAYCASTER_USE_ADAPTIVE_SAMPLING = GL_FALSE;
const GLboolean RAYCASTER_USE_VOXEL_SPACED_SAMPLING = GL_FALSE;
const GLuint RAYCASTER_NOISE_RES = 64;

class Raycaster
{
public:
    Raycaster();
    ~Raycaster();

    /** Friends of this class */
    friend class RcCreator;

    void init(GLint handle, std::string name);
    void draw(
        GLint volumeTextureHandle,
        GLint importanceVolumeTextureHandle,
        glm::vec3 volumeScale,
        glm::vec3 volumeResolution,
        VolumeProperties volumeProperties,
        GLfloat aspectRatio,
        GLfloat fieldOfView,
        glm::vec3 cameraPosition,
        glm::mat4 viewMatrix,
        glm::vec3 sunDirection,
        glm::vec3 sunColor,
        GLfloat sunBrightness,
        glm::vec3 ambientColor,
        GLint colorAlphaFunctionHandle,
        GLint ambientSpecularFunctionHandle,
        GLint advancedFunctionHandle,
        GLint colorAlphaPreintegrationHandle,
        GLint ambientSpecularPreintegrationHandle,
        GLint advancedPreintegrationHandle,
        GLint tfResolution,
        GLint reflectionHandle,
        glm::vec3 volumeExtent,
        glm::vec3 volumeExtentOffset);

    /** Gett/set properties */
    RaycasterProperties getProperties() const;
    void setProperties(RaycasterProperties properties);

    /** Returns handle */
    GLint getHandle() const;

    /** Returns name */
    std::string getName() const;

    /** Renaming */
    void rename(std::string name);

    /** Getter/setter for defines */
    GLboolean getUseERT() const;
    void setUseERT(GLboolean useERT);
    GLboolean getUseLocalIllumination() const;
    void setUseLocalIllumination(GLboolean useLocalIllumination);
    GLboolean getUseShadows() const;
    void setUseShadows(GLboolean useShadows);
    GLboolean getUseJittering() const;
    void setUseJittering(GLboolean useJittering);
    GLboolean getUseExtentPreservingJittering() const;
    void setUseExtentPreservingJittering(GLboolean useExtentPreservingJittering);
    GLboolean getUseGradientAlphaMultiplier() const;
    void setUseGradientAlphaMultiplier(GLboolean useGradientAlphaMultiplier);
    GLboolean getUseFresnelAlphaMultiplier() const;
    void setUseFresnelAlphaMultiplier(GLboolean useFresnelAlphaMultiplier);
    GLboolean getUseReflectionColorMultiplier() const;
    void setUseReflectionColorMultiplier(GLboolean useReflectionColorMultiplier);
    GLboolean getUseEmissionColorMultiplier() const;
    void setUseEmissionColorMultiplier(GLboolean useEmissionColorMultiplier);
    GLboolean getUseSimpleESS() const;
    void setUseSimpleESS(GLboolean useSimpleESS);
    GLboolean getUseNormalsOfClassifiedData() const;
    void setUseNormalsOfClassifiedData(GLboolean useNormalsOfClassifiedData);
    GLboolean getUseExtentAwareNormals() const;
    void setUseExtentAwareNormals(GLboolean useExtentAwareNormals);
    GLboolean getUsePreintegration() const;
    void setUsePreintegration(GLboolean useColorAlphaPreintegration);
    GLboolean getUseAdaptiveSampling() const;
    void setUseAdaptiveSampling(GLboolean useAdaptiveSampling);
    GLboolean getUseVoxelSpacedSampling() const;
    void setUseVoxelSpacedSampling(GLboolean useVoxelSpacedSampling);

protected:
    /** Reload shader after initialization or change of a define */
    void reloadShader();

    /** Basics */
    GLint handle;
    std::string name;

    /** Shader with raycasting algorithm */
    Shader shader;

    /** General properties for raycaster */
    RaycasterProperties properties;

    /** Defines */
    GLboolean useERT;
    GLboolean useLocalIllumination;
    GLboolean useShadows;
    GLboolean useJittering;
    GLboolean useExtentPreservingJittering;
    GLboolean useSimpleESS;
    GLboolean useGradientAlphaMultiplier;
    GLboolean useFresnelAlphaMultiplier;
    GLboolean useReflectionColorMultiplier;
    GLboolean useEmissionColorMultiplier;
    GLboolean useNormalsOfClassifiedData;
    GLboolean useExtentAwareNormals;
    GLboolean usePreintegration;
    GLboolean useAdaptiveSampling;
    GLboolean useVoxelSpacedSampling;

    /** Max. only once per frame reload shader */
    GLboolean shaderShouldBeReloaded;

    /** Uniform handles */
    GLuint uniformModelScaleHandle;
    GLuint uniformModelRotationHandle;
    GLuint uniformViewHandle;
    GLuint uniformProjectionHandle;
    GLuint uniformCameraPosHandle;
    GLuint uniformVolumeHandle;
    GLuint uniformImportanceVolumeHandle;
    GLuint uniformColorAlphaHandle;
    GLuint uniformAmbientSpecularHandle;
    GLuint uniformReflectionHandle;
    GLuint uniformAdvancedHandle;
    GLuint uniformBasicInputHandle;
    GLuint uniformSunDirectionHandle;
    GLuint uniformSunColorHandle;
    GLuint uniformAmbientColorHandle;
    GLuint uniformNoiseHandle;
    GLuint uniformAdvancedInputHandle;
    GLuint uniformVolumeValueInformationHandle;
    GLuint uniformVolumeResolutionHandle;
    GLuint uniformMirrorUVWHandle;
    GLuint uniformColorAlphaPreintegrationHandle;
    GLuint uniformAmbientSpecularPreintegrationHandle;
    GLuint uniformAdvancedPreintegrationHandle;
    GLuint uniformVolumeExtentHandle;
    GLuint uniformVolumeExtentOffsetHandle;

    /** Vectors to fill uniforms */
    glm::vec4 basicInput;

    /** Handle to noise textur */
    GLuint noiseHandle;

    /** Calculate normal distributed random number */
    GLfloat calcNormalRand(GLfloat uA, GLfloat uB);
};

#endif
