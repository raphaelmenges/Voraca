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
 * Editor
 *-------------_
 * Manages volumes and gives access to
 * them through own bar.
 * Owner of all managers.
 *
 */

#ifndef EDITOR_H_
#define EDITOR_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "AntTweakBar.h"

#include "Logger.h"
#include "Input.h"
#include "ViewportManager.h"
#include "TfManager.h"
#include "RcManager.h"
#include "VolumeManager.h"
#include "Utilities.h"
#include "Launch.h"

const std::string EDITOR_BAR_TITLE = "Editor";
const GLfloat EDITOR_BAR_VOLUME_VALUE_OFFSET_STEP = 0.05f;
const GLfloat EDITOR_BAR_VOLUME_VALUE_SCALE_STEP = 0.1f;
const GLfloat EDITOR_BAR_VOLUME_VOXEL_SCALE_MULTIPLIER_STEP = 0.05f;

enum EditorCallToApp
{
    EDITOR_CALL_NONE, EDITOR_CALL_QUIT, EDITOR_CALL_RESET, EDITOR_CALL_TOGGLE_FULLSCREEN
};

enum EditorViewportPreset
{
    EDITOR_VIEWPORT_PRESET_A, EDITOR_VIEWPORT_PRESET_B, EDITOR_VIEWPORT_PRESET_C, EDITOR_VIEWPORT_PRESET_D
};

class Editor
{
public:
    Editor();
    ~Editor();

    /** Standard methods */
    void init(GLint windowWidth, GLint windowHeight);
    EditorCallToApp update(GLfloat tpf, InputData inputData);
    void draw();
    void terminate();

    void windowResize(GLint windowWidth, GLint windowHeight);

    /** Methods for callbacks */
    void quit();
    void reset();
    void toggleFullscreen();
    void reloadVolume();
    void saveVolume();
    void loadVolume();
    void importPVM();
    void importDAT();

protected:
    /** Bars want input, too */
    void forwardInputToBars(InputData inputData);

    /** Set volume handle, returns wether sucessfull */
    bool setVolumeHandle(GLint handle);

    /** Set volume in all viewports */
    void setVolumeInAllViewports(GLint handle);

    /** Update bar variables */
    void updateBarVariables();

    /** Use bar vars */
    void useBarVariables();

    /** Fill bar variables which could have been manipulated
    from outside with new data every frame (done before drawing) */
    void fillBarVariables();

    /** Owner of all managers */
    ViewportManager viewportManager;
    TfManager tfManager;
    RcManager rcManager;
    VolumeManager volumeManager;

    /** Owns controls over volumes */
    GLint volumeHandle;

    /** AntTweakBar */
    TwBar* pBar;

    /** Booleans controlled by super keys */
    GLboolean barsActive;
    GLboolean benchmarkActive;

    /** Some markers */
    GLuint inputHandledByBars;
    GLuint mouseButtonsHandledByBars;

    /** Window size */
    GLuint windowWidth;
    GLuint windowHeight;

    /** Non-template bar variabels */
    GLfloat bar_tpf;
    GLfloat bar_fps;
    std::string bar_pathToExternVolume;
    GLboolean bar_overwriteExisting;
    GLboolean bar_setVolumeInAllViewports;

    /** Bar variables */
    BarVariable<GLint> bar_activeVolume;
    BarVariable<std::string> bar_volumeName;
    BarVariable<glm::vec3> bar_volumeVoxelScaleMultiplier;
    BarVariable<GLfloat> bar_volumeValueOffset;
    BarVariable<GLfloat> bar_volumeValueScale;
    BarVariable<glm::vec3> bar_volumeEulerZXZRotation;
    BarVariable<GLboolean> bar_volumeMirrorX;
    BarVariable<GLboolean> bar_volumeMirrorY;
    BarVariable<GLboolean> bar_volumeMirrorZ;
    BarVariable<GLboolean> bar_volumeLinearFiltering;

    /** Control app via booleans */
    GLboolean doQuit;
    GLboolean doReset;
    GLboolean doToggleFullscreen;

    /** Variables for benchmarking */
    GLdouble benchmarkCounter;
    GLdouble benchmarkTime;

    /** Viewport preset stuff */
    EditorViewportPreset bar_viewportPreset;
    GLint previousViewportPreset;
};

/** AntTweakBar needs a explicit copy functions to be able to edit std::string */
void TW_CALL copyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString);

/** Callbacks */
static void TW_CALL quitButtonCallback(void* clientData);
static void TW_CALL resetButtonCallback(void* clientData);
static void TW_CALL toggleFullscreenButtonCallback(void* clientData);
static void TW_CALL reloadVolumeButtonCallback(void* clientData);
static void TW_CALL saveVolumeButtonCallback(void* clientData);
static void TW_CALL loadVolumeButtonCallback(void* clientData);
static void TW_CALL importPVMButtonCallback(void* clientData);
static void TW_CALL importDATButtonCallback(void* clientData);

#endif
