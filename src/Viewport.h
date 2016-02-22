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
 * Viewport
 *--------------
 * Basic viewport class featuring
 * clearing and highlighting when
 * selected.
 *
 */

#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "AntTweakBar.h"

#include <string>
#include <vector>

#include "Logger.h"
#include "Input.h"
#include "BarVariable.h"
#include "Utilities.h"

const GLfloat VIEWPORT_BORDER_COLOR_UNSELECTED_MULTIPLIER = 0.5f;
const glm::vec3 VIEWPORT_INNER_COLOR(0.0f, 0.0f, 0.0f);
const GLint VIEWPORT_BORDER_WIDTH = 3;

/** Each subclass should be listed here */
enum ViewportType
{
    VIEWPORT_NONE, VIEWPORT_RENDERER, VIEWPORT_SLICER, VIEWPORT_TFEDITOR
};

class Viewport
{
public:
    Viewport();
    virtual ~Viewport();

    /** Standard methods */
    virtual void init(GLint handle, std::string title, glm::vec3 color, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive);
    virtual ViewportType update(GLfloat tpf, InputData inputData, GLboolean selected);
    virtual void draw();
    virtual void terminate();

    /** Viewports needs to know wether resolution changes etc */
    virtual void viewportResize(GLint windowWidth, GLint windowHeight);

    /** When selection of viewport is cancelled */
    virtual void unselect();

    /** Method for manager to test for intersection */
    GLboolean intersect(glm::vec2 coord);

    /** Returns handle */
    GLint getHandle() const;

    /** For drawing, it is useful to know its aspect ratio */
    GLfloat getAspectRatio() const;

    /** Set status of bar */
    void setBarActive(GLboolean barActive);

    /** Get type of viewport */
    ViewportType getType() const;

    /** Get postion and width */
    GLfloat getRelativePosX() const;
    GLfloat getRelativePosY() const;
    GLfloat getRelativeWidth() const;
    GLfloat getRelativeHeight() const;

protected:
    /** Called by update method */
    virtual void handleInput(InputData inputData);

    /** Update bar variables */
    virtual void updateBarVariables();

    /** Use bar vars */
    virtual void useBarVariables();

    /** Fill bar variables which could have been manipulated
    from outside with new data every frame (done before drawing) */
    virtual void fillBarVariables();

    /** Hide or show bar variables */
    void showBarVariable(std::string name, GLboolean show);

    /** Type of viewport */
    ViewportType type;
    ViewportType bar_type;

    /** AntTweakBar */
    TwBar* pBar;

    /** Basics */
    std::string title;
    GLboolean selected;
    GLint handle;
    glm::vec3 color;

    /** Absolute positions for internal use */
    GLint viewportX;
    GLint viewportY;
    GLint viewportWidth;
    GLint viewportHeight;

    /** Relative positions as input */
    GLfloat relativePosX;
    GLfloat relativePosY;
    GLfloat relativeWidth;
    GLfloat relativeHeight;

    /** Relative cursor position */
    glm::vec2 relativeCursorPos;
    glm::vec2 relativeCursorPosDelta;

    /** Status of bar */
    GLboolean barActive;
};

#endif
