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

#include "TfPoint.h"

TfPoint::TfPoint()
{
	controlPointsLinked = TFPOINT_CONTROL_POINTS_LINKED;
}

TfPoint::~TfPoint()
{
}

void TfPoint::init(	GLint handle,
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
			GLuint controlLineShaderColorHandle)
{
	// Handle
	this->handle = handle;

	// Location
	this->location = location;

	if(!TFPOINT_LOCATION_NORMAL)
	{
		controlPointsLinked = GL_FALSE;
	}

	// Point position
	setPoint(point);

	setLeftControlPoint(glm::vec2(point.x - TFPOINT_CONTROL_POINT_DISTANCE, point.y));
	setRightControlPoint(glm::vec2(point.x + TFPOINT_CONTROL_POINT_DISTANCE, point.y));

	// Shaders
	this->pPointShader = pPointShader; 
	this->pControlPointShader = pControlPointShader;
	this->pControlLineShader = pControlLineShader;
	this->pointShaderModelHandle = pointShaderModelHandle;
	this->pointShaderViewHandle = pointShaderViewHandle;
	this->pointShaderInnerColorHandle = pointShaderInnerColorHandle;
	this->pointShaderBorderColorHandle = pointShaderBorderColorHandle;
	this->controlPointShaderModelHandle = controlPointShaderModelHandle;
	this->controlPointShaderViewHandle = controlPointShaderViewHandle;
	this->controlLineShaderModelHandle = controlLineShaderModelHandle;
	this->controlLineShaderViewHandle = controlLineShaderViewHandle;
	this->controlLineShaderColorHandle = controlLineShaderColorHandle;
}

