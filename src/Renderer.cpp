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

#include "Renderer.h"

Renderer::Renderer() : Viewport()
{
    type = VIEWPORT_RENDERER;
    bar_showAxesGizmo = RENDERER_SHOW_AXES_GIZMO;
    bar_showVolumeExtentGizmo = RENDERER_SHOW_VOLUME_EXTENT_GIZMO;
    bar_showSunGizmo = RENDERER_SHOW_SUN_GIZMO;
    bar_showImportanceVolume = RENDERER_SHOW_IMPORTANCE_VOLUME;
    bar_overwriteExisting = GL_FALSE;
    tfHandle = -1;
    rcHandle = -1;

    // Initialize field of view
    bar_fieldOfView = RENDERER_FIELD_OF_VIEW;

    // Initialize lighting
    bar_sunDirection = RENDERER_SUN_DIRECTION;
    bar_sunColor = RENDERER_SUN_COLOR;
    bar_sunBrightness = RENDERER_SUN_BRIGHTNESS;
    bar_ambientColor = RENDERER_AMBIENT_COLOR;

    // Initialize volume clipping stuff
    bar_volumeExtent = RENDERER_VOLUME_EXTENT;
    bar_volumeExtentOffset = RENDERER_VOLUME_EXTENT_OFFSET;
}

Renderer::~Renderer()
{
    glDeleteTextures(1, &reflectionHandle);
}

