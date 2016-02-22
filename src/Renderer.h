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
 * Renderer
 *--------------
 * Subclass of Viewport.
 * Does combine transferfunction, volume and raycaster.
 * Fills the raycaster with information.
 *
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "glm/gtc/matrix_transform.hpp"
#include "PicoPNG/picopng.h"

#include "Viewport.h"
#include "Raycaster.h"
#include "RaycasterProperties.h"
#include "Camera.h"
#include "TfManager.h"
#include "RcManager.h"
#include "VolumeManager.h"

const GLfloat RENDERER_CAMERA_ROTATION_SPEED = 200;
const GLfloat RENDERER_CAMERA_ZOOM_SPEED = 0.2f;
const GLfloat RENDERER_CAMERA_RADIUS_MIN = 0.2f;
const GLfloat RENDERER_CAMERA_RADIUS_MAX = 10.0f;
const glm::vec3 RENDERER_CAMERA_CENTER(0, 0, 0);
const GLfloat RENDERER_CAMERA_ALPHA = 45.0f;
const GLfloat RENDERER_CAMERA_BETA = 80.0f;
const GLfloat RENDERER_CAMERA_RADIUS = 4.0f;
const GLfloat RENDERER_FIELD_OF_VIEW = 20.0f;
const GLfloat RENDERER_FIELD_OF_VIEW_MIN = 5.0f;
const GLfloat RENDERER_FIELD_OF_VIEW_MAX = 80.0f;
const GLfloat RENDERER_RC_STEP_SIZE_STEP = 0.0001f;
const GLfloat RENDERER_RC_STEP_SIZE_STEP_MULTIPLIER = 0.1f;
const GLfloat RENDERER_RC_JITTERING_RANGE_MULTIPLIER_STEP = 0.1f;
const GLfloat RENDERER_RC_FRESNEL_ALPHA_MULTIPLIER_STEP = 0.1f;
const GLfloat RENDERER_RC_ALPHA_THRESHOLD_STEP = 0.01f;
const GLfloat RENDERER_RC_NORMAL_RANGE_MULTIPLIER_STEP = 0.1f;
const glm::vec3 RENDERER_SUN_DIRECTION(0,-1,0);
const glm::vec3 RENDERER_SUN_COLOR(1,1,1);
const GLfloat RENDERER_SUN_BRIGHTNESS = 1.0f;
const glm::vec3 RENDERER_AMBIENT_COLOR(0.25f,0.3f,0.5f);
const GLfloat RENDERER_VOLUME_CLIPPING_STEPS = 0.01f;
const GLfloat RENDERER_VOLUME_EXTENT_MIN = 0.01f;
const GLfloat RENDERER_VOLUME_EXTENT_MAX = 1;
const GLboolean RENDERER_SHOW_AXES_GIZMO = GL_FALSE;
const GLboolean RENDERER_SHOW_VOLUME_EXTENT_GIZMO = GL_FALSE;
const glm::vec3 RENDERER_VOLUME_EXTENT(1.0f, 1.0f, 1.0f);
const glm::vec3 RENDERER_VOLUME_EXTENT_OFFSET(0.0f, 0.0f, 0.0f);
const GLboolean RENDERER_SHOW_SUN_GIZMO = GL_FALSE;
const GLfloat RENDERER_SUN_GIZMO_RAY_LENGTH = 0.1f;
const GLfloat RENDERER_SUN_BRIGHTNESS_STEP = 0.05f;
const GLfloat RENDERER_SUN_BRIGHTNESS_MIN = 0.00f;
const GLfloat RENDERER_SUN_BRIGHTNESS_MAX = 10.0f;
const GLboolean RENDERER_SHOW_IMPORTANCE_VOLUME = GL_FALSE;

class Renderer : public Viewport
{
public:
    Renderer();
    virtual ~Renderer();

    void init(TfManager* pTfManager, RcManager* pRcManager, VolumeManager* pVolumeManager, GLint handle, glm::vec3 color, std::string title, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive);
    ViewportType update(GLfloat tpf, InputData inputData, GLboolean selected);
    void draw();

    /** Set active transferfunction */
    void setTfHandle(GLint handle);

    /** Set active raycaster */
    void setRcHandle(GLint handle);

    /** Set active volume */
    void setVolumeHandle(GLint handle);

    /** Methods for bar buttons */
    void newRc();
    void reloadRc();
    void saveRc();
    void loadRc();
    void resetCamera();
    void resetRaycasterProperties();

protected:
    /** Viewport method, too */
    virtual void handleInput(InputData inputData);

    /** Update bar variables */
    virtual void updateBarVariables();

    /** Use bar vars */
    virtual void useBarVariables();

    /** Fill bar variables with new data if could have been manipulated from outside (done before drawing) */
    virtual void fillBarVariables();

    /** Camera for 3D-Space */
    Camera camera;

    /** Pointer to tfMananger of editor */
    TfManager* pTfManager;

    /** Pointer to rcManager of editor */
    RcManager* pRcManager;

    /** Pointer to volumeManager of editor */
    VolumeManager* pVolumeManager;

    /** Currently used transferfunction */
    GLint tfHandle;

    /** Currently used raycaster */
    GLint rcHandle;

    /** Currently used volume */
    GLint volumeHandle;

    /** Reflection map */
    GLuint reflectionHandle;

    /** Shader for gizmos */
    Shader lineShader;

    /** Volume extent gizmo shader */
    Shader volumeExtentGizmo;

