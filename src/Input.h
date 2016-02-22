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
 * Input
 *--------------
 * This class uses the singleton pattern to get the callbacks
 * by GLFW and fill a InputData object. This object is used
 * further.
 *
 */

#ifndef INPUT_H_
#define INPUT_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Logger.h"
#include "InputData.h"

class Input
{
public:
    /** Creates single instance if none exisits */
    static Input* instantiate(GLFWwindow* pWindow, GLint windowWidth, GLint windowHeight);

    /** GLFW callback for keyboard */
    static void keyCallback(GLFWwindow* pWindow, GLint key, GLint scancode, GLint action, GLint mods);

    /** GLFW callback for cursor */
    static void cursorCallback(GLFWwindow* pWindow, GLdouble xpos, GLdouble ypos);

    /** GLFW callback for mouse buttons */
    static void buttonsCallback(GLFWwindow* pWindow, GLint button, GLint action, GLint mods);

    /** GLFW callback for scrolling */
    static void scrollCallback(GLFWwindow* pWindow, GLdouble xoffset, GLdouble yoffset);

    /** Get copy of input data */
    InputData getInputData() const;

    /** Reset input values */
    void resetInputData();

    /** Set window size to adjust mouse coords */
    void windowResize(GLint windowWidth, GLint windowHeight);

private:
    Input();
    ~Input();

    /** Initialization */
    void init(GLFWwindow* pWindow, GLint windowWidth, GLint windowHeight);

    /** Class methods for callback values*/
    void key(GLint key, GLint action, GLint mods);
    void cursor(GLfloat xpos, GLfloat ypos);
    void buttons(GLint button, GLint action, GLint mods);
    void scroll(GLfloat xoffset, GLfloat yoffset);

    /** Maps keys from GLFW3 to GLFW2 */
    GLint getGLFW2Key(GLint key, GLint mods) const;

    /** Members */
    InputData inputData;

    /** Private copy constuctor */
    Input(Input const&) {};

    /** Private assignment operator */
    Input& operator=(Input const&) {return *this;};

    /** The pointer to the single instance */
    static Input* pInstance;

    /** Window size */
    GLint windowWidth;
    GLint windowHeight;
};

#endif
