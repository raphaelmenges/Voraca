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
 * Logger
 *--------------
 * Logs events to console or file or both or does nothing at all.
 *
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>

/** Defines for easier access */
#define LogInfo(s) Logger::print(LOG_INFO, s);
#define LogWarning(s) Logger::print(LOG_WARNING, s);
#define LogError(s) Logger::print(LOG_ERROR, s);
#define LogShader(h, f) Logger::print(h, f);

/** Types of logging texts */
enum LogType {LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_SHADER};

/** Settings */
const GLboolean LOG_CONSOLE_OUTPUT = GL_TRUE;
const GLboolean LOG_FILE_OUTPUT = GL_TRUE;
const LogType LOG_LEVEL = LOG_INFO;

class Logger
{
public:
    /** Print text to log */
    static void print(LogType type, std::string text);

    /** Print shader log to log */
    static void print(GLuint shaderHandle, std::string file);

private:
    Logger();
    ~Logger();

    /** Private copy constuctor */
    Logger(Logger const&) {};

    /** Private assignment operator */
    Logger& operator=(Logger const&) {return *this;};

    /** The pointer to the instance */
    static Logger* pInstance;
};

#endif
