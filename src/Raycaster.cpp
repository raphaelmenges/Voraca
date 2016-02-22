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

#include "Raycaster.h"

Raycaster::Raycaster()
{
    useERT = RAYCASTER_USE_ERT;
    useLocalIllumination = RAYCASTER_USE_LOCAL_ILLUMINATION;
    useShadows = RAYCASTER_USE_SHADOWS;
    useJittering = RAYCASTER_USE_JITTERING;
    useExtentPreservingJittering = RAYCASTER_USE_EXTENT_PRESERVING_JITTERING,
    useSimpleESS = RAYCASTER_USE_ESS;
    useGradientAlphaMultiplier = RAYCASTER_USE_GRADIENT_ALPHA_MULTIPLIER;
    useFresnelAlphaMultiplier = RAYCASTER_USE_FRESNEL_ALPHA_MULTIPLIER;
    useReflectionColorMultiplier = RAYCASTER_USE_REFLECTION_COLOR_MULTIPLIER;
    useEmissionColorMultiplier = RAYCASTER_USE_EMISSION_COLOR_MULTIPLIER;
    useNormalsOfClassifiedData = RAYCASTER_USE_NORMALS_OF_CLASSIFIED_DATA;
    useExtentAwareNormals = RAYCASTER_USE_EXTENT_AWARE_NORMALS;
    usePreintegration = RAYCASTER_USE_PREINTEGRATION;
    useAdaptiveSampling = RAYCASTER_USE_ADAPTIVE_SAMPLING;
    useVoxelSpacedSampling = RAYCASTER_USE_VOXEL_SPACED_SAMPLING;
}

Raycaster::~Raycaster()
{
    glDeleteTextures(1, &noiseHandle);
}

