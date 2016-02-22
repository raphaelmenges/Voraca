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

#include "Camera.h"

Camera::Camera()
{
	alpha = 0;
	beta = glm::half_pi<GLfloat>();
}

Camera::~Camera()
{
}

void Camera::init(glm::vec3 center, GLfloat alpha, GLfloat beta, GLfloat radius, GLfloat minRadius, GLfloat maxRadius)
{
	this->center = center;
	this->alpha = alpha;
	this->beta = beta;
	this->radius = radius;
	this->minRadius = minRadius;
	this->maxRadius = maxRadius;

	clampValues();
}

void Camera::reset(glm::vec3 center, GLfloat alpha, GLfloat beta, GLfloat radius)
{
	this->center = center;
	this->alpha = alpha;
	this->beta = beta;
	this->radius = radius;
}

void Camera::setAlpha(GLfloat alpha)
{
	this->alpha = alpha;

	clampAlpha();
}

void Camera::setCenter(glm::vec3 center)
{
	this->center = center;
}

void Camera::setBeta(GLfloat beta)
{
	this->beta = beta;

	clampBeta();
}


void Camera::setRadius(GLfloat radius)
{
	this->radius = radius;

	clampRadius();
}

glm::mat4 Camera::getViewMatrix()
{
	calcPosition();
	return glm::lookAt(position, center, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::getPosition()
{
	calcPosition();
	return position;
}

glm::vec3 Camera::getCenter() const
{
	return center;
}

GLfloat Camera::getAlpha() const
{
	return alpha;
}

GLfloat Camera::getBeta() const
{
	return beta;
}

GLfloat Camera::getRadius() const
{
	return radius;
}

void Camera::calcPosition()
{
	position.x = radius * glm::sin(beta) * glm::cos(alpha);
	position.y = radius * glm::cos(beta);
	position.z = radius * glm::sin(beta) * glm::sin(alpha);
	position += center;
}

void Camera::clampValues()
{
	// Horizontal rotation
	clampAlpha();

	// Vertical rotation
	clampBeta();

	// Zoom/Radius
	clampRadius();
}

void Camera::clampAlpha()
{
	alpha = fmodf(alpha, 2*glm::pi<GLfloat>());
	if(alpha < 0)
	{
		alpha = 2*glm::pi<GLfloat>() + alpha;
	}
}

void Camera::clampBeta()
{
	beta = glm::clamp(beta, CAMERA_BETA_BIAS, glm::pi<GLfloat>()-CAMERA_BETA_BIAS);
}

void Camera::clampRadius()
{
	radius = glm::clamp(radius, minRadius, maxRadius);
}