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
 * Shader
 *--------------
 * Class for reading, compiling and using OpenGL shader.
 * Only vertex and fragment shader are supported. Mesh data is
 * bound to shader (not really good decision, but working for this project).
 *
 */

#ifndef SHADER_H_
#define SHADER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "Logger.h"

const std::string SHADER_PATH = std::string(DATA_PATH) + "/_Resources/Shaders/";

class Shader
{
public:
    Shader();
    ~Shader();

    void loadShaders(std::string vertexFile, std::string fragmentFile);

    void loadShaders(std::string vertexFile, std::string fragmentFile, std::vector<std::string> vertexDefines, std::vector<std::string> fragmentDefines);

    void setVertexBuffer(GLfloat vertices[], GLuint size, std::string location);

    GLint getUniformHandle(std::string location) const;
    void setUniformValue(GLint uniformHandle, const glm::mat4& value);
    void setUniformValue(GLint uniformHandle, const glm::vec2& value);
    void setUniformValue(GLint uniformHandle, const glm::vec3& value);
    void setUniformValue(GLint uniformHandle, const glm::vec4& value);
    void setUniformValue(GLint uniformHandle, const GLfloat& value);
    void setUniformValue(GLint uniformHandle, const GLint& value);
    void setUniformTexture(GLint uniformHandle, GLint textureHandle, GLenum mode);

    void use();

    void draw(GLenum mode);

protected:
    GLboolean readShaderFromFile(std::string file, GLint handle, std::vector<std::string> defines);

    GLuint programHandle;
    GLuint vertexHandle;
    GLuint fragmentHandle;
    GLuint vertexArray;
    GLuint vertexBuffer;
    GLint vertexCount;
    GLboolean programInitialized;
    GLboolean vertexArrayInitialized;
    GLint textureSlotCounter;
};

#endif
