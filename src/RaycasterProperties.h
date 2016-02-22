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
 * RaycasterProperties
 *--------------
 * Properties of a raycaster.
 *
 */

#ifndef RAYCASTERPROPERTIES_H_
#define RAYCASTERPROPERTIES_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

const GLfloat RAYCASTERPROPERTIES_STEP_SIZE = 0.008f;
const GLfloat RAYCASTERPROPERTIES_STEP_SIZE_MIN = 0.0001f;
const GLfloat RAYCASTERPROPERTIES_STEP_SIZE_MAX = 0.5f;
const GLint RAYCASTERPROPERTIES_OUTER_ITERATIONS = 100;
const GLint RAYCASTERPROPERTIES_OUTER_ITERATIONS_MIN = 1;
const GLint RAYCASTERPROPERTIES_OUTER_ITERATIONS_MAX = 1000;
const GLint RAYCASTERPROPERTIES_INNER_ITERATIONS = 3;
const GLint RAYCASTERPROPERTIES_INNER_ITERATIONS_MIN = 1;
const GLint RAYCASTERPROPERTIES_INNER_ITERATIONS_MAX = 100;
const GLfloat RAYCASTERPROPERTIES_ALPHA_THRESHOLD = 0.98f;
const GLfloat RAYCASTERPROPERTIES_ALPHA_THRESHOLD_MIN = 0.0f;
const GLfloat RAYCASTERPROPERTIES_ALPHA_THRESHOLD_MAX = 1.0f;
const GLfloat RAYCASTERPROPERTIES_JITTERING_RANGE_MULTIPLIER = 2.5f;
const GLfloat RAYCASTERPROPERTIES_JITTERING_RANGE_MULTIPLIER_MIN = 0.0f;
const GLfloat RAYCASTERPROPERTIES_JITTERING_RANGE_MULTIPLIER_MAX = 50.0f;
const GLfloat RAYCASTERPROPERTIES_NORMAL_RANGE_MULTIPLIER = 1.1f;
const GLfloat RAYCASTERPROPERTIES_NORMAL_RANGE_MULTIPLIER_MIN = 0.1f;
const GLfloat RAYCASTERPROPERTIES_NORMAL_RANGE_MULTIPLIER_MAX = 10.0f;
const GLfloat RAYCASTERPROPERTIES_FRESNEL_POWER = 3.0f;
const GLfloat RAYCASTERPROPERTIES_FRESNEL_POWER_MIN = 0.1f;
const GLfloat RAYCASTERPROPERTIES_FRESNEL_POWER_MAX = 100.0f;
const GLfloat RAYCASTERPROPERTIES_MIN_ADAPTIVE_STEP_SIZE= 0.002f;
const GLfloat RAYCASTERPROPERTIES_MAX_ADAPTIVE_STEP_SIZE= 0.01f;
const GLfloat RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER = 1.0f;
const GLfloat RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MIN = 0.05f;
const GLfloat RAYCASTERPROPERTIES_STEP_SIZE_MULTIPLIER_MAX = 10.0f;
const GLfloat RAYCASTERPROPERTIES_MIN_ADAPTIVE_STEP_SIZE_MULTIPLIER= 0.9f;
const GLfloat RAYCASTERPROPERTIES_MAX_ADAPTIVE_STEP_SIZE_MULTIPLIER= 1.1f;

class RaycasterProperties
{
public:
    RaycasterProperties();
    ~RaycasterProperties();

    void reset();

    GLfloat stepSize;
    GLfloat stepSizeMultiplier;
    GLint outerIterations;
    GLint innerIterations;
    GLfloat alphaThreshold;
    GLfloat jitteringRangeMultiplier;
    GLfloat normalRangeMultiplier;
    GLfloat fresnelPower;

    /** For adaptive sampling */
    GLfloat minAdaptiveStepSize;
    GLfloat maxAdaptiveStepSize;
    GLfloat minAdaptiveStepSizeMultiplier;
    GLfloat maxAdaptiveStepSizeMultiplier;
};


#endif
