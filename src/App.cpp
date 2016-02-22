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

#include "App.h"

App::App()
{
}

App::~App()
{
}

void App::init()
{
    LogInfo("Welcome to Voraca");
    LogInfo("Development Version " + UT::to_string(APP_VERSION));
    LogInfo("Created by Raphael Menges");

    // Try to read values from launch file
    GLint errorsInLaunchFile = 0;
    GLuint launchWindowWidth;
    GLuint launchWindowHeight;
    GLboolean launchFullscreen;
    errorsInLaunchFile += Launch::getWindowWidth(launchWindowWidth);
    errorsInLaunchFile += Launch::getWindowHeight(launchWindowHeight);
    errorsInLaunchFile += Launch::isFullscreen(launchFullscreen);

    // GLFW and GLEW
    if(errorsInLaunchFile > 0)
    {
        windowWidth = APP_RES_WIDTH;
        windowHeight = APP_RES_HEIGHT;
    }
    else
    {
        windowWidth = launchWindowWidth;
        windowHeight = launchWindowHeight;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    if(errorsInLaunchFile > 0)
    {
        pWindow = glfwCreateWindow(windowWidth, windowHeight, APP_TITLE.c_str(), NULL, NULL);
    }
    else
    {
        if(launchFullscreen)
            pWindow = glfwCreateWindow(windowWidth, windowHeight, APP_TITLE.c_str(), glfwGetPrimaryMonitor(), NULL);
        else
            pWindow = glfwCreateWindow(windowWidth, windowHeight, APP_TITLE.c_str(), NULL, NULL);
    }

    if (!pWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(pWindow);
    ogl_LoadFunctions();

    LogInfo("Initial window resolution: " + UT::to_string(windowWidth) + std::string("x") +  UT::to_string(windowHeight));

    // Log hardware and maximum OpenGL version
    const GLubyte* renderer = glGetString(GL_RENDERER);
    LogInfo(std::string(reinterpret_cast<char const*>(renderer)));
    const GLubyte* version = glGetString(GL_VERSION);
    LogInfo(std::string(reinterpret_cast<char const*>(version)));
    GLint maxVolumeResolution;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &maxVolumeResolution);
    LogInfo("Maximum volume resolution per dimension: " + UT::to_string(maxVolumeResolution));

    // Member initializations
    pInput = Input::instantiate(pWindow, windowWidth, windowHeight);
    pEditor = new Editor();
    pEditor->init(windowWidth, windowHeight);

    // OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Other
    lastTime = (GLfloat)glfwGetTime();
}

void App::run()
{
    EditorCallToApp call = EDITOR_CALL_NONE;

    while(!glfwWindowShouldClose(pWindow))
    {
        // Toggle fullscreen mode
        if(call == EDITOR_CALL_TOGGLE_FULLSCREEN)
        {
            LogInfo("Toggle fullscreen not implemented, yet");
        }

        //  Get input data and decide whether to stop after this run
        InputData inputData = pInput->getInputData();
        GLboolean quit = GL_FALSE;
        if(inputData.key_int == GLFW_KEY_Q && inputData.key_action == GLFW_PRESS && inputData.key_mods == GLFW_MOD_CONTROL)
        {
            quit = GL_TRUE;
        }

        // Check window for resizing
        if(checkForWindowSizeChange())
        {
            pInput->windowResize(windowWidth, windowHeight);
            pEditor->windowResize(windowWidth, windowHeight);
        }

        // Clear buffers
        glClearColor(APP_CLEAR_COLOR.r, APP_CLEAR_COLOR.g, APP_CLEAR_COLOR.b, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // Calc time per frame
        GLfloat tpf = calcDeltaTime();

        // Updates
        call = pEditor->update(tpf, inputData);

        // Draws
        pEditor->draw();

        // Resets for next run
        pInput->resetInputData();

        // GLFW updates
        glfwSwapBuffers(pWindow);
        glfwPollEvents();

        // Quit?
        if(call == EDITOR_CALL_QUIT || quit)
        {
            LogInfo("Quit");
            glfwSetWindowShouldClose(pWindow, GL_TRUE);
        }

        // Reset?
        if(call == EDITOR_CALL_RESET)
        {
            LogInfo("Reset started");
            if(pEditor != NULL)
            {
                pEditor->terminate();
                delete pEditor;
                pEditor = NULL;
            }
            pEditor = new Editor();
            pEditor->init(windowWidth, windowHeight);
            LogInfo("Reset done");
        }
    }

    terminate();
}

void App::terminate()
{
    if(pEditor != NULL)
    {
        pEditor->terminate();
        delete pEditor;
        pEditor = NULL;
    }
    if(pWindow != NULL)
    {
        glfwDestroyWindow(pWindow);
    }
    glfwTerminate();
}

GLfloat App::calcDeltaTime()
{
    // Simple version, seams to work even for empty canvas
    GLfloat currentTime = (GLfloat)glfwGetTime();
    GLfloat deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}

GLboolean App::checkForWindowSizeChange()
{
    GLboolean changed = false;
    GLint width, height;
    glfwGetWindowSize(pWindow, &width, &height);
    if(width != windowWidth)
    {
        windowWidth = width;
        changed = true;
    }
    if(height != windowHeight)
    {
        windowHeight = height;
        changed = true;
    }

    if(changed)
    {
        LogInfo("Window resized: " + UT::to_string(windowWidth) + std::string("x") +  UT::to_string(windowHeight));
    }

    return changed;
}
