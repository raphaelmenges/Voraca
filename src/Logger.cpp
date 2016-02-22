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

#include "Logger.h"

// Global static pointer to the single instance
Logger* Logger::pInstance = NULL;

Logger::Logger()
{
    // Delete old log
    if(LOG_FILE_OUTPUT)
    {
        std::ofstream file;
        file.open("Log.txt", std::ios::out | std::ios::ate);
        file.close();
    }
}

Logger::~Logger()
{
}

void Logger::print(LogType type, std::string text)
{
    // Check wether logging is necessary
    if (LOG_LEVEL > type)
    {
        return;
    }

    // Exists an instance?
    if(pInstance == NULL)
    {
        // Create instance of it on stack
        static Logger logger;
        pInstance = &logger;
    }

    // Get prefix
    std::string prefix;
    if(type == LOG_INFO)
    {
        prefix = "INFO   : ";
    }
    else if(type == LOG_WARNING)
    {
        prefix = "WARNING: ";
    }
    else if(type == LOG_ERROR)
    {
        prefix = "ERROR  : ";
    }
    else
    {
        prefix = "SHADER : ";
    }

    // Print it to the output
    if(LOG_CONSOLE_OUTPUT)
    {
        // Use console for output
        std::cout << prefix << text << std::endl;
    }
    if(LOG_FILE_OUTPUT)
    {
        // Use file for output
        std::ofstream file;
        file.open("Log.txt", std::ios::out | std::ios::app);
        file << prefix << text << "\n";
        file.close();
    }
}

void Logger::print(GLuint shaderHandle, std::string file)
{
    // Get length of log
    GLint log_length = 0;
    if(glIsShader(shaderHandle))
    {
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &log_length);
    }
    else
    {
        LogError(file + std::string(" was tried to log as shader"));
    }

    if(log_length > 1)
    {
        // Copy log to chars
        char *log = new char[log_length];

        if(glIsShader(shaderHandle))
        {
            glGetShaderInfoLog(shaderHandle, log_length, NULL, log);
        }

        // If there is a log, print it
        print(LOG_SHADER, (file + std::string(" compiler message\n \n")) + std::string(log));

        // Delete chars
        delete[] log;
    }
}
