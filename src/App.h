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
 * App
 *--------------
 * Owner of GLFW window and editor object. Controls
 * input object and resets it after each update.
 * Calculates time per frame.
 *
 */

#ifndef APP_H_
#define APP_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <string>

#include "Logger.h"
#include "Input.h"
#include "Editor.h"
#include "Utilities.h"
#include "Launch.h"

const GLfloat APP_VERSION = 1.0f;
const GLuint APP_RES_WIDTH = 1280;
const GLuint APP_RES_HEIGHT = 720;
const std::string APP_TITLE = "Voraca";
const glm::vec3 APP_CLEAR_COLOR(0.2f, 0.25f, 0.3f);

class App
{
public:
    App();
    ~App();

    void init();
    void run();

protected:
    void terminate();
    GLfloat calcDeltaTime();
    GLboolean checkForWindowSizeChange();

    /** Pointer to glfw window */
    GLFWwindow* pWindow;

    /** App is owner of input object */
    Input* pInput;

    /** Pointer to editor */
    Editor* pEditor;

    /** Some members */
    GLuint windowWidth;
    GLuint windowHeight;
    GLfloat lastTime;
};

#endif
