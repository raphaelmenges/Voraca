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
 * ViewportMananger
 *--------------
 * Manages viewports.
 *
 */

#ifndef VIEWPORTMANAGER_H_
#define VIEWPORTMANAGER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"

#include <map>
#include <string>

#include "Logger.h"
#include "Viewport.h"
#include "Input.h"
#include "Renderer.h"
#include "TfEditor.h"
#include "Slicer.h"
#include "TfManager.h"
#include "RcManager.h"
#include "VolumeManager.h"

static glm::vec3 viewportColors[] =
{
    glm::vec3(1.0f,0.0f,0.0f),
    glm::vec3(0.0f,1.0f,0.0f),
    glm::vec3(0.0f,0.0f,1.0f),
    glm::vec3(1.0f,1.0f,0.0f),
    glm::vec3(0.0f,1.0f,1.0f),
    glm::vec3(1.0f,0.0f,1.0f),
    glm::vec3(1.0f,1.0f,1.0f),
};

class ViewportManager
{
public:
    ViewportManager();
    ~ViewportManager();

    /** Standard methods */
    void init(GLint windowWidth, GLint windowHeight, TfManager* pTfManager, RcManager* pRcManager, VolumeManager* pVolumeManager, GLboolean barsActive);
    void update(GLfloat tpf, InputData inputData);
    void draw();
    void terminate();

    /** Returns handle to viewport */
    GLint addViewport(ViewportType type, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint handle = -1);

    /** Deletes viewport */
    void deleteViewport(GLint handle);

    /** Deletes all viewports */
    void deleteAllViewports();

    /** If window of app was resized, please tell */
    void windowResize(GLint windowWidth, GLint windowHeight);

    /** Returns pointer of viewport with given handle */
    Viewport* getViewport(GLint handle);

    /** Unselect the currently selected viewport */
    void unselectSelectedViewport();

    /** Set status of viewports' bars */
    void setBarsActive(GLboolean barsActive);

    /** Set volume in all viewports */
    void setVolumeInAllViewports(GLint volumeHandle);

protected:
    /** Returns handle to viewport at given coordinates */
    GLint getViewportAt(glm::vec2 coord);

    /** Map with viewports */
    std::map<GLint, Viewport*> viewports;

    /** Handle of the currently selected viewport */
    GLint selectedViewportHandle;

    /** Window infos */
    GLuint windowWidth;
    GLuint windowHeight;

    /** Pointer to tfManager */
    TfManager* pTfManager;

    /** Pointer to rcManager */
    RcManager* pRcManager;

    /** Pointer to volumeManager */
    VolumeManager* pVolumeManager;

    /** Counter for used viewport handles */
    GLint viewportHandleCounter;

    /** Bars active */
    GLboolean barsActive;
};

#endif