void TfPoint::draw(TfPointState state, glm::vec3 innerColor, GLfloat scale, GLfloat aspectRatio, GLboolean drawControlPoints, glm::mat4 viewMatrix)
{
	// *** PREPARE DRAWING ***

	// Predefintion of matrices
	glm::mat4 pointModelMatrix;
	glm::mat4 leftControlPointModelMatrix;
	glm::mat4 rightControlPointModelMatrix;
	glm::mat4 leftControlLineModelMatrix;
	glm::mat4 rightControlLineModelMatrix;

	// Calc size of points
	glm::vec2 pointSize = calcRealPointSize(scale, aspectRatio);

	// Create model matrix for point
	pointModelMatrix = glm::mat4(1.0f);
	pointModelMatrix = glm::translate(pointModelMatrix, glm::vec3(point.x, point.y, TFPOINT_POS_Z));
	pointModelMatrix = glm::scale(pointModelMatrix, glm::vec3(pointSize, 1));	

	// Create model matrices for control points if necessary
	if(drawControlPoints == GL_TRUE)
	{
		// XY scale of control points
		glm::vec2 controlPointSize = pointSize * TFPOINT_CONTROL_POINTS_RELATIVE_SCALE;

		// Y scale of line
		GLfloat controlLinesScale = controlPointSize.x * TFPOINT_CONTROL_LINES_RELATIVE_SCALE;

		if(location != TFPOINT_LOCATION_LEFTEND)
		{
			// Model matrix for left control point
			leftControlPointModelMatrix = glm::mat4(1.0f);
			leftControlPointModelMatrix = glm::translate(leftControlPointModelMatrix, glm::vec3(leftControlPoint.x, leftControlPoint.y, TFPOINT_POS_Z + TFPOINT_CONTROL_POINTS_RELATIVE_POS_Z));
			leftControlPointModelMatrix = glm::scale(leftControlPointModelMatrix, glm::vec3(controlPointSize, 1));

			// Distance between point and control point
			GLfloat dist = glm::distance(leftControlPoint, point);

			// Angle between line and x axis
			GLfloat angle = 0;
			if(point.y < leftControlPoint.y)
			{
				angle = glm::degrees(glm::acos((leftControlPoint.x-point.x)/dist));
			}
			else
			{
				angle = 180 + glm::degrees(glm::acos((-leftControlPoint.x+point.x)/dist));
			}

			// Fix for deformation because of non-square viewport
			GLfloat rotationScaleFix = glm::abs((glm::sin(glm::radians(angle))));

			// Model matrix for line
			leftControlLineModelMatrix = glm::mat4(1.0f);
			leftControlLineModelMatrix = glm::translate(leftControlLineModelMatrix, glm::vec3(point, TFPOINT_POS_Z + TFPOINT_CONTROL_LINES_RELATIVE_POS_Z));
			leftControlLineModelMatrix = glm::rotate(leftControlLineModelMatrix, glm::radians(angle), glm::vec3(0, 0, 1));
			leftControlLineModelMatrix = glm::scale(leftControlLineModelMatrix, glm::vec3(dist, (1-rotationScaleFix) * controlLinesScale + (rotationScaleFix) * (controlLinesScale / aspectRatio), 1));
			leftControlLineModelMatrix = glm::translate(leftControlLineModelMatrix, glm::vec3(0, -0.5f, 0));
		}
		if(location != TFPOINT_LOCATION_RIGHTEND)
		{
			// Model matrix for right control point
			rightControlPointModelMatrix = glm::mat4(1.0f);
			rightControlPointModelMatrix = glm::translate(rightControlPointModelMatrix, glm::vec3(rightControlPoint.x, rightControlPoint.y, TFPOINT_POS_Z + TFPOINT_CONTROL_POINTS_RELATIVE_POS_Z));
			rightControlPointModelMatrix = glm::scale(rightControlPointModelMatrix, glm::vec3(controlPointSize, 1));

			// Distance between point and control point
			GLfloat dist = glm::distance(rightControlPoint, point);

			// Angle between line and x axis
			GLfloat angle = 0;
			if(point.y < rightControlPoint.y)
			{
				angle = glm::degrees(glm::acos((rightControlPoint.x - point.x)/dist));
			}
			else
			{
				angle = 180 + glm::degrees(glm::acos((-rightControlPoint.x + point.x)/dist));
			}

			// Fix for deformation because of non-square viewport
			GLfloat rotationScaleFix = glm::abs((glm::sin(glm::radians(angle))));

			// Model matrix for line
			rightControlLineModelMatrix = glm::mat4(1.0f);
			rightControlLineModelMatrix = glm::translate(rightControlLineModelMatrix, glm::vec3(point, TFPOINT_POS_Z + TFPOINT_CONTROL_LINES_RELATIVE_POS_Z));
			rightControlLineModelMatrix = glm::rotate(rightControlLineModelMatrix, glm::radians(angle), glm::vec3(0, 0, 1));
			rightControlLineModelMatrix = glm::scale(rightControlLineModelMatrix, glm::vec3(dist, (1-rotationScaleFix) * controlLinesScale + (rotationScaleFix) * (controlLinesScale / aspectRatio), 1));
			rightControlLineModelMatrix = glm::translate(rightControlLineModelMatrix, glm::vec3(0, -0.5f, 0));
		}
	}

	// *** DRAW ***

	// Check state
	glm::vec3 borderColor;
	if(state == TFPOINT_STATE_UNSELECTED)
	{
		borderColor = TFPOINT_BORDER_COLOR_UNSELECTED;
	}
	else if(state == TFPOINT_STATE_SELECTED)
	{
		borderColor = TFPOINT_BORDER_COLOR_SELECTED;
	}
	else
	{
		borderColor = TFPOINT_BORDER_COLOR_ACTIVE;
	}

	// Draw point in own color
	pPointShader->use();
	pPointShader->setUniformValue(pointShaderInnerColorHandle, innerColor);
	pPointShader->setUniformValue(pointShaderBorderColorHandle, borderColor);
	pPointShader->setUniformValue(pointShaderModelHandle, pointModelMatrix);
	pPointShader->setUniformValue(pointShaderViewHandle, viewMatrix);
	pPointShader->draw(GL_TRIANGLE_FAN);

	if(drawControlPoints)
	{
		// Draw control points
		if(location != TFPOINT_LOCATION_LEFTEND)
		{
			// Draw left control point
			pControlPointShader->use();
			pControlPointShader->setUniformValue(controlPointShaderModelHandle, leftControlPointModelMatrix);
			pControlPointShader->setUniformValue(controlPointShaderViewHandle, viewMatrix);
			pPointShader->draw(GL_TRIANGLE_FAN);

			// Draw line
			pControlLineShader->use();
			pControlLineShader->setUniformValue(controlLineShaderModelHandle, leftControlLineModelMatrix);
			pControlLineShader->setUniformValue(controlLineShaderViewHandle, viewMatrix);
			if(controlPointsLinked)
			{
				pControlLineShader->setUniformValue(controlLineShaderColorHandle, TFPOINT_LINKED_CONTROL_POINT_COLOR);
			}
			else
			{
				pControlLineShader->setUniformValue(controlLineShaderColorHandle, TFPOINT_LEFT_CONTROL_POINT_COLOR);
			}
			pControlLineShader->draw(GL_TRIANGLES);
		}
		if(location != TFPOINT_LOCATION_RIGHTEND)
		{
			// Draw right control point
			pControlPointShader->use();
			pControlPointShader->setUniformValue(controlPointShaderModelHandle, rightControlPointModelMatrix);
			pControlPointShader->setUniformValue(controlPointShaderViewHandle, viewMatrix);
			pPointShader->draw(GL_TRIANGLE_FAN);

			// Draw line
			pControlLineShader->use();
			pControlLineShader->setUniformValue(controlLineShaderModelHandle, rightControlLineModelMatrix);
			pControlLineShader->setUniformValue(controlLineShaderViewHandle, viewMatrix);
			if(controlPointsLinked)
			{
				pControlLineShader->setUniformValue(controlLineShaderColorHandle, TFPOINT_LINKED_CONTROL_POINT_COLOR);
			}
			else
			{
				pControlLineShader->setUniformValue(controlLineShaderColorHandle, TFPOINT_RIGHT_CONTROL_POINT_COLOR);
			}
			pControlLineShader->draw(GL_TRIANGLES);
		}
	}
}

