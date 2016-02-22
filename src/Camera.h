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
 * Camera
 *--------------
 * Holds information about a camera.
 * Used by Renderer.
 *
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Logger.h"

const GLfloat CAMERA_BETA_BIAS = 0.0001f;

class Camera
{
public:
    Camera();
    ~Camera();

    void init(glm::vec3 center, GLfloat alpha, GLfloat beta, GLfloat radius, GLfloat minRadius, GLfloat maxRadius);

    void reset(glm::vec3 center, GLfloat alpha, GLfloat beta, GLfloat radius);

    void setCenter(glm::vec3 center);
    void setAlpha(GLfloat alpha);
    void setBeta(GLfloat beta);
    void setRadius(GLfloat radius);

    glm::mat4 getViewMatrix();
    glm::vec3 getPosition();
    glm::vec3 getCenter() const;
    GLfloat getAlpha() const;
    GLfloat getBeta() const;
    GLfloat getRadius() const;

protected:
    void calcPosition();
    void clampValues();
    void clampAlpha();
    void clampBeta();
    void clampRadius();

    GLfloat alpha;
    GLfloat beta;
    GLfloat radius;
    GLfloat minRadius;
    GLfloat maxRadius;
    glm::vec3 center;
    glm::vec3 position;
};

#endif