    /** Handles for line shader */
    GLuint lineShaderModelHandle;
    GLuint lineShaderViewHandle;
    GLuint lineShaderProjectionHandle;
    GLuint lineShaderColorHandle;

    /** Model matrices for axes gizmo */
    glm::mat4 redAxisGizmoModel;
    glm::mat4 greenAxisGizmoModel;
    glm::mat4 blueAxisGizmoModel;

    /** Matrices for volume extent */
    glm::mat4 volumeExtentGizmoModelScale;
    glm::mat4 volumeExtentGizmoModelRotation;
    glm::mat4 volumeExtentGizmoOriginalModel;
    glm::mat4 volumeExtentGizmoClippedModel;

    /** Simple booleans to decide whether to show gizmos */
    GLboolean bar_showAxesGizmo;
    GLboolean bar_showVolumeExtentGizmo;
    GLboolean bar_showSunGizmo;
    GLboolean bar_showImportanceVolume;

    /** Shader for importance volume */
    Shader importanceVolumeShader;

    /** Handles for importance volume shader */
    GLuint importanceVolumeShaderModelScaleHandle;
    GLuint importanceVolumeModelRotationHandle;
    GLuint importanceVolumeViewHandle;
    GLuint importanceVolumeProjectionHandle;
    GLuint importanceVolumeCameraPosHandle;
    GLuint importanceVolumeVolumeTextureHandle;
    GLuint importanceVolumeShaderMirrorUVWHandle;
    GLuint importanceVolumeExtentHandle;
    GLuint importanceVolumeExtentOffsetHandle;

    /** Matrices of importance volume shader */
    glm::mat4 importanceVolumeModelScaleMatrix;
    glm::mat4 importanceVolumeModelRotationMatrix;
    glm::mat4 importanceVolumeExtentMatrix;
    glm::vec3 importanceVolumeMirrorUVW;

    /** Matrices shared by multiple shaders */
    glm::mat4 projectionMatrix;

    /** Bar variables (exclusive for each renderer)*/
    GLfloat bar_fieldOfView;
    glm::vec3 bar_sunDirection;
    glm::vec3 bar_sunColor;
    GLfloat bar_sunBrightness;
    glm::vec3 bar_ambientColor;
    glm::vec3 bar_volumeExtent;
    glm::vec3 bar_volumeExtentOffset;
    std::string bar_pathToExternRc;
    GLboolean bar_overwriteExisting;

    /** Bar variables */
    BarVariable<GLint> bar_activeTf;
    BarVariable<GLint> bar_activeRc;
    BarVariable<GLint> bar_activeVolume;
    BarVariable<std::string> bar_tfName;
    BarVariable<std::string> bar_rcName;
    BarVariable<std::string> bar_volumeName;
    BarVariable<GLboolean> bar_rcUseERT;
    BarVariable<GLboolean> bar_rcUseLocalIllumination;
    BarVariable<GLboolean> bar_rcUseShadows;
    BarVariable<GLboolean> bar_rcUseJittering;
    BarVariable<GLboolean> bar_rcUseExtentPreservingJittering;
    BarVariable<GLboolean> bar_rcUseGradientAlphaMultiplier;
    BarVariable<GLboolean> bar_rcUseFresnelAlphaMultiplier;
    BarVariable<GLboolean> bar_rcUseReflectionColorMultiplier;
    BarVariable<GLboolean> bar_rcUseEmissionColorMultiplier;
    BarVariable<GLboolean> bar_rcUseSimpleESS;
    BarVariable<GLboolean> bar_rcUseNormalsOfClassifiedData;
    BarVariable<GLboolean> bar_rcUseExtentAwareNormals;
    BarVariable<GLboolean> bar_rcUsePreintegration;
    BarVariable<GLboolean> bar_rcUseAdaptiveSampling;
    BarVariable<GLboolean> bar_rcUseVoxelSpacedSampling;
    BarVariable<GLfloat> bar_rcStepSize;
    BarVariable<GLfloat> bar_rcStepSizeMultiplier;
    BarVariable<GLfloat> bar_rcMinAdaptiveStepSize;
    BarVariable<GLfloat> bar_rcMaxAdaptiveStepSize;
    BarVariable<GLfloat> bar_rcMinAdaptiveStepSizeMultiplier;
    BarVariable<GLfloat> bar_rcMaxAdaptiveStepSizeMultiplier;
    BarVariable<GLint> bar_rcOuterIteration;
    BarVariable<GLint> bar_rcInnerIteration;
    BarVariable<GLfloat> bar_rcAlphaThreshold;
    BarVariable<GLfloat> bar_rcJitteringRangeMultiplier;
    BarVariable<GLfloat> bar_rcNormalRangeMultiplier;
    BarVariable<GLfloat> bar_rcFresnelPower;
    BarVariable<GLfloat> bar_cameraAlpha;
    BarVariable<GLfloat> bar_cameraBeta;
    BarVariable<GLfloat> bar_cameraRadius;
};

static void TW_CALL newRcButtonCallback(void* clientData);
static void TW_CALL reloadRcButtonCallback(void* clientData);
static void TW_CALL saveRcButtonCallback(void* clientData);
static void TW_CALL loadRcButtonCallback(void* clientData);
static void TW_CALL resetCameraButtonCallback(void* clientData);
static void TW_CALL resetRaycasterPropertiesButtonCallback(void* clientData);

#endif