void Raycaster::init(GLint handle, std::string name)
{
    this->handle = handle;
    this->name = name;

    shaderShouldBeReloaded = GL_TRUE;

    // Create noise
    std::vector<GLubyte> noise;
    for(GLint i = 0; i < RAYCASTER_NOISE_RES*RAYCASTER_NOISE_RES; i++)
    {
        GLfloat rand = calcNormalRand((static_cast<GLfloat>(std::rand())/RAND_MAX), (static_cast<GLfloat>(std::rand())/RAND_MAX));
        noise.push_back(static_cast<GLubyte>(255*rand));
    }
    glGenTextures(1, &noiseHandle);
    glBindTexture(GL_TEXTURE_2D, noiseHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, RAYCASTER_NOISE_RES, RAYCASTER_NOISE_RES, 0, GL_RED, GL_UNSIGNED_BYTE, reinterpret_cast<GLubyte*> (&noise[0]));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Raycaster::draw(
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
        glm::vec3 volumeExtentOffset)
{
    if(shaderShouldBeReloaded)
    {
        reloadShader();
        shaderShouldBeReloaded = GL_FALSE;
    }

    // basicInput.x has many jobs
    // Standard:				stepSize
    // Adaptive Sampling:		minAdaptiveStepSize
    // Voxel Spaced Samp.:		stepSizeMultiplier
    // Ad. Voxel Spaced Samp.:	minAdaptiveStepSizeMultiplier

    // Set basic input (need not to be set each frame..)
    glm::vec4 basicInput;
    GLfloat stepSize;
    GLfloat minAdaptiveStepSize;
    GLfloat maxAdaptiveStepSize;

    // When voxel spaced sampling is used, step size depends on direction and volume
    if(!useVoxelSpacedSampling)
    {
        stepSize = properties.stepSize;
        minAdaptiveStepSize = properties.minAdaptiveStepSize;
        maxAdaptiveStepSize = properties.maxAdaptiveStepSize;
    }
    else
    {
        stepSize = 1;
        minAdaptiveStepSize = 1;
        maxAdaptiveStepSize = 1;
    }

    if(useAdaptiveSampling)
    {
        // Max adaptive step size is filled into advanced input's w value
        basicInput.x = minAdaptiveStepSize * properties.minAdaptiveStepSizeMultiplier;
    }
    else
    {
        basicInput.x = stepSize * properties.stepSizeMultiplier;
    }
    basicInput.y = static_cast<GLfloat>(properties.outerIterations);
    basicInput.z = static_cast<GLfloat>(properties.innerIterations);
    basicInput.w = properties.alphaThreshold;

    // Set advanced input
    glm::vec4 advancedInput;
    advancedInput.x = properties.jitteringRangeMultiplier;
    advancedInput.y = properties.normalRangeMultiplier;
    advancedInput.z = properties.fresnelPower;
    advancedInput.w = maxAdaptiveStepSize * properties.maxAdaptiveStepSizeMultiplier;

    // Set volume value information
    glm::vec2 volumeValueInformation;
    volumeValueInformation.x = volumeProperties.valueOffset;
    volumeValueInformation.y = volumeProperties.valueScale;

    glm::vec3 mirrorUVW(0,0,0);
    if(volumeProperties.mirrorX)
    {
        mirrorUVW.x = 1;
    }
    if(volumeProperties.mirrorY)
    {
        mirrorUVW.y = 1;
    }
    if(volumeProperties.mirrorZ)
    {
        mirrorUVW.z = 1;
    }

    // Compute model matrix
    glm::mat4 modelScaleMatrix = glm::mat4(1.0f);
    glm::mat4 modelRotationMatrix = glm::mat4(1.0f);
    modelScaleMatrix = glm::scale(modelScaleMatrix,	volumeProperties.voxelScaleMultiplier * volumeScale);
    modelRotationMatrix = glm::rotate(modelRotationMatrix, glm::radians(volumeProperties.eulerZXZRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelRotationMatrix = glm::rotate(modelRotationMatrix, glm::radians(volumeProperties.eulerZXZRotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
    modelRotationMatrix = glm::rotate(modelRotationMatrix, glm::radians(volumeProperties.eulerZXZRotation.x), glm::vec3(0.0f, 0.0f, 1.0f));

    // Compute projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.f);

    // Use program because setUniform needs ist
    shader.use();

    // Set matrices and vectors
    shader.setUniformValue(uniformModelScaleHandle, modelScaleMatrix);
    shader.setUniformValue(uniformModelRotationHandle, modelRotationMatrix);
    shader.setUniformValue(uniformViewHandle, viewMatrix);
    shader.setUniformValue(uniformProjectionHandle, projectionMatrix);
    shader.setUniformValue(uniformCameraPosHandle, cameraPosition);
    shader.setUniformValue(uniformBasicInputHandle, basicInput);
    shader.setUniformValue(uniformAdvancedInputHandle, advancedInput);
    shader.setUniformValue(uniformVolumeValueInformationHandle, volumeValueInformation);
    shader.setUniformValue(uniformMirrorUVWHandle, mirrorUVW);

    // Volume clipping
    glm::mat4 volumeExtentMatrix = glm::mat4(1.0f);
    volumeExtentMatrix = glm::scale(volumeExtentMatrix,	volumeExtent);
    shader.setUniformValue(uniformVolumeExtentHandle, volumeExtentMatrix);
    shader.setUniformValue(uniformVolumeExtentOffsetHandle, volumeExtentOffset);

    // Set up depending on defines
    shader.setUniformTexture(uniformVolumeHandle, volumeTextureHandle, GL_TEXTURE_3D);

    if(usePreintegration)
    {
        shader.setUniformTexture(uniformColorAlphaPreintegrationHandle, colorAlphaPreintegrationHandle, GL_TEXTURE_2D);
    }
    else
    {
        shader.setUniformTexture(uniformColorAlphaHandle, colorAlphaFunctionHandle, GL_TEXTURE_1D);
    }

    if(useGradientAlphaMultiplier || useFresnelAlphaMultiplier || useReflectionColorMultiplier || useEmissionColorMultiplier)
    {
        if(usePreintegration)
        {
            shader.setUniformTexture(uniformAdvancedPreintegrationHandle, advancedPreintegrationHandle, GL_TEXTURE_2D);
        }
        else
        {
            shader.setUniformTexture(uniformAdvancedHandle, advancedFunctionHandle, GL_TEXTURE_1D);
        }
    }

    if(useReflectionColorMultiplier)
    {
        shader.setUniformTexture(uniformReflectionHandle, reflectionHandle, GL_TEXTURE_2D);
    }

    if(useLocalIllumination)
    {
        if(usePreintegration)
        {
            shader.setUniformTexture(uniformAmbientSpecularPreintegrationHandle, ambientSpecularPreintegrationHandle, GL_TEXTURE_2D);
        }
        else
        {
            shader.setUniformTexture(uniformAmbientSpecularHandle, ambientSpecularFunctionHandle, GL_TEXTURE_1D);
        }
        shader.setUniformValue(uniformSunDirectionHandle, sunDirection);
        shader.setUniformValue(uniformSunColorHandle, glm::vec4(sunColor, sunBrightness));
        shader.setUniformValue(uniformAmbientColorHandle, ambientColor);
    }

    if(useJittering)
    {
        shader.setUniformTexture(uniformNoiseHandle, noiseHandle, GL_TEXTURE_2D);
    }

    if(useAdaptiveSampling)
    {
        shader.setUniformTexture(uniformImportanceVolumeHandle, importanceVolumeTextureHandle, GL_TEXTURE_3D);
    }

    if(useVoxelSpacedSampling)
    {
        shader.setUniformValue(uniformVolumeResolutionHandle, volumeResolution);
    }

    // Draw it
    shader.draw(GL_TRIANGLES);
}


RaycasterProperties Raycaster::getProperties() const
{
    return properties;
}

void Raycaster::setProperties(RaycasterProperties properties)
{
    this->properties = properties;
}

GLint Raycaster::getHandle() const
{
    return handle;
}

std::string Raycaster::getName() const
{
    return name;
}

void Raycaster::rename(std::string name)
{
    this->name = name;
}

GLboolean Raycaster::getUseERT() const
{
    return useERT;
}

void Raycaster::setUseERT(GLboolean useERT)
{
    GLboolean previous = this->useERT;
    this->useERT = useERT;

    // Only reload shader if necessary
    if(this->useERT != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseLocalIllumination() const
{
    return useLocalIllumination;
}

void Raycaster::setUseLocalIllumination(GLboolean useLocalIllumination)
{
    GLboolean previous = this->useLocalIllumination;
    this->useLocalIllumination = useLocalIllumination;

    // Only reload shader if necessary
    if(this->useLocalIllumination != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseShadows() const
{
    return useShadows;
}

void Raycaster::setUseShadows(GLboolean useShadows)
{
    GLboolean previous = this->useShadows;
    this->useShadows = useShadows;

    // Only reload shader if necessary
    if(this->useShadows != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseJittering() const
{
    return useJittering;
}

void Raycaster::setUseJittering(GLboolean useJittering)
{
    GLboolean previous = this->useJittering;
    this->useJittering = useJittering;

    // Only reload shader if necessary
    if(this->useJittering != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseExtentPreservingJittering() const
{
    return useExtentPreservingJittering;
}

void Raycaster::setUseExtentPreservingJittering(GLboolean useExtentPreservingJittering)
{
    GLboolean previous = this->useExtentPreservingJittering;
    this->useExtentPreservingJittering = useExtentPreservingJittering;

    // Only reload shader if necessary
    if(this->useExtentPreservingJittering != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}


GLboolean Raycaster::getUseGradientAlphaMultiplier() const
{
    return useGradientAlphaMultiplier;
}

void Raycaster::setUseGradientAlphaMultiplier(GLboolean useGradientAlphaMultiplier)
{
    GLboolean previous = this->useGradientAlphaMultiplier;
    this->useGradientAlphaMultiplier = useGradientAlphaMultiplier;

    // Only reload shader if necessary
    if(this->useGradientAlphaMultiplier != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseFresnelAlphaMultiplier() const
{
    return useFresnelAlphaMultiplier;
}

void Raycaster::setUseFresnelAlphaMultiplier(GLboolean useFresnelAlphaMultiplier)
{
    GLboolean previous = this->useFresnelAlphaMultiplier;
    this->useFresnelAlphaMultiplier = useFresnelAlphaMultiplier;

    // Only reload shader if necessary
    if(this->useFresnelAlphaMultiplier != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseReflectionColorMultiplier() const
{
    return useReflectionColorMultiplier;
}

void Raycaster::setUseReflectionColorMultiplier(GLboolean useReflectionColorMultiplier)
{
    GLboolean previous = this->useReflectionColorMultiplier;
    this->useReflectionColorMultiplier = useReflectionColorMultiplier;

    // Only reload shader if necessary
    if(this->useReflectionColorMultiplier != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseEmissionColorMultiplier() const
{
    return useEmissionColorMultiplier;
}

void Raycaster::setUseEmissionColorMultiplier(GLboolean useEmissionColorMultiplier)
{
    GLboolean previous = this->useEmissionColorMultiplier;
    this->useEmissionColorMultiplier = useEmissionColorMultiplier;

    // Only reload shader if necessary
    if(this->useEmissionColorMultiplier != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseSimpleESS() const
{
    return useSimpleESS;
}

void Raycaster::setUseSimpleESS(GLboolean useSimpleESS)
{
    GLboolean previous = this->useSimpleESS;
    this->useSimpleESS = useSimpleESS;

    // Only reload shader if necessary
    if(this->useSimpleESS != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseNormalsOfClassifiedData() const
{
    return useNormalsOfClassifiedData;
}

void Raycaster::setUseNormalsOfClassifiedData(GLboolean useNormalsOfClassifiedData)
{
    GLboolean previous = this->useNormalsOfClassifiedData;
    this->useNormalsOfClassifiedData = useNormalsOfClassifiedData;

    // Only reload shader if necessary
    if(this->useNormalsOfClassifiedData != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseExtentAwareNormals() const
{
    return useExtentAwareNormals;
}

void Raycaster::setUseExtentAwareNormals(GLboolean useExtentAwareNormals)
{
    GLboolean previous = this->useExtentAwareNormals;
    this->useExtentAwareNormals = useExtentAwareNormals;

    // Only reload shader if necessary
    if(this->useExtentAwareNormals != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUsePreintegration() const
{
    return usePreintegration;
}

void Raycaster::setUsePreintegration(GLboolean useColorAlphaPreintegration)
{
    GLboolean previous = this->usePreintegration;
    this->usePreintegration = useColorAlphaPreintegration;

    // Only reload shader if necessary
    if(this->usePreintegration != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseAdaptiveSampling() const
{
    return useAdaptiveSampling;
}

void Raycaster::setUseAdaptiveSampling(GLboolean useAdaptiveSampling)
{
    GLboolean previous = this->useAdaptiveSampling;
    this->useAdaptiveSampling = useAdaptiveSampling;

    // Only reload shader if necessary
    if(this->useAdaptiveSampling != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

GLboolean Raycaster::getUseVoxelSpacedSampling() const
{
    return useVoxelSpacedSampling;
}

void Raycaster::setUseVoxelSpacedSampling(GLboolean useVoxelSpacedSampling)
{
    GLboolean previous = this->useVoxelSpacedSampling;
    this->useVoxelSpacedSampling = useVoxelSpacedSampling;

    // Only reload shader if necessary
    if(this->useVoxelSpacedSampling != previous)
    {
        shaderShouldBeReloaded = GL_TRUE;
    }
}

void Raycaster::reloadShader()
{
    // Define vectors
    std::vector<std::string> vertexDefines;
    std::vector<std::string> fragmentDefines;

    if(useERT)
    {
        fragmentDefines.push_back("USE_ERT");
    }

    if(useLocalIllumination)
    {
        fragmentDefines.push_back("USE_LOCAL_ILLUMINATION");

        if(useShadows)
        {
            fragmentDefines.push_back("USE_SHADOWS");
        }
    }

    if(useJittering)
    {
        fragmentDefines.push_back("USE_JITTERING");
        fragmentDefines.push_back("NOISE_RES " + UT::to_string(RAYCASTER_NOISE_RES));
        if(useExtentPreservingJittering)
        {
            fragmentDefines.push_back("USE_EXTENT_PRESERVING_JITTERING");
        }
    }

    if(useSimpleESS)
    {
        fragmentDefines.push_back("USE_SIMPLE_ESS");
    }

    if(useGradientAlphaMultiplier)
    {
        fragmentDefines.push_back("USE_GRADIENT_ALPHA_MULTIPLIER");
    }

    if(useFresnelAlphaMultiplier)
    {
        fragmentDefines.push_back("USE_FRESNEL_ALPHA_MULTIPLIER");
    }

    if(useReflectionColorMultiplier)
    {
        fragmentDefines.push_back("USE_REFLECTION_COLOR_MULTIPLIER");
    }

    if(useEmissionColorMultiplier)
    {
        fragmentDefines.push_back("USE_EMISSION_COLOR_MULTIPLIER");
    }

    if(useNormalsOfClassifiedData)
    {
        fragmentDefines.push_back("USE_NORMALS_OF_CLASSIFIED_DATA");
    }

    if(useExtentAwareNormals)
    {
        fragmentDefines.push_back("USE_EXTENT_AWARE_NORMALS");
    }

    if(usePreintegration)
    {
        fragmentDefines.push_back("USE_PREINTEGRATION");
    }

    if(useAdaptiveSampling)
    {
        fragmentDefines.push_back("USE_ADAPTIVE_SAMPLING");
    }

    if(useVoxelSpacedSampling)
    {
        fragmentDefines.push_back("USE_VOXEL_SPACED_SAMPLING");
    }

    // Load shaders
    shader.loadShaders("Raycaster.vert", "Raycaster.frag", vertexDefines, fragmentDefines);
    shader.setVertexBuffer(primitives::cube, sizeof(primitives::cube), "positionAttribute");

    // Get uniform handles
    uniformModelScaleHandle = shader.getUniformHandle("uniformModelScale");
    uniformModelRotationHandle = shader.getUniformHandle("uniformModelRotation");
    uniformViewHandle = shader.getUniformHandle("uniformView");
    uniformProjectionHandle = shader.getUniformHandle("uniformProjection");
    uniformCameraPosHandle = shader.getUniformHandle("uniformCameraPos");
    uniformVolumeHandle = shader.getUniformHandle("uniformVolume");
    uniformBasicInputHandle = shader.getUniformHandle("uniformBasicInput");
    uniformAdvancedInputHandle = shader.getUniformHandle("uniformAdvancedInput");
    uniformVolumeValueInformationHandle = shader.getUniformHandle("uniformVolumeValueInformation");
    uniformMirrorUVWHandle = shader.getUniformHandle("uniformMirrorUVW");
    uniformVolumeExtentHandle = shader.getUniformHandle("uniformVolumeExtent");
    uniformVolumeExtentOffsetHandle = shader.getUniformHandle("uniformVolumeExtentOffset");

    if(usePreintegration)
    {
        uniformColorAlphaPreintegrationHandle = shader.getUniformHandle("uniformColorAlphaPreintegration");
    }
    else
    {
        uniformColorAlphaHandle = shader.getUniformHandle("uniformColorAlpha");
    }

    if(useGradientAlphaMultiplier || useFresnelAlphaMultiplier || useReflectionColorMultiplier || useEmissionColorMultiplier)
    {
        if(usePreintegration)
        {
            uniformAdvancedPreintegrationHandle = shader.getUniformHandle("uniformAdvancedPreintegration");
        }
        else
        {
            uniformAdvancedHandle = shader.getUniformHandle("uniformAdvanced");
        }
    }

    if(useReflectionColorMultiplier)
    {
        uniformReflectionHandle = shader.getUniformHandle("uniformReflection");
    }

    if(useLocalIllumination)
    {
        if(usePreintegration)
        {
            uniformAmbientSpecularPreintegrationHandle = shader.getUniformHandle("uniformAmbientSpecularPreintegration");
        }
        else
        {
            uniformAmbientSpecularHandle = shader.getUniformHandle("uniformAmbientSpecular");
        }
        uniformSunDirectionHandle = shader.getUniformHandle("uniformSunDirection");
        uniformSunColorHandle = shader.getUniformHandle("uniformSunColor");
        uniformAmbientColorHandle = shader.getUniformHandle("uniformAmbientColor");
    }

    if(useJittering)
    {
        uniformNoiseHandle = shader.getUniformHandle("uniformNoise");
    }

    if(useAdaptiveSampling)
    {
        uniformImportanceVolumeHandle = shader.getUniformHandle("uniformImportanceVolume");
    }

    if(useVoxelSpacedSampling)
    {
        uniformVolumeResolutionHandle = shader.getUniformHandle("uniformVolumeResolution");
    }
}

GLfloat Raycaster::calcNormalRand(GLfloat uA, GLfloat uB)
{
    return glm::cos(2 * glm::pi<GLfloat>() * uA) * glm::sqrt(-2 * (glm::log(uB)/glm::log(glm::e<GLfloat>())));
}