GLboolean TfPoint::intersectWithPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio)
{
	// Forward to general method
	if(intersect(coord, point, calcRealPointSize(scale, aspectRatio)))
	{
		return GL_TRUE;
	}
	else
	{
		return GL_FALSE;
	}
}

GLboolean TfPoint::intersectWithLeftControlPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio)
{
	// Left control point
	if(location != TFPOINT_LOCATION_LEFTEND && intersect(coord, leftControlPoint, calcRealPointSize(scale, aspectRatio)))
	{
		return GL_TRUE;
	}
	else
	{
		return GL_FALSE;
	}
}

GLboolean TfPoint::intersectWithRightControlPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio)
{
	// Right control point
	if(location != TFPOINT_LOCATION_RIGHTEND && intersect(coord, rightControlPoint, calcRealPointSize(scale, aspectRatio)))
	{
		return GL_TRUE;
	}
	else
	{
		return GL_FALSE;
	}
}

GLboolean TfPoint::toggleControlPointsLinked()
{
	if(location == TFPOINT_LOCATION_NORMAL)
	{
		controlPointsLinked = !controlPointsLinked;
		if(controlPointsLinked)
		{
			adjustLinkedControlPoints();
		}
	}
	else
	{
		controlPointsLinked = GL_FALSE;
	}
	return controlPointsLinked;
}

GLint TfPoint::getHandle() const
{
	return handle;
}

TfPointLocation TfPoint::getLocation() const
{
	return location;
}

glm::vec2 TfPoint::getPoint() const
{
	return point;
}

glm::vec2 TfPoint::getLeftControlPoint() const
{
	return leftControlPoint;
}

glm::vec2 TfPoint::getRightControlPoint() const
{
	return rightControlPoint;
}

TfPointValue TfPoint::getValue() const
{
	return value;
}

GLboolean TfPoint::getControlPointsLinked() const
{
	return controlPointsLinked;
}

void TfPoint::setPoint(glm::vec2 point)
{
	this->point = point;
}

void TfPoint::setLeftControlPoint(glm::vec2 leftControlPoint)
{
	this->leftControlPoint = leftControlPoint;

	// Clamp at point
	if(this->leftControlPoint.x > point.x)
	{
		this->leftControlPoint.x = point.x;
	}

	if(controlPointsLinked)
	{
		rightControlPoint = mirrorPoint(this->leftControlPoint);
	}
}

void TfPoint::setRightControlPoint(glm::vec2 rightControlPoint)
{
	this->rightControlPoint = rightControlPoint;

	// Clamp at point
	if(this->rightControlPoint.x < point.x)
	{
		this->rightControlPoint.x = point.x;
	}

	if(controlPointsLinked)
	{
		leftControlPoint = mirrorPoint(this->rightControlPoint);
	}
}

void TfPoint::setValue(TfPointValue value)
{
	this->value = value;
}

void TfPoint::setControlPointsLinked(GLboolean linked)
{
	if(location == TFPOINT_LOCATION_NORMAL)
	{
		controlPointsLinked = linked;
		if(controlPointsLinked)
		{
			adjustLinkedControlPoints();
		}
	}
	else
	{
		controlPointsLinked = GL_FALSE;
	}
}

bool TfPoint::operator<(const TfPoint &rhs) const
{
	return point.x < rhs.getPoint().x;
}

GLboolean TfPoint::intersect(glm::vec2 coord, glm::vec2 point, glm::vec2 size)
{
	// Straight forward intersection test
	GLfloat distX = glm::abs(coord.x - point.x);

	if(distX > (size.x * (1 + TFPOINT_INTERSECTION_BIAS)))
	{
		return GL_FALSE;
	}	
	
	GLfloat distY = glm::abs(coord.y - point.y);
	if(distY > (size.y * (1 + TFPOINT_INTERSECTION_BIAS)))
	{
		return GL_FALSE;
	}

	return GL_TRUE;
}

glm::vec2 TfPoint::calcRealPointSize(GLfloat scale, GLfloat aspectRatio)
{
	return glm::vec2(TFPOINT_POINTS_SCALE*scale/aspectRatio, TFPOINT_POINTS_SCALE*scale);
}

glm::vec2 TfPoint::mirrorPoint(glm::vec2 point)
{
	return (this->point + (this->point - point));
}

void TfPoint::adjustLinkedControlPoints()
{
	// Adjust left control point (which automatically sets right one)
	glm::vec2 delta = leftControlPoint - mirrorPoint(rightControlPoint);
	setLeftControlPoint(leftControlPoint - (delta * 0.5f));
}