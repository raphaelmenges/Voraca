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
 * TfPoint
 *--------------
 * Simple data structure for points of the transferfunction.
 * Manipulation is done in Transferfunction.
 * Objects of this class just hold the information.
 * Does not know wether it is selected.
 *
 */

#ifndef TFPOINT_H_
#define TFPOINT_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Logger.h"
#include "Shader.h"
#include "TfPointValue.h"

const glm::vec3 TFPOINT_LEFT_CONTROL_POINT_COLOR(0,1,0);
const glm::vec3 TFPOINT_RIGHT_CONTROL_POINT_COLOR(0,0,1);
const glm::vec3 TFPOINT_LINKED_CONTROL_POINT_COLOR(0,1,1);
const GLfloat TFPOINT_CONTROL_POINT_DISTANCE = 0.025f;
const GLfloat TFPOINT_POS_Z = -0.75f;
const GLfloat TFPOINT_POINTS_SCALE = 0.05f;
const GLfloat TFPOINT_CONTROL_POINTS_RELATIVE_SCALE = 0.5f;
const GLfloat TFPOINT_CONTROL_POINTS_RELATIVE_POS_Z = -0.05f;
const GLfloat TFPOINT_CONTROL_LINES_RELATIVE_SCALE = 2.00f;
const GLfloat TFPOINT_CONTROL_LINES_RELATIVE_POS_Z = 0.2f;
const GLfloat TFPOINT_INTERSECTION_BIAS = 0.2f;
const glm::vec3 TFPOINT_BORDER_COLOR_UNSELECTED(0.1f,0.1f,0.1f);
const glm::vec3 TFPOINT_BORDER_COLOR_SELECTED(0.2f,0.5f,0.7f);
const glm::vec3 TFPOINT_BORDER_COLOR_ACTIVE(0.4f,0.9f,1.0f);
const GLboolean TFPOINT_CONTROL_POINTS_LINKED = GL_FALSE;

/** First and last point may not be moved in x-Direction */
enum TfPointLocation
{
    TFPOINT_LOCATION_NORMAL, TFPOINT_LOCATION_LEFTEND, TFPOINT_LOCATION_RIGHTEND
};

/** States of a tfPoint */
enum TfPointState
{
    TFPOINT_STATE_UNSELECTED, TFPOINT_STATE_SELECTED, TFPOINT_STATE_ACTIVE
};

class TfPoint
{
public:
    TfPoint();
    ~TfPoint();

    /** Shaders are managed by transferfunction */
    void init(
        GLint handle,
        TfPointLocation location,
        glm::vec2 point,
        Shader* pPointShader,
        Shader* pControlPointShader,
        Shader* pControlLineShader,
        GLuint pointShaderModelHandle,
        GLuint pointShaderViewHandle,
        GLuint pointShaderInnerColorHandle,
        GLuint pointShaderBorderColorHandle,
        GLuint controlPointShaderModelHandle,
        GLuint controlPointShaderViewHandle,
        GLuint controlLineShaderModelHandle,
        GLuint controlLineShaderViewHandle,
        GLuint controlLineShaderColorHandle);

    /** Draws itself, the control points and the lines to them */
    void draw(TfPointState state, glm::vec3 innerColor, GLfloat scale, GLfloat aspectRatio, GLboolean drawControlPoints, glm::mat4 viewMatrix);

    /** Public intersection tests */
    GLboolean intersectWithPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio);
    GLboolean intersectWithLeftControlPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio);
    GLboolean intersectWithRightControlPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio);

    /** Toggle link of control points */
    GLboolean toggleControlPointsLinked();

    /** Simple getter */
    GLint getHandle() const;
    TfPointLocation getLocation() const;
    glm::vec2 getPoint() const;
    glm::vec2 getLeftControlPoint() const;
    glm::vec2 getRightControlPoint() const;
    TfPointValue getValue() const;
    GLboolean getControlPointsLinked() const;

    /** Simple setter */
    void setPoint(glm::vec2 point);
    void setLeftControlPoint(glm::vec2 leftControlPoint);
    void setRightControlPoint(glm::vec2 rightControlPoint);
    void setValue(TfPointValue value);
    void setControlPointsLinked(GLboolean linked);

    /** For sorting a comparator is needed */
    bool operator<(const TfPoint &rhs) const;

protected:
    /** Internal function to test for intersection with abitrary point */
    GLboolean intersect(glm::vec2 coord, glm::vec2 point, glm::vec2 size);

    /** Calculates point size for this viewport*/
    glm::vec2 calcRealPointSize(GLfloat scale, GLfloat aspectRatio);

    /** For linked control points */
    glm::vec2 mirrorPoint(glm::vec2 point);

    /** When control points are linked */
    void adjustLinkedControlPoints();

    /** Basic information */
    GLint handle;
    TfPointLocation location;
    glm::vec2 point;
    glm::vec2 leftControlPoint;
    glm::vec2 rightControlPoint;
    GLboolean controlPointsLinked;

    /** Value */
    TfPointValue value;

    /** Pointer to used shaders */
    Shader* pPointShader;
    Shader* pControlPointShader;
    Shader* pControlLineShader;

    /** Lot of handles to fill shaders */
    GLuint pointShaderModelHandle;
    GLuint pointShaderViewHandle;
    GLuint pointShaderInnerColorHandle;
    GLuint pointShaderBorderColorHandle;
    GLuint controlPointShaderModelHandle;
    GLuint controlPointShaderViewHandle;
    GLuint controlLineShaderModelHandle;
    GLuint controlLineShaderViewHandle;
    GLuint controlLineShaderColorHandle;
};
#endif
