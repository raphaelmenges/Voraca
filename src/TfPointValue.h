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
 * TfPointValue
 *--------------
 * Values saved in each TfPoint.
 *
 */

#ifndef TFPOINTVALUE_H_
#define TFPOINTVALUE_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

const glm::vec3 TFPOINTVALUE_COLOR(0.5f,0.5f,0.5f);
const GLfloat TFPOINTVALUE_AMBIENT_MULTIPLIER = 0.5f;
const GLfloat TFPOINTVALUE_AMBIENT_MULTIPLIER_MIN = 0;
const GLfloat TFPOINTVALUE_AMBIENT_MULTIPLIER_MAX = 1;
const GLfloat TFPOINTVALUE_SPECULAR_MULTIPLIER = 0.5f;
const GLfloat TFPOINTVALUE_SPECULAR_MULTIPLIER_MIN = 0;
const GLfloat TFPOINTVALUE_SPECULAR_MULTIPLIER_MAX = 10;
const GLfloat TFPOINTVALUE_SPECULAR_SATURATION = 0.5f;
const GLfloat TFPOINTVALUE_SPECULAR_SATURATION_MIN = 0;
const GLfloat TFPOINTVALUE_SPECULAR_SATURATION_MAX = 1;
const GLfloat TFPOINTVALUE_SPECULAR_POWER = 16;
const GLfloat TFPOINTVALUE_SPECULAR_POWER_MIN = 0.5;
const GLfloat TFPOINTVALUE_SPECULAR_POWER_MAX = 128;
const GLfloat TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER = 0.0f;
const GLfloat TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER_MIN = 0;
const GLfloat TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER_MAX = 1;
const GLfloat TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER = 0.0f;
const GLfloat TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER_MIN = 0;
const GLfloat TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER_MAX = 1;
const GLfloat TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER = 0.0f;
const GLfloat TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER_MIN = 0;
const GLfloat TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER_MAX = 1;
const GLfloat TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER = 0.0f;
const GLfloat TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER_MIN = 0;
const GLfloat TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER_MAX = 1;

class TfPointValue
{
public:
    TfPointValue();
    ~TfPointValue();

    void reset();

    // Texture 1
    glm::vec3 color;

    // Texture 2
    GLfloat ambientMultiplier;
    GLfloat specularMultiplier;
    GLfloat specularSaturation;
    GLfloat specularPower;

    // Texture 3
    GLfloat gradientAlphaMultiplier;
    GLfloat fresnelAlphaMultiplier;
    GLfloat reflectionColorMultiplier;
    GLfloat emissionColorMultiplier;
};

#endif