void Renderer::init(TfManager* pTfManager, RcManager* pRcManager, VolumeManager* pVolumeManager, GLint handle, glm::vec3 color, std::string title, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive)
{
    // Call super method
    Viewport::init(handle, title, color, posX, posY, width, height, windowWidth, windowHeight, barActive);

    // Save pointer to tfManager
    this->pTfManager = pTfManager;

    // Sava pointer to rcManager
    this->pRcManager = pRcManager;

    // Save pointer to volumeManager
    this->pVolumeManager = pVolumeManager;

    // Get tf handle
    setTfHandle(pTfManager->getLatestTfHandle());

    // Get rc handle
    setRcHandle(pRcManager->getLatestRcHandle());

    // Get volume handle
    setVolumeHandle(pVolumeManager->getLatestVolumeHandle());

    // Initialize camera
    camera.init(RENDERER_CAMERA_CENTER, glm::radians(RENDERER_CAMERA_ALPHA), glm::radians(RENDERER_CAMERA_BETA), RENDERER_CAMERA_RADIUS, RENDERER_CAMERA_RADIUS_MIN, RENDERER_CAMERA_RADIUS_MAX);

    // Add variables to bar
    TwAddVarRW(pBar, "Active Tf", TW_TYPE_INT32, &(bar_activeTf.value), " min=0 ");
    TwAddVarRW(pBar, "Tf Name", TW_TYPE_STDSTRING, &(bar_tfName.value), "");

    TwAddSeparator(pBar, NULL, "");

    TwAddVarRW(pBar, "Active Rc", TW_TYPE_INT32, &(bar_activeRc.value), " min=0 ");
    TwAddVarRW(pBar, "Rc Name", TW_TYPE_STDSTRING, &(bar_rcName.value), "");

    TwAddSeparator(pBar, NULL, "");

    TwAddVarRW(pBar, "Active Volume", TW_TYPE_INT32, &(bar_activeVolume.value), " min=0 ");
    TwAddVarRW(pBar, "Volume Name", TW_TYPE_STDSTRING, &(bar_volumeName.value), "");

    TwAddSeparator(pBar, NULL, "");

    TwAddVarRW(pBar, "Use Simple ESS", TW_TYPE_BOOLCPP, &(bar_rcUseSimpleESS.value), "");
    TwAddVarRW(pBar, "Use ERT", TW_TYPE_BOOLCPP, &(bar_rcUseERT.value), "");
    TwAddVarRW(pBar, "Use Jittering", TW_TYPE_BOOLCPP, &(bar_rcUseJittering.value), "");
    TwAddVarRW(pBar, "Use Extent Preserving Jittering", TW_TYPE_BOOLCPP, &(bar_rcUseExtentPreservingJittering.value), "");
    TwAddVarRW(pBar, "Use Preintegration", TW_TYPE_BOOLCPP, &(bar_rcUsePreintegration.value), "");
    TwAddVarRW(pBar, "Use Voxel Spaced Sampling", TW_TYPE_BOOLCPP, &(bar_rcUseVoxelSpacedSampling.value), "");
    TwAddVarRW(pBar, "Use Adaptive Sampling", TW_TYPE_BOOLCPP, &(bar_rcUseAdaptiveSampling.value), "");
    TwAddVarRW(pBar, "Use Local Illumination", TW_TYPE_BOOLCPP, &(bar_rcUseLocalIllumination.value), "");
    TwAddVarRW(pBar, "Use Shadows", TW_TYPE_BOOLCPP, &(bar_rcUseShadows.value), "");
    TwAddVarRW(pBar, "Use Normals Of Classified Data", TW_TYPE_BOOLCPP, &(bar_rcUseNormalsOfClassifiedData.value), "");
    TwAddVarRW(pBar, "Use Extent Aware Normals", TW_TYPE_BOOLCPP, &(bar_rcUseExtentAwareNormals.value), "");

    TwAddVarRW(pBar, "Use Gradient Alpha Multiplier", TW_TYPE_BOOLCPP, &(bar_rcUseGradientAlphaMultiplier.value), " group='Use Of TfValues' ");
    TwAddVarRW(pBar, "Use Fresnel Alpha Multiplier", TW_TYPE_BOOLCPP, &(bar_rcUseFresnelAlphaMultiplier.value), " group='Use Of TfValues' ");
    TwAddVarRW(pBar, "Use Reflection Color Multiplier", TW_TYPE_BOOLCPP, &(bar_rcUseReflectionColorMultiplier.value), " group='Use Of TfValues' ");
    TwAddVarRW(pBar, "Use Emission Color Multiplier", TW_TYPE_BOOLCPP, &(bar_rcUseEmissionColorMultiplier.value), " group='Use Of TfValues' ");

    TwAddVarRW(pBar, "Step Size", TW_TYPE_FLOAT, &(bar_rcStepSize.value), " group='Raycaster Properties' precision=4 ");
    TwAddVarRW(pBar, "Step Size Multiplier", TW_TYPE_FLOAT, &(bar_rcStepSizeMultiplier.value), " group='Raycaster Properties' precision=1 ");
    TwAddVarRW(pBar, "Min Adaptive Step Size", TW_TYPE_FLOAT, &(bar_rcMinAdaptiveStepSize.value), " group='Raycaster Properties' precision=4 ");
    TwAddVarRW(pBar, "Max Adaptive Step Size", TW_TYPE_FLOAT, &(bar_rcMaxAdaptiveStepSize.value), " group='Raycaster Properties' precision=4 ");
    TwAddVarRW(pBar, "Min Adaptive Step Size Multiplier", TW_TYPE_FLOAT, &(bar_rcMinAdaptiveStepSizeMultiplier.value), " group='Raycaster Properties' precision=1 ");
    TwAddVarRW(pBar, "Max Adaptive Step Size Multiplier", TW_TYPE_FLOAT, &(bar_rcMaxAdaptiveStepSizeMultiplier.value), " group='Raycaster Properties' precision=1 ");
    TwAddVarRW(pBar, "Outer Iteration", TW_TYPE_INT32, &(bar_rcOuterIteration.value), " group='Raycaster Properties' precision=0 ");
    TwAddVarRW(pBar, "Inner Iteration", TW_TYPE_INT32, &(bar_rcInnerIteration.value), " group='Raycaster Properties' precision=0 ");
    TwAddVarRW(pBar, "Alpha Threshold", TW_TYPE_FLOAT, &(bar_rcAlphaThreshold.value), " group='Raycaster Properties' precision=2 ");
    TwAddVarRW(pBar, "Jittering Range Multiplier", TW_TYPE_FLOAT, &(bar_rcJitteringRangeMultiplier.value), " group='Raycaster Properties' precision=1 ");
    TwAddVarRW(pBar, "Normal Range Multiplier", TW_TYPE_FLOAT, &(bar_rcNormalRangeMultiplier.value), " group='Raycaster Properties' precision=1 ");
    TwAddVarRW(pBar, "Fresnel Power", TW_TYPE_FLOAT, &(bar_rcFresnelPower.value), " group='Raycaster Properties' precision=1 ");

    TwAddButton(pBar, "Reset Properties", resetRaycasterPropertiesButtonCallback, this, " group='Raycaster Properties' ");

    TwAddSeparator(pBar, NULL, "");

    TwAddButton(pBar, "New", newRcButtonCallback, this, " group='Rc Management' ");
    TwAddButton(pBar, "Reload", reloadRcButtonCallback, this, " group='Rc Management' ");
    TwAddButton(pBar, "Save", saveRcButtonCallback, this, " group='Rc Management' ");
    TwAddVarRW(pBar, "Rc To Load", TW_TYPE_STDSTRING, &bar_pathToExternRc, " group='Rc Management' ");
    TwAddButton(pBar, "Load", loadRcButtonCallback, this, " group='Rc Management' ");
    TwAddVarRW(pBar, "Overwrite Existing", TW_TYPE_BOOLCPP, &bar_overwriteExisting, " group='Rc Management' ");

    TwAddSeparator(pBar, NULL, "");

    TwAddVarRW(pBar, "Sun Direction", TW_TYPE_DIR3F, &bar_sunDirection, " group='Lighting' ");
    TwAddVarRW(pBar, "Sun Color", TW_TYPE_COLOR3F, &bar_sunColor, " colormode=rgb group='Lighting' ");
    TwAddVarRW(pBar, "Sun Brightness", TW_TYPE_FLOAT, &bar_sunBrightness, " group='Lighting' ");
    TwAddVarRW(pBar, "Ambient Color", TW_TYPE_COLOR3F, &bar_ambientColor, " colormode=rgb group='Lighting' ");

    TwAddVarRW(pBar, "Volume Extent X", TW_TYPE_FLOAT, &(bar_volumeExtent.x), " group='Volume Clipping' precision=2 ");
    TwAddVarRW(pBar, "Volume Extent Y", TW_TYPE_FLOAT, &(bar_volumeExtent.y), " group='Volume Clipping' precision=2 ");
    TwAddVarRW(pBar, "Volume Extent Z", TW_TYPE_FLOAT, &(bar_volumeExtent.z), " group='Volume Clipping' precision=2 ");
    TwAddVarRW(pBar, "Volume Extent Offset X", TW_TYPE_FLOAT, &(bar_volumeExtentOffset.x), " group='Volume Clipping' precision=2 ");
    TwAddVarRW(pBar, "Volume Extent Offset Y", TW_TYPE_FLOAT, &(bar_volumeExtentOffset.y), " group='Volume Clipping' precision=2 ");
    TwAddVarRW(pBar, "Volume Extent Offset Z", TW_TYPE_FLOAT, &(bar_volumeExtentOffset.z), " group='Volume Clipping' precision=2 ");

    TwAddSeparator(pBar, NULL, "");

    TwAddVarRW(pBar, "FOV", TW_TYPE_FLOAT, &bar_fieldOfView, " precision=0 ");
    TwAddVarRW(pBar, "Alpha", TW_TYPE_FLOAT, &(bar_cameraAlpha.value), " group='Camera' precision=3 ");
    TwAddVarRW(pBar, "Beta", TW_TYPE_FLOAT, &(bar_cameraBeta.value), " group='Camera' precision=3  ");
    TwAddVarRW(pBar, "Radius", TW_TYPE_FLOAT, &(bar_cameraRadius.value), " group='Camera' precision=1 step=0.1 ");
    TwAddButton(pBar, "Reset Camera", resetCameraButtonCallback, this, " group='Camera' ");
    TwAddVarRW(pBar, "Show Axes", TW_TYPE_BOOLCPP, &bar_showAxesGizmo, "");
    TwAddVarRW(pBar, "Show Volume Clipping", TW_TYPE_BOOLCPP, &bar_showVolumeExtentGizmo, "");
    TwAddVarRW(pBar, "Show Sun", TW_TYPE_BOOLCPP, &bar_showSunGizmo, "");
    TwAddVarRW(pBar, "Show Importance Volume", TW_TYPE_BOOLCPP, &bar_showImportanceVolume, "");

    // Set some variable parameters
    TwSetParam(pBar, "FOV", "min", TW_PARAM_FLOAT, 1, &RENDERER_FIELD_OF_VIEW_MIN);
    TwSetParam(pBar, "FOV", "max", TW_PARAM_FLOAT, 1, &RENDERER_FIELD_OF_VIEW_MAX);
    TwSetParam(pBar, "Step Size", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_STEP_SIZE_STEP);
    TwSetParam(pBar, "Step Size", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MIN);
    TwSetParam(pBar, "Step Size", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MAX);
    TwSetParam(pBar, "Step Size Multiplier", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_STEP_SIZE_STEP_MULTIPLIER);
    TwSetParam(pBar, "Step Size Multiplier", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MIN);
    TwSetParam(pBar, "Step Size Multiplier", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MAX);
    TwSetParam(pBar, "Min Adaptive Step Size", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_STEP_SIZE_STEP);
    TwSetParam(pBar, "Min Adaptive Step Size", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MIN);
    TwSetParam(pBar, "Min Adaptive Step Size", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MAX);
    TwSetParam(pBar, "Max Adaptive Step Size", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_STEP_SIZE_STEP);
    TwSetParam(pBar, "Max Adaptive Step Size", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MIN);
    TwSetParam(pBar, "Max Adaptive Step Size", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MAX);
    TwSetParam(pBar, "Min Adaptive Step Size Multiplier", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_STEP_SIZE_STEP_MULTIPLIER);
    TwSetParam(pBar, "Min Adaptive Step Size Multiplier", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MIN);
    TwSetParam(pBar, "Min Adaptive Step Size Multiplier", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MAX);
    TwSetParam(pBar, "Max Adaptive Step Size Multiplier", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_STEP_SIZE_STEP_MULTIPLIER);
    TwSetParam(pBar, "Max Adaptive Step Size Multiplier", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MIN);
    TwSetParam(pBar, "Max Adaptive Step Size Multiplier", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MAX);
    TwSetParam(pBar, "Outer Iteration", "min", TW_PARAM_INT32, 1, &RAYCASTERPROPERTIES_OUTER_ITERATIONS_MIN);
    TwSetParam(pBar, "Outer Iteration", "max", TW_PARAM_INT32, 1, &RAYCASTERPROPERTIES_OUTER_ITERATIONS_MAX);
    TwSetParam(pBar, "Inner Iteration", "min", TW_PARAM_INT32, 1, &RAYCASTERPROPERTIES_INNER_ITERATIONS_MIN);
    TwSetParam(pBar, "Inner Iteration", "max", TW_PARAM_INT32, 1, &RAYCASTERPROPERTIES_INNER_ITERATIONS_MAX);
    TwSetParam(pBar, "Alpha Threshold", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_ALPHA_THRESHOLD_STEP);
    TwSetParam(pBar, "Alpha Threshold", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_ALPHA_THRESHOLD_MIN);
    TwSetParam(pBar, "Alpha Threshold", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_ALPHA_THRESHOLD_MAX);
    TwSetParam(pBar, "Jittering Range Multiplier", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_JITTERING_RANGE_MULTIPLIER_MIN);
    TwSetParam(pBar, "Jittering Range Multiplier", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_JITTERING_RANGE_MULTIPLIER_MAX);
    TwSetParam(pBar, "Jittering Range Multiplier", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_JITTERING_RANGE_MULTIPLIER_STEP);
    TwSetParam(pBar, "Normal Range Multiplier", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_NORMAL_RANGE_MULTIPLIER_STEP);
    TwSetParam(pBar, "Normal Range Multiplier", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_NORMAL_RANGE_MULTIPLIER_MIN);
    TwSetParam(pBar, "Normal Range Multiplier", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_NORMAL_RANGE_MULTIPLIER_MAX);
    TwSetParam(pBar, "Fresnel Power", "min", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_FRESNEL_POWER_MIN);
    TwSetParam(pBar, "Fresnel Power", "max", TW_PARAM_FLOAT, 1, &RAYCASTERPROPERTIES_FRESNEL_POWER_MAX);
    TwSetParam(pBar, "Fresnel Power", "step", TW_PARAM_FLOAT, 1, &RENDERER_RC_FRESNEL_ALPHA_MULTIPLIER_STEP);
    TwSetParam(pBar, "Volume Extent X", "step", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_CLIPPING_STEPS);
    TwSetParam(pBar, "Volume Extent X", "min", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_EXTENT_MIN);
    TwSetParam(pBar, "Volume Extent X", "max", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_EXTENT_MAX);
    TwSetParam(pBar, "Volume Extent Y", "step", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_CLIPPING_STEPS);
    TwSetParam(pBar, "Volume Extent Y", "min", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_EXTENT_MIN);
    TwSetParam(pBar, "Volume Extent Y", "max", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_EXTENT_MAX);
    TwSetParam(pBar, "Volume Extent Z", "step", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_CLIPPING_STEPS);
    TwSetParam(pBar, "Volume Extent Z", "min", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_EXTENT_MIN);
    TwSetParam(pBar, "Volume Extent Z", "max", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_EXTENT_MAX);
    TwSetParam(pBar, "Volume Extent Offset X", "step", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_CLIPPING_STEPS);
    TwSetParam(pBar, "Volume Extent Offset Y", "step", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_CLIPPING_STEPS);
    TwSetParam(pBar, "Volume Extent Offset Z", "step", TW_PARAM_FLOAT, 1, &RENDERER_VOLUME_CLIPPING_STEPS);
    TwSetParam(pBar, "Sun Brightness", "step", TW_PARAM_FLOAT, 1, &RENDERER_SUN_BRIGHTNESS_STEP);
    TwSetParam(pBar, "Sun Brightness", "min", TW_PARAM_FLOAT, 1, &RENDERER_SUN_BRIGHTNESS_MIN);
    TwSetParam(pBar, "Sun Brightness", "max", TW_PARAM_FLOAT, 1, &RENDERER_SUN_BRIGHTNESS_MAX);

    // Fold groups
    GLint opened = 0;
    TwSetParam(pBar, "Use Of TfValues", "opened", TW_PARAM_INT32, 1, &opened);
    TwSetParam(pBar, "Rc Management", "opened", TW_PARAM_INT32, 1, &opened);
    TwSetParam(pBar, "Volume Clipping", "opened", TW_PARAM_INT32, 1, &opened);
    TwSetParam(pBar, "Camera", "opened", TW_PARAM_INT32, 1, &opened);

    // Fill bar variables
    fillBarVariables();

    // *** REFLECTION MAP ***

    // Load reflection map
    std::vector<unsigned char> reflection;
    unsigned long reflectionX;
    unsigned long reflectionY;

    // Read png file
    std::ifstream in(std::string(std::string(DATA_PATH) + "/_Resources/Spheremap.png").c_str(), std::ios::in|std::ios::binary);
    in.seekg(0, std::ios::end);
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<GLchar> buffer(static_cast<GLuint>(size));
    in.read(&(buffer[0]), static_cast<size_t>(size));
    in.close();
    decodePNG(reflection, reflectionX, reflectionY, reinterpret_cast<unsigned char*>(&(buffer[0])),  static_cast<int>(size), GL_FALSE);

    glGenTextures(1, &reflectionHandle);
    glBindTexture(GL_TEXTURE_2D, reflectionHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, reflectionX, reflectionY, 0, GL_RGB, GL_UNSIGNED_BYTE, &reflection[0]);
    glBindTexture(GL_TEXTURE_2D, 0);

    // *** GIZMOS ***

    // Initialize line shader
    lineShader.loadShaders("Line.vert", "Line.frag");

    // Get uniform handles of the line shader
    lineShaderModelHandle = lineShader.getUniformHandle("uniformModel");
    lineShaderViewHandle = lineShader.getUniformHandle("uniformView");
    lineShaderProjectionHandle = lineShader.getUniformHandle("uniformProjection");
    lineShaderColorHandle = lineShader.getUniformHandle("uniformColor");

    // Calculate model matrices of axes gizmo
    redAxisGizmoModel = glm::mat4(1.0f);
    redAxisGizmoModel = glm::scale(redAxisGizmoModel, glm::vec3(0.2f));

    greenAxisGizmoModel = glm::mat4(1.0f);
    greenAxisGizmoModel = glm::scale(greenAxisGizmoModel, glm::vec3(0.2f));
    greenAxisGizmoModel = glm::rotate(greenAxisGizmoModel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    blueAxisGizmoModel = glm::mat4(1.0f);
    blueAxisGizmoModel = glm::scale(blueAxisGizmoModel, glm::vec3(0.2f));
    blueAxisGizmoModel = glm::rotate(blueAxisGizmoModel, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // *** IMPORTANCE VOLUME VISUALIZATION ***

    // Initialize shader
    importanceVolumeShader.loadShaders("ImportanceVolume.vert", "ImportanceVolume.frag");
    importanceVolumeShader.setVertexBuffer(primitives::cube, sizeof(primitives::cube), "positionAttribute");

    // Get uniforms
    importanceVolumeShaderModelScaleHandle = importanceVolumeShader.getUniformHandle("uniformModelScale");
    importanceVolumeModelRotationHandle = importanceVolumeShader.getUniformHandle("uniformModelRotation");
    importanceVolumeViewHandle = importanceVolumeShader.getUniformHandle("uniformView");
    importanceVolumeProjectionHandle = importanceVolumeShader.getUniformHandle("uniformProjection");
    importanceVolumeCameraPosHandle = importanceVolumeShader.getUniformHandle("uniformCameraPos");
    importanceVolumeVolumeTextureHandle = importanceVolumeShader.getUniformHandle("uniformImportanceVolume");
    importanceVolumeShaderMirrorUVWHandle = importanceVolumeShader.getUniformHandle("uniformMirrorUVW");
    importanceVolumeExtentHandle = importanceVolumeShader.getUniformHandle("uniformVolumeExtent");
    importanceVolumeExtentOffsetHandle = importanceVolumeShader.getUniformHandle("uniformVolumeExtentOffset");

}

ViewportType Renderer::update(GLfloat tpf, InputData inputData, GLboolean selected)
{
    // Call super method
    Viewport::update(tpf, inputData, selected);

    // *** MATRICES ***
    projectionMatrix = glm::perspective(glm::radians(bar_fieldOfView), this->getAspectRatio(), 0.1f, 100.f);

    // Importance volume
    if(bar_showImportanceVolume)
    {
        if(volumeHandle >= 0)
        {
            Volume* pVolume = pVolumeManager->getVolume(volumeHandle);
            VolumeProperties volumeProperties = pVolume->getProperties();

            // Mirror
            if(volumeProperties.mirrorX)
            {
                importanceVolumeMirrorUVW.x = 1;
            }
            if(volumeProperties.mirrorY)
            {
                importanceVolumeMirrorUVW.y = 1;
            }
            if(volumeProperties.mirrorZ)
            {
                importanceVolumeMirrorUVW.z = 1;
            }

            // Volume clipping
            importanceVolumeExtentMatrix = glm::mat4(1.0f);
            importanceVolumeExtentMatrix = glm::scale(importanceVolumeExtentMatrix, bar_volumeExtent);

            // Compute model matrix
            importanceVolumeModelScaleMatrix = glm::mat4(1.0f);
            importanceVolumeModelRotationMatrix = glm::mat4(1.0f);
            importanceVolumeModelScaleMatrix = glm::scale(importanceVolumeModelScaleMatrix, volumeProperties.voxelScaleMultiplier * pVolume->getRenderingScale());
            importanceVolumeModelRotationMatrix = glm::rotate(importanceVolumeModelRotationMatrix, glm::radians(volumeProperties.eulerZXZRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            importanceVolumeModelRotationMatrix = glm::rotate(importanceVolumeModelRotationMatrix, glm::radians(volumeProperties.eulerZXZRotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
            importanceVolumeModelRotationMatrix = glm::rotate(importanceVolumeModelRotationMatrix, glm::radians(volumeProperties.eulerZXZRotation.x), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    }

    // Volume extent gizmo
    if(bar_showVolumeExtentGizmo)
    {
        if(volumeHandle >= 0)
        {
            Volume* pVolume = pVolumeManager->getVolume(volumeHandle);
            VolumeProperties volumeProperties = pVolume->getProperties();

            // Reconstruct volume's matrices
            volumeExtentGizmoModelScale = glm::mat4(1.0f);
            volumeExtentGizmoModelRotation = glm::mat4(1.0f);
            volumeExtentGizmoModelScale = glm::scale(volumeExtentGizmoModelScale, volumeProperties.voxelScaleMultiplier * pVolume->getRenderingScale());
            volumeExtentGizmoModelRotation = glm::rotate(volumeExtentGizmoModelRotation, glm::radians(volumeProperties.eulerZXZRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            volumeExtentGizmoModelRotation = glm::rotate(volumeExtentGizmoModelRotation, glm::radians(volumeProperties.eulerZXZRotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
            volumeExtentGizmoModelRotation = glm::rotate(volumeExtentGizmoModelRotation, glm::radians(volumeProperties.eulerZXZRotation.x), glm::vec3(0.0f, 0.0f, 1.0f));

            // Original size
            volumeExtentGizmoOriginalModel = volumeExtentGizmoModelRotation * volumeExtentGizmoModelScale * glm::mat4(1.0f);

            // Clipped size
            volumeExtentGizmoClippedModel = glm::translate(glm::mat4(1.0f), bar_volumeExtentOffset);
            volumeExtentGizmoClippedModel = glm::scale(volumeExtentGizmoClippedModel, bar_volumeExtent);
            volumeExtentGizmoClippedModel = volumeExtentGizmoModelRotation * volumeExtentGizmoModelScale * volumeExtentGizmoClippedModel;
        }
    }

    return bar_type;
}

void Renderer::draw()
{
    // Call super method to set viewport
    Viewport::draw();

    // Get view matrix from camera
    glm::mat4 viewMatrix = camera.getViewMatrix();

    // *** DRAW WITH RAYCASTER ***
    if(volumeHandle >= 0)
    {
        glDisable(GL_DEPTH_TEST);

        Volume* pVolume = pVolumeManager->getVolume(volumeHandle);
        if(!bar_showImportanceVolume)
        {
            pRcManager->getRc(rcHandle)->draw(
                                            pVolume->getTextureHandle(),
                                            pVolume->getImportanceVolumeTextureHandle(),
                                            pVolume->getRenderingScale(),
                                            pVolume->getVolumeResolution(),
                                            pVolume->getProperties(),
                                            this->getAspectRatio(),
                                            bar_fieldOfView,
                                            camera.getPosition(),
                                            viewMatrix,
                                            bar_sunDirection,
                                            bar_sunColor,
                                            bar_sunBrightness,
                                            bar_ambientColor,
                                            pTfManager->getTf(tfHandle)->getColorAlphaFunctionHandle(),
                                            pTfManager->getTf(tfHandle)->getAmbientSpecularFunctionHandle(),
                                            pTfManager->getTf(tfHandle)->getAdvancedFunctionHandle(),
                                            pTfManager->getTf(tfHandle)->getColorAlphaPreintegrationHandle(),
                                            pTfManager->getTf(tfHandle)->getAmbientSpecularPreintegrationHandle(),
                                            pTfManager->getTf(tfHandle)->getAdvancedPreintegrationHandle(),
                                            pTfManager->getTf(tfHandle)->getTextureResolution(),
                                            reflectionHandle,
                                            bar_volumeExtent,
                                            bar_volumeExtentOffset);
        }
        else
        {
            // Use program because setUniform needs ist
            importanceVolumeShader.use();

            // Set uniforms
            importanceVolumeShader.setUniformValue(importanceVolumeShaderModelScaleHandle, importanceVolumeModelScaleMatrix);
            importanceVolumeShader.setUniformValue(importanceVolumeModelRotationHandle, importanceVolumeModelRotationMatrix);
            importanceVolumeShader.setUniformValue(importanceVolumeViewHandle, camera.getViewMatrix());
            importanceVolumeShader.setUniformValue(importanceVolumeProjectionHandle, projectionMatrix);
            importanceVolumeShader.setUniformValue(importanceVolumeCameraPosHandle, camera.getPosition());
            importanceVolumeShader.setUniformTexture(importanceVolumeVolumeTextureHandle, pVolume->getImportanceVolumeTextureHandle(), GL_TEXTURE_3D);
            importanceVolumeShader.setUniformValue(importanceVolumeShaderMirrorUVWHandle, importanceVolumeMirrorUVW);
            importanceVolumeShader.setUniformValue(importanceVolumeExtentHandle, importanceVolumeExtentMatrix);
            importanceVolumeShader.setUniformValue(importanceVolumeExtentOffsetHandle, bar_volumeExtentOffset);

            // Draw it
            importanceVolumeShader.draw(GL_TRIANGLES);
        }

        glEnable(GL_DEPTH_TEST);
    }

    // *** DRAW GIZMOS ***
    lineShader.use();
    lineShader.setUniformValue(lineShaderViewHandle, viewMatrix);
    lineShader.setUniformValue(lineShaderProjectionHandle, projectionMatrix);

    // Axes gizmo
    if(bar_showAxesGizmo)
    {
        lineShader.setVertexBuffer(primitives::line, sizeof(primitives::line), "positionAttribute");

        // Red axis
        lineShader.setUniformValue(lineShaderModelHandle, redAxisGizmoModel);
        lineShader.setUniformValue(lineShaderColorHandle, glm::vec3(1,0,0));
        lineShader.draw(GL_LINES);

        // Green axis
        lineShader.setUniformValue(lineShaderModelHandle, greenAxisGizmoModel);
        lineShader.setUniformValue(lineShaderColorHandle, glm::vec3(0,1,0));
        lineShader.draw(GL_LINES);

        // Blue axis
        lineShader.setUniformValue(lineShaderModelHandle, blueAxisGizmoModel);
        lineShader.setUniformValue(lineShaderColorHandle, glm::vec3(0,0,1));
        lineShader.draw(GL_LINES);
    }

    // Volume Extent gizmo
    if(bar_showVolumeExtentGizmo)
    {
        lineShader.setVertexBuffer(primitives::lineCube, sizeof(primitives::lineCube), "positionAttribute");

        // Original size
        lineShader.setUniformValue(lineShaderModelHandle, volumeExtentGizmoOriginalModel);
        lineShader.setUniformValue(lineShaderColorHandle, glm::vec3(0.7f, 0.7f, 0.7f));
        lineShader.draw(GL_LINES);

        // Clipped size
        lineShader.setUniformValue(lineShaderModelHandle, volumeExtentGizmoClippedModel);
        lineShader.setUniformValue(lineShaderColorHandle, glm::vec3(1.0f, 0.0f, 0.0f));
        lineShader.draw(GL_LINES);
    }

    // Sun gizmo
    if(bar_showSunGizmo)
    {
        GLfloat* pSunGizmoVertices = new GLfloat[6*7];
        glm::vec3 sunDirection = bar_sunDirection * 0.6f;

        // Main ray
        pSunGizmoVertices[0] = -sunDirection.x;
        pSunGizmoVertices[1] = -sunDirection.y;
        pSunGizmoVertices[2] = -sunDirection.z;
        pSunGizmoVertices[3] = sunDirection.x;
        pSunGizmoVertices[4] = sunDirection.y;
        pSunGizmoVertices[5] = sunDirection.z;

        // Decoration
        pSunGizmoVertices[6] = -sunDirection.x;
        pSunGizmoVertices[7] = -sunDirection.y;
        pSunGizmoVertices[8] = -sunDirection.z;
        pSunGizmoVertices[9] = -sunDirection.x + RENDERER_SUN_GIZMO_RAY_LENGTH;
        pSunGizmoVertices[10] = -sunDirection.y;
        pSunGizmoVertices[11] = -sunDirection.z;

        pSunGizmoVertices[12] = -sunDirection.x;
        pSunGizmoVertices[13] = -sunDirection.y;
        pSunGizmoVertices[14] = -sunDirection.z;
        pSunGizmoVertices[15] = -sunDirection.x - RENDERER_SUN_GIZMO_RAY_LENGTH;
        pSunGizmoVertices[16] = -sunDirection.y;
        pSunGizmoVertices[17] = -sunDirection.z;

        pSunGizmoVertices[18] = -sunDirection.x;
        pSunGizmoVertices[19] = -sunDirection.y;
        pSunGizmoVertices[20] = -sunDirection.z;
        pSunGizmoVertices[21] = -sunDirection.x;
        pSunGizmoVertices[22] = -sunDirection.y + RENDERER_SUN_GIZMO_RAY_LENGTH;
        pSunGizmoVertices[23] = -sunDirection.z;

        pSunGizmoVertices[24] = -sunDirection.x;
        pSunGizmoVertices[25] = -sunDirection.y;
        pSunGizmoVertices[26] = -sunDirection.z;
        pSunGizmoVertices[27] = -sunDirection.x;
        pSunGizmoVertices[28] = -sunDirection.y - RENDERER_SUN_GIZMO_RAY_LENGTH;
        pSunGizmoVertices[29] = -sunDirection.z;

        pSunGizmoVertices[30] = -sunDirection.x;
        pSunGizmoVertices[31] = -sunDirection.y;
        pSunGizmoVertices[32] = -sunDirection.z;
        pSunGizmoVertices[33] = -sunDirection.x;
        pSunGizmoVertices[34] = -sunDirection.y;
        pSunGizmoVertices[35] = -sunDirection.z + RENDERER_SUN_GIZMO_RAY_LENGTH;

        pSunGizmoVertices[36] = -sunDirection.x;
        pSunGizmoVertices[37] = -sunDirection.y;
        pSunGizmoVertices[38] = -sunDirection.z;
        pSunGizmoVertices[39] = -sunDirection.x;
        pSunGizmoVertices[40] = -sunDirection.y;
        pSunGizmoVertices[41] = -sunDirection.z - RENDERER_SUN_GIZMO_RAY_LENGTH;

        lineShader.setVertexBuffer(pSunGizmoVertices, 24*7, "positionAttribute");
        lineShader.setUniformValue(lineShaderModelHandle, glm::mat4(1.0f));
        lineShader.setUniformValue(lineShaderColorHandle, glm::vec3(1.0f, 1.0f, 0.0f));
        lineShader.draw(GL_LINES);

        delete[] pSunGizmoVertices;
    }
}

void Renderer::setTfHandle(GLint handle)
{
    if(handle >= 0)
    {
        if(pTfManager->getTf(handle) != NULL)
        {
            tfHandle = handle;
        }
        bar_activeTf.setValue(tfHandle);
    }
}

void Renderer::setRcHandle(GLint handle)
{
    if(handle >= 0)
    {
        if(pRcManager->getRc(handle) != NULL)
        {
            rcHandle = handle;
        }
        bar_activeRc.setValue(rcHandle);
    }
}

void Renderer::setVolumeHandle(GLint handle)
{
    if(handle >= 0)
    {
        if(pVolumeManager->getVolume(handle) != NULL)
        {
            volumeHandle = handle;
        }
        bar_activeVolume.setValue(volumeHandle);
    }
}

void Renderer::newRc()
{
    setRcHandle(pRcManager->newRc());
}

void Renderer::reloadRc()
{
    pRcManager->reloadRc(rcHandle);
    setRcHandle(rcHandle);
}

void Renderer::saveRc()
{
    pRcManager->saveRc(rcHandle, bar_overwriteExisting);
}

void Renderer::loadRc()
{
    setRcHandle(pRcManager->loadRc(bar_pathToExternRc));
}

void Renderer::resetCamera()
{
    camera.reset(RENDERER_CAMERA_CENTER, glm::radians(RENDERER_CAMERA_ALPHA), glm::radians(RENDERER_CAMERA_BETA), RENDERER_CAMERA_RADIUS);
}

void Renderer::resetRaycasterProperties()
{
    if(rcHandle >= 0)
    {
        pRcManager->getRc(rcHandle)->setProperties(RaycasterProperties());
    }
}

void Renderer::handleInput(InputData inputData)
{
    // Call super method
    Viewport::handleInput(inputData);

    // *** CAMERA ***

    // Rotation
    if(inputData.mouse_drag_left)
    {
        GLfloat x = glm::degrees(camera.getAlpha()) + relativeCursorPosDelta.x * getAspectRatio() * RENDERER_CAMERA_ROTATION_SPEED;
        GLfloat y =  glm::degrees(camera.getBeta()) + relativeCursorPosDelta.y * RENDERER_CAMERA_ROTATION_SPEED;
        camera.setAlpha(glm::radians(x));
        camera.setBeta(glm::radians(y));
    }

    // Radius
    if(inputData.scroll_changed)
    {
        GLfloat radius = camera.getRadius() - (inputData.scroll.y * RENDERER_CAMERA_ZOOM_SPEED);
        camera.setRadius(radius);
    }
}

void Renderer::updateBarVariables()
{
    // Call super method
    Viewport::updateBarVariables();

    // Call update methods
    bar_cameraAlpha.update();
    bar_cameraBeta.update();
    bar_cameraRadius.update();
    bar_activeTf.update();
    bar_tfName.update();
    bar_activeRc.update();
    bar_rcName.update();
    bar_rcUseERT.update();
    bar_rcUseLocalIllumination.update();
    bar_rcUseJittering.update();
    bar_rcStepSize.update();
    bar_rcOuterIteration.update();
    bar_rcInnerIteration.update();
    bar_rcAlphaThreshold.update();
    bar_rcJitteringRangeMultiplier.update();
    bar_activeVolume.update();
    bar_volumeName.update();
    bar_rcUseGradientAlphaMultiplier.update();
    bar_rcUseFresnelAlphaMultiplier.update();
    bar_rcUseReflectionColorMultiplier.update();
    bar_rcUseEmissionColorMultiplier.update();
    bar_rcUseSimpleESS.update();
    bar_rcNormalRangeMultiplier.update();
    bar_rcFresnelPower.update();
    bar_rcUseNormalsOfClassifiedData.update();
    bar_rcUsePreintegration.update();
    bar_rcUseShadows.update();
    bar_rcUseAdaptiveSampling.update();
    bar_rcMinAdaptiveStepSize.update();
    bar_rcMaxAdaptiveStepSize.update();
    bar_rcUseVoxelSpacedSampling.update();
    bar_rcStepSizeMultiplier.update();
    bar_rcMinAdaptiveStepSizeMultiplier.update();
    bar_rcMaxAdaptiveStepSizeMultiplier.update();
    bar_rcUseExtentPreservingJittering.update();
    bar_rcUseExtentAwareNormals.update();
}

void Renderer::useBarVariables()
{
    // Call super method
    Viewport::useBarVariables();

    // Camera
    if(bar_cameraAlpha.hasChanged())
    {
        camera.setAlpha(glm::radians(bar_cameraAlpha.getValue()));
    }
    if(bar_cameraBeta.hasChanged())
    {
        camera.setBeta(glm::radians(bar_cameraBeta.getValue()));
    }
    if(bar_cameraRadius.hasChanged())
    {
        camera.setRadius(bar_cameraRadius.getValue());
    }

    // Update active transferfunction if necessary
    if(bar_activeTf.hasChanged())
    {
        if(bar_activeTf.getValue() < 0)
        {
            bar_activeTf.setValue(0);
        }
        else
        {
            setTfHandle(bar_activeTf.getValue());
            bar_activeTf.setValue(tfHandle);
        }
    }

    // Update transferfunction's name
    if(bar_tfName.hasChanged())
    {
        pTfManager->getTf(tfHandle)->rename(bar_tfName.getValue());
    }

    // Update active raycaster if necessary
    if(bar_activeRc.hasChanged())
    {
        if(bar_activeRc.getValue() < 0)
        {
            bar_activeRc.setValue(0);
        }
        else
        {
            setRcHandle(bar_activeRc.getValue());
            bar_activeRc.setValue(rcHandle);
        }
    }

    // Update raycasters's name
    if(bar_rcName.hasChanged())
    {
        pRcManager->getRc(tfHandle)->rename(bar_rcName.getValue());
    }

    // Update active volume if necessary
    if(bar_activeVolume.hasChanged())
    {
        if(bar_activeVolume.getValue() < 0)
        {
            bar_activeVolume.setValue(0);
        }
        else
        {
            setVolumeHandle(bar_activeVolume.getValue());
            bar_activeVolume.setValue(volumeHandle);
        }
    }

    // Update volumes's name
    if(bar_volumeName.hasChanged())
    {
        pVolumeManager->getVolume(volumeHandle)->rename(bar_volumeName.getValue());
    }

    // Raycaster
    if(rcHandle >= 0)
    {
        // *** PROPERTIES ***

        // Only if bar has changed the value, change it. Otherwise there would be conflicts when using multiple renderers
        RaycasterProperties properties = pRcManager->getRc(rcHandle)->getProperties();
        GLboolean propertiesChanged = GL_FALSE;

        if(bar_rcStepSize.hasChanged())
        {
            properties.stepSize = bar_rcStepSize.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcStepSizeMultiplier.hasChanged())
        {
            properties.stepSizeMultiplier = bar_rcStepSizeMultiplier.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcMinAdaptiveStepSize.hasChanged())
        {
            properties.minAdaptiveStepSize = bar_rcMinAdaptiveStepSize.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcMaxAdaptiveStepSize.hasChanged())
        {
            properties.maxAdaptiveStepSize = bar_rcMaxAdaptiveStepSize.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcMinAdaptiveStepSizeMultiplier.hasChanged())
        {
            properties.minAdaptiveStepSizeMultiplier = bar_rcMinAdaptiveStepSizeMultiplier.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcMaxAdaptiveStepSizeMultiplier.hasChanged())
        {
            properties.maxAdaptiveStepSizeMultiplier = bar_rcMaxAdaptiveStepSizeMultiplier.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcOuterIteration.hasChanged())
        {
            properties.outerIterations = bar_rcOuterIteration.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcInnerIteration.hasChanged())
        {
            properties.innerIterations = bar_rcInnerIteration.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcAlphaThreshold.hasChanged())
        {
            properties.alphaThreshold = bar_rcAlphaThreshold.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcJitteringRangeMultiplier.hasChanged())
        {
            properties.jitteringRangeMultiplier = bar_rcJitteringRangeMultiplier.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcNormalRangeMultiplier.hasChanged())
        {
            properties.normalRangeMultiplier = bar_rcNormalRangeMultiplier.getValue();
            propertiesChanged = GL_TRUE;
        }
        if(bar_rcFresnelPower.hasChanged())
        {
            properties.fresnelPower = bar_rcFresnelPower.getValue();
            propertiesChanged = GL_TRUE;
        }

        // Only when a variable changed its value, set properties
        if(propertiesChanged)
        {
            pRcManager->getRc(rcHandle)->setProperties(properties);
        }

        // *** DEFINES ***
        if(bar_rcUseERT.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseERT(bar_rcUseERT.getValue());
        }

        if(bar_rcUseLocalIllumination.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseLocalIllumination(bar_rcUseLocalIllumination.getValue());
        }

        if(bar_rcUseShadows.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseShadows(bar_rcUseShadows.getValue());
        }

        if(bar_rcUseJittering.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseJittering(bar_rcUseJittering.getValue());
        }

        if(bar_rcUseExtentPreservingJittering.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseExtentPreservingJittering(bar_rcUseExtentPreservingJittering.getValue());
        }

        if(bar_rcUseSimpleESS.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseSimpleESS(bar_rcUseSimpleESS.getValue());
        }

        if(bar_rcUseNormalsOfClassifiedData.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseNormalsOfClassifiedData(bar_rcUseNormalsOfClassifiedData.getValue());
        }

        if(bar_rcUseExtentAwareNormals.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseExtentAwareNormals(bar_rcUseExtentAwareNormals.getValue());
        }

        if(bar_rcUseGradientAlphaMultiplier.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseGradientAlphaMultiplier(bar_rcUseGradientAlphaMultiplier.getValue());
        }

        if(bar_rcUseFresnelAlphaMultiplier.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseFresnelAlphaMultiplier(bar_rcUseFresnelAlphaMultiplier.getValue());
        }

        if(bar_rcUseReflectionColorMultiplier.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseReflectionColorMultiplier(bar_rcUseReflectionColorMultiplier.getValue());
        }

        if(bar_rcUseEmissionColorMultiplier.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseEmissionColorMultiplier(bar_rcUseEmissionColorMultiplier.getValue());
        }

        if(bar_rcUsePreintegration.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUsePreintegration(bar_rcUsePreintegration.getValue());
        }

        if(bar_rcUseAdaptiveSampling.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseAdaptiveSampling(bar_rcUseAdaptiveSampling.getValue());
        }

        if(bar_rcUseVoxelSpacedSampling.hasChanged())
        {
            pRcManager->getRc(rcHandle)->setUseVoxelSpacedSampling(bar_rcUseVoxelSpacedSampling.getValue());
        }

    }
}

void Renderer::fillBarVariables()
{
    // Call super method
    Viewport::fillBarVariables();

    // TfName
    if(tfHandle >= 0)
    {
        bar_tfName.setValue(pTfManager->getTf(tfHandle)->getName());
    }

    // RcName
    if(rcHandle >= 0)
    {
        bar_rcName.setValue(pRcManager->getRc(rcHandle)->getName());
    }

    // VolumeName
    if(volumeHandle >= 0)
    {
        bar_volumeName.setValue(pVolumeManager->getVolume(volumeHandle)->getName());
    }

    // Camera
    bar_cameraAlpha.setValue(glm::degrees(camera.getAlpha()));
    bar_cameraBeta.setValue(glm::degrees(camera.getBeta()));
    bar_cameraRadius.setValue(camera.getRadius());

    // Raycaster
    if(rcHandle >= 0)
    {
        Raycaster* pRaycaster = pRcManager->getRc(rcHandle);

        // Raycaster defines
        bar_rcUseERT.setValue(pRaycaster->getUseERT());
        showBarVariable("Alpha Threshold", bar_rcUseERT.getValue());

        bar_rcUseLocalIllumination.setValue(pRaycaster->getUseLocalIllumination());
        showBarVariable("Lighting", bar_rcUseLocalIllumination.getValue());
        showBarVariable("Use Shadows", bar_rcUseLocalIllumination.getValue());

        bar_rcUseShadows.setValue(pRaycaster->getUseShadows());

        bar_rcUseJittering.setValue(pRaycaster->getUseJittering());
        showBarVariable("Jittering Range Multiplier", bar_rcUseJittering.getValue());
        showBarVariable("Use Extent Preserving Jittering", bar_rcUseJittering.getValue());

        bar_rcUseExtentPreservingJittering.setValue(pRaycaster->getUseExtentPreservingJittering());

        bar_rcUseSimpleESS.setValue(pRaycaster->getUseSimpleESS());

        bar_rcUseNormalsOfClassifiedData.setValue(pRaycaster->getUseNormalsOfClassifiedData());

        bar_rcUseExtentAwareNormals.setValue(pRaycaster->getUseExtentAwareNormals());

        bar_rcUsePreintegration.setValue(pRaycaster->getUsePreintegration());

        bar_rcUseGradientAlphaMultiplier.setValue(pRaycaster->getUseGradientAlphaMultiplier());

        bar_rcUseFresnelAlphaMultiplier.setValue(pRaycaster->getUseFresnelAlphaMultiplier());
        showBarVariable("Fresnel Power", bar_rcUseFresnelAlphaMultiplier.getValue());

        bar_rcUseReflectionColorMultiplier.setValue(pRaycaster->getUseReflectionColorMultiplier());

        bar_rcUseEmissionColorMultiplier.setValue(pRaycaster->getUseEmissionColorMultiplier());

        bar_rcUseAdaptiveSampling.setValue(pRaycaster->getUseAdaptiveSampling());
        GLboolean adaptiveSampling = bar_rcUseAdaptiveSampling.getValue();

        bar_rcUseVoxelSpacedSampling.setValue(pRaycaster->getUseVoxelSpacedSampling());
        GLboolean voxelSpacedSampling = bar_rcUseVoxelSpacedSampling.getValue();

        // Only show necessary values
        if(adaptiveSampling && !voxelSpacedSampling)
        {
            showBarVariable("Step Size", GL_FALSE);
            showBarVariable("Step Size Multiplier", GL_FALSE);
            showBarVariable("Min Adaptive Step Size", GL_TRUE);
            showBarVariable("Max Adaptive Step Size", GL_TRUE);
            showBarVariable("Min Adaptive Step Size Multiplier", GL_TRUE);
            showBarVariable("Max Adaptive Step Size Multiplier", GL_TRUE);
        }
        else if(!adaptiveSampling && voxelSpacedSampling)
        {
            showBarVariable("Step Size", GL_FALSE);
            showBarVariable("Step Size Multiplier", GL_TRUE);
            showBarVariable("Min Adaptive Step Size", GL_FALSE);
            showBarVariable("Max Adaptive Step Size", GL_FALSE);
            showBarVariable("Min Adaptive Step Size Multiplier", GL_FALSE);
            showBarVariable("Max Adaptive Step Size Multiplier", GL_FALSE);
        }
        else if(adaptiveSampling && voxelSpacedSampling)
        {
            showBarVariable("Step Size", GL_FALSE);
            showBarVariable("Step Size Multiplier", GL_FALSE);
            showBarVariable("Min Adaptive Step Size", GL_FALSE);
            showBarVariable("Max Adaptive Step Size", GL_FALSE);
            showBarVariable("Min Adaptive Step Size Multiplier", GL_TRUE);
            showBarVariable("Max Adaptive Step Size Multiplier", GL_TRUE);
        }
        else
        {
            showBarVariable("Step Size", GL_TRUE);
            showBarVariable("Step Size Multiplier", GL_TRUE);
            showBarVariable("Min Adaptive Step Size", GL_FALSE);
            showBarVariable("Max Adaptive Step Size", GL_FALSE);
            showBarVariable("Min Adaptive Step Size Multiplier", GL_FALSE);
            showBarVariable("Max Adaptive Step Size Multiplier", GL_FALSE);
        }

        // Raycaster properties
        RaycasterProperties properties = pRaycaster->getProperties();
        bar_rcStepSize.setValue(properties.stepSize);
        bar_rcStepSizeMultiplier.setValue(properties.stepSizeMultiplier);
        bar_rcMinAdaptiveStepSize.setValue(properties.minAdaptiveStepSize);
        bar_rcMaxAdaptiveStepSize.setValue(properties.maxAdaptiveStepSize);
        bar_rcMinAdaptiveStepSizeMultiplier.setValue(properties.minAdaptiveStepSizeMultiplier);
        bar_rcMaxAdaptiveStepSizeMultiplier.setValue(properties.maxAdaptiveStepSizeMultiplier);
        bar_rcOuterIteration.setValue(properties.outerIterations);
        bar_rcInnerIteration.setValue(properties.innerIterations);
        bar_rcAlphaThreshold.setValue(properties.alphaThreshold);
        bar_rcJitteringRangeMultiplier.setValue(properties.jitteringRangeMultiplier);
        bar_rcNormalRangeMultiplier.setValue(properties.normalRangeMultiplier);
        bar_rcFresnelPower.setValue(properties.fresnelPower);
    }
}

static void TW_CALL newRcButtonCallback(void* clientData)
{
    reinterpret_cast<Renderer*>(clientData)->newRc();
}

static void TW_CALL reloadRcButtonCallback(void* clientData)
{
    reinterpret_cast<Renderer*>(clientData)->reloadRc();
}

static void TW_CALL saveRcButtonCallback(void* clientData)
{
    reinterpret_cast<Renderer*>(clientData)->saveRc();
}

static void TW_CALL loadRcButtonCallback(void* clientData)
{
    reinterpret_cast<Renderer*>(clientData)->loadRc();
}

static void TW_CALL resetCameraButtonCallback(void* clientData)
{
    reinterpret_cast<Renderer*>(clientData)->resetCamera();
}

static void TW_CALL resetRaycasterPropertiesButtonCallback(void* clientData)
{
    reinterpret_cast<Renderer*>(clientData)->resetRaycasterProperties();
}
