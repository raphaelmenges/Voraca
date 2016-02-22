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

#include "Transferfunction.h"

Transferfunction::Transferfunction()
{
	functionShouldBeUpdated = GL_FALSE;
	preintegrationShouldBeUpdated = GL_FALSE;
	tfPointHandleCounter = 0;
	overwriteProtected = GL_FALSE;
}

Transferfunction::~Transferfunction()
{
	// Only textures have to be deleted here
	deleteTexture(colorAlphaFunctionHandle);
	deleteTexture(ambientSpecularFunctionHandle);
	deleteTexture(advancedFunctionHandle);
	deleteTexture(colorAlphaPreintegrationHandle);
	deleteTexture(ambientSpecularPreintegrationHandle);
	deleteTexture(advancedPreintegrationHandle);
}

void Transferfunction::init(GLint handle, std::string name)
{
	// Simple initializations
	this->handle = handle;
	this->name = name;

	// Create filled circle for points
	std::vector<glm::vec3> pointShaderVertices;
	GLfloat pointShaderVerticesStepSize = (2* glm::pi<GLfloat>()) / TRANSFERFUNCTION_TFPOINT_BORDER_VERTEX_COUNT;
	pointShaderVertices.push_back(glm::vec3(0, 0, 0));
	for(GLuint i = 0; i <= TRANSFERFUNCTION_TFPOINT_BORDER_VERTEX_COUNT; i++)
	{
		GLfloat value = i * pointShaderVerticesStepSize;
		pointShaderVertices.push_back(glm::vec3(glm::cos(value), glm::sin(value), 0));
	
	}

	// *** POINT SHADER ***
	pointShader.loadShaders("Point.vert", "Point.frag");

	// Use filled circle as vertex data
	pointShader.setVertexBuffer(reinterpret_cast<GLfloat*> (&(pointShaderVertices[0])), 3*(int)pointShaderVertices.size()*sizeof(GLfloat), "positionAttribute");

	// Get handles
	pointShaderModelHandle = pointShader.getUniformHandle("uniformModel");
	pointShaderViewHandle = pointShader.getUniformHandle("uniformView");
	pointShaderInnerColorHandle = pointShader.getUniformHandle("uniformInnerColor");
	pointShaderBorderColorHandle = pointShader.getUniformHandle("uniformBorderColor");

	// *** CONTROL POINT SHADER ***
	controlPointShader.loadShaders("ControlPoint.vert", "ControlPoint.frag");

	// Use filled circle as model
	controlPointShader.setVertexBuffer(reinterpret_cast<GLfloat*> (&(pointShaderVertices[0])), 3*(int)pointShaderVertices.size()*sizeof(GLfloat), "positionAttribute");

	// Get handles
	controlPointShaderModelHandle = controlPointShader.getUniformHandle("uniformModel");
	controlPointShaderViewHandle = controlPointShader.getUniformHandle("uniformView");

	// *** LINES BETWEEN POINT AND CONTROL POINT ***
	controlLineShader.loadShaders("ControlLine.vert", "ControlLine.frag");

	// Use quad as vertex data
	controlLineShader.setVertexBuffer(primitives::quad, sizeof(primitives::quad), "positionAttribute");
	
	// Get handles
	controlLineShaderModelHandle = controlLineShader.getUniformHandle("uniformModel");
	controlLineShaderViewHandle = controlLineShader.getUniformHandle("uniformView");
	controlLineShaderColorHandle = controlLineShader.getUniformHandle("uniformColor");

	// *** FUNCTION SHADER ***
	functionShader.loadShaders("Function.vert", "Function.frag");

	// Set vertex data
	std::vector<glm::vec3> functionShaderVertices;
	GLfloat functionShaderVerticesStepSize = 1.0f / TRANSFERFUNCTION_VISUALIZATION_STEPS;
	for(GLuint i = 1; i <= TRANSFERFUNCTION_VISUALIZATION_STEPS; i++)
	{
		GLfloat lastX = (i-1) * functionShaderVerticesStepSize;
		GLfloat currentX = lastX + functionShaderVerticesStepSize;

		glm::vec3 a = glm::vec3(currentX + TRANSFERFUNCTION_VISUALIZATION_X_OVERLAPPING, 1.f, 0.f);
		glm::vec3 b = glm::vec3(lastX - TRANSFERFUNCTION_VISUALIZATION_X_OVERLAPPING, 1.f, 0.f);
		glm::vec3 c = glm::vec3(lastX - TRANSFERFUNCTION_VISUALIZATION_X_OVERLAPPING, 0.f, 0.f);
		glm::vec3 d = glm::vec3(currentX + TRANSFERFUNCTION_VISUALIZATION_X_OVERLAPPING, 0.f, 0.f);
		functionShaderVertices.push_back(a);
		functionShaderVertices.push_back(b);
		functionShaderVertices.push_back(c);
		functionShaderVertices.push_back(c);
		functionShaderVertices.push_back(d);
		functionShaderVertices.push_back(a);
	}
	functionShader.setVertexBuffer(reinterpret_cast<GLfloat*> (&(functionShaderVertices[0])), 3*(int)functionShaderVertices.size()*sizeof(GLfloat), "positionAttribute");

	// Get handles
	functionShaderModelHandle = functionShader.getUniformHandle("uniformModel");
	functionShaderViewHandle = functionShader.getUniformHandle("uniformView");
	functionShaderColorAlphaFunctionHandle = functionShader.getUniformHandle("uniformColorAlpha");
	functionShaderAmbientSpecularFunctionHandle = functionShader.getUniformHandle("uniformAmbientSpecular");
	functionShaderAdvancedFunctionHandle = functionShader.getUniformHandle("uniformAdvanced");
	functionShaderVisualizationStateHandle = functionShader.getUniformHandle("uniformVisualizationState");
	functionShaderOpacityHandle = functionShader.getUniformHandle("uniformOpacity");
	functionShaderValueRangeHandle = functionShader.getUniformHandle("uniformValueRange");

	// Texture initializations
	textureInitialization(colorAlphaFunctionHandle, GL_TEXTURE_1D);
	textureInitialization(ambientSpecularFunctionHandle, GL_TEXTURE_1D);
	textureInitialization(advancedFunctionHandle, GL_TEXTURE_1D);
	textureInitialization(colorAlphaPreintegrationHandle, GL_TEXTURE_2D);
	textureInitialization(ambientSpecularPreintegrationHandle, GL_TEXTURE_2D);
	textureInitialization(advancedPreintegrationHandle, GL_TEXTURE_2D);

	functionShouldBeUpdated = GL_TRUE;
	preintegrationShouldBeUpdated = GL_TRUE;
}

void Transferfunction::draw(TfVisualization visualization, GLfloat functionOpacity, GLint activeTfPoint, std::set<GLint> selectedTfPoints, GLboolean locked, GLfloat scale, GLfloat aspectRatio, glm::mat4 viewMatrix)
{
	// Update function and preintegration
	if(functionShouldBeUpdated)
	{
		updateFunction();
	}
	if(preintegrationShouldBeUpdated)
	{
		updatePreintegation();
	}

	// Get range by values for function
	glm::vec2 valueRange;
	if(visualization == TF_VISUALIZATION_COLOR)
	{
		valueRange.x = 0;
		valueRange.y = 1;
	}
	else if(visualization == TF_VISUALIZATION_AMBIENT_MULTIPLIER)
	{
		valueRange.x = TFPOINTVALUE_AMBIENT_MULTIPLIER_MIN;
		valueRange.y = TFPOINTVALUE_AMBIENT_MULTIPLIER_MAX;
	}
	else if(visualization == TF_VISUALIZATION_SPECULAR_MULTIPLIER)
	{
		valueRange.x = TFPOINTVALUE_SPECULAR_MULTIPLIER_MIN;
		valueRange.y = TFPOINTVALUE_SPECULAR_MULTIPLIER_MAX;
	}
	else if(visualization == TF_VISUALIZATION_SPECULAR_SATURATION)
	{
		valueRange.x = TFPOINTVALUE_SPECULAR_SATURATION_MIN;
		valueRange.y = TFPOINTVALUE_SPECULAR_SATURATION_MAX;
	}
	else if(visualization == TF_VISUALIZATION_SPECULAR_POWER)
	{
		valueRange.x = TFPOINTVALUE_SPECULAR_POWER_MIN;
		valueRange.y = TFPOINTVALUE_SPECULAR_POWER_MAX;
	}
	else if(visualization == TF_VISUALIZATION_GRADIENT_ALPHA_MULTIPLIER)
	{
		valueRange.x = TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER_MIN;
		valueRange.y = TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER_MAX;
	}
	else if(visualization == TF_VISUALIZATION_FRESNEL_ALPHA_MULTIPLIER)
	{
		valueRange.x = TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER_MIN;
		valueRange.y = TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER_MAX;
	}
	else if(visualization == TF_VISUALIZATION_RELECTION_COLOR_MULTIPLIER)
	{
		valueRange.x = TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER_MIN;
		valueRange.y = TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER_MAX;
	}
	else 
	{
		valueRange.x = TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER_MIN;
		valueRange.y = TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER_MAX;
	}

	// Draw function
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	functionShader.use();
	functionShader.setUniformValue(functionShaderModelHandle, glm::translate(glm::mat4(1.0f), glm::vec3(0,0,TRANSFERFUNCTION_VISUALIZATION_POS_Z)));
	functionShader.setUniformValue(functionShaderViewHandle, viewMatrix);
	functionShader.setUniformTexture(functionShaderColorAlphaFunctionHandle, colorAlphaFunctionHandle, GL_TEXTURE_1D);
	functionShader.setUniformTexture(functionShaderAmbientSpecularFunctionHandle, ambientSpecularFunctionHandle, GL_TEXTURE_1D);
	functionShader.setUniformTexture(functionShaderAdvancedFunctionHandle,advancedFunctionHandle, GL_TEXTURE_1D);
	functionShader.setUniformValue(functionShaderVisualizationStateHandle, visualization);
	functionShader.setUniformValue(functionShaderOpacityHandle, functionOpacity);
	functionShader.setUniformValue(functionShaderValueRangeHandle, valueRange);
	functionShader.draw(GL_TRIANGLES);
	glDisable(GL_BLEND); 

	// Draw all tfPoints
	for(std::vector<TfPoint>::iterator it = tfPoints.begin(); it != tfPoints.end(); ++it)
	{
		// Right color for visualization
		TfPointValue value = it->getValue();
		glm::vec3 color;
		
		if(visualization == TF_VISUALIZATION_COLOR)
		{
			color = value.color;
		}
		else if(visualization == TF_VISUALIZATION_AMBIENT_MULTIPLIER)
		{
			color = glm::vec3(value.ambientMultiplier);
		}
		else if(visualization == TF_VISUALIZATION_SPECULAR_MULTIPLIER)
		{
			color = glm::vec3(value.specularMultiplier);
		}
		else if(visualization == TF_VISUALIZATION_SPECULAR_SATURATION)
		{
			color = glm::vec3(value.specularSaturation);
		}
		else if(visualization == TF_VISUALIZATION_SPECULAR_POWER)
		{
			color = glm::vec3(value.specularPower);
		}
		else if(visualization == TF_VISUALIZATION_GRADIENT_ALPHA_MULTIPLIER)
		{
			color = glm::vec3(value.gradientAlphaMultiplier);
		}
		else if(visualization == TF_VISUALIZATION_FRESNEL_ALPHA_MULTIPLIER)
		{
			color = glm::vec3(value.fresnelAlphaMultiplier);
		}
		else if(visualization == TF_VISUALIZATION_RELECTION_COLOR_MULTIPLIER)
		{
			color = glm::vec3(value.reflectionColorMultiplier);
		}
		else 
		{
			color = glm::vec3(value.emissionColorMultiplier);
		}

		// Adjust range of color of tfPoint
		color = (color - valueRange.x) / (valueRange.y - valueRange.x);

		// Is tfPoint selected?
		if(std::find(selectedTfPoints.begin(), selectedTfPoints.end(), it->getHandle())!=selectedTfPoints.end())
		{
			// Is it even active?
			if(activeTfPoint == it->getHandle())
			{
				it->draw(TFPOINT_STATE_ACTIVE, color, scale, aspectRatio, !locked, viewMatrix);
			}
			else
			{
				it->draw(TFPOINT_STATE_SELECTED, color, scale, aspectRatio, GL_FALSE, viewMatrix);
			}
		}
		else
		{
			it->draw(TFPOINT_STATE_UNSELECTED, color, scale, aspectRatio, GL_FALSE, viewMatrix);
		}
	}
}

GLint Transferfunction::selectTfPoint(glm::vec2 coord, GLfloat scale, GLfloat aspectRatio, glm::vec2 &offset)
{
	GLint handle = -1;

	// Check for all tfPoints
	for(std::vector<TfPoint*>::size_type i = 0; i != tfPoints.size(); i++)
	{
		if(tfPoints[i].intersectWithPoint(coord, scale, aspectRatio))
		{
			handle = tfPoints[i].getHandle();
			offset = coord - tfPoints[i].getPoint();
		}
	}

	return handle;
}

GLboolean Transferfunction::intersectLeftControlPoint(GLint handle, glm::vec2 coord, GLfloat scale, GLfloat aspectRatio, glm::vec2 &offset)
{
	TfPoint* pTfPoint =  getTfPointByHandle(handle);
	offset = coord - pTfPoint->getLeftControlPoint();
	return pTfPoint->intersectWithLeftControlPoint(coord, scale, aspectRatio);
}

GLboolean Transferfunction::intersectRightControlPoint(GLint handle, glm::vec2 coord, GLfloat scale, GLfloat aspectRatio, glm::vec2 &offset)
{
	TfPoint* pTfPoint =  getTfPointByHandle(handle);
	offset = coord - pTfPoint->getRightControlPoint();
	return getTfPointByHandle(handle)->intersectWithRightControlPoint(coord, scale, aspectRatio);
}

GLboolean Transferfunction::insideBorders(glm::vec2 coord)
{
	// Test given coordinates to be within the borders
	if(coord.x <= TRANSFERFUNCTION_X_MAX 
		&& coord.x >= TRANSFERFUNCTION_X_MIN
		&& coord.y <= TRANSFERFUNCTION_Y_MAX
		&& coord.y >= TRANSFERFUNCTION_Y_MIN)
	{
		return GL_TRUE;
	}
	else
	{
		return GL_FALSE;
	}
}

GLboolean Transferfunction::setTfPoint(GLint handle, glm::vec2 coord)
{
	TfPoint* pTfPoint = getTfPointByHandle(handle);

	GLboolean clamped = GL_FALSE;

	// Save old position
	glm::vec2 oldPoint = pTfPoint->getPoint();

	// Move tfPoint
	pTfPoint->setPoint(coord);

	// Clamp it
	clamped = clampTfPointPosition(pTfPoint);
	
	// Move control points
	glm::vec2 offset = pTfPoint->getPoint() - oldPoint;
	pTfPoint->setLeftControlPoint(pTfPoint->getLeftControlPoint() + offset);
	pTfPoint->setRightControlPoint(pTfPoint->getRightControlPoint() + offset);

	// Clamp them if allowed
	if(TRANSFERFUNCTION_CLAMP_CONTROL_POINTS)
	{
		clampRightControlPointPosition(pTfPoint);
		clampLeftControlPointPosition(pTfPoint);
	}

	// Sort tfPoints
	reorderTfPoints();
	
	// Update function
	functionShouldBeUpdated = GL_TRUE;

	// Return whether tfPoint was clamped
	return clamped;
}

GLboolean Transferfunction::moveTfPoint(GLint handle, glm::vec2 delta)
{
	TfPoint* pTfPoint = getTfPointByHandle(handle);

	// Forward to setter
	return setTfPoint(handle, pTfPoint->getPoint() + delta);
}

void Transferfunction::setLeftControlPoint(GLint handle, glm::vec2 coord)
{
	TfPoint* pTfPoint = getTfPointByHandle(handle);

	// Move left control point
	pTfPoint->setLeftControlPoint(coord);

	// Clamp it
	if(TRANSFERFUNCTION_CLAMP_CONTROL_POINTS)
	{
		clampLeftControlPointPosition(pTfPoint);
	}

	// Update function
	functionShouldBeUpdated = GL_TRUE;
}

void Transferfunction::setRightControlPoint(GLint handle, glm::vec2 coord)
{
	TfPoint* pTfPoint = getTfPointByHandle(handle);

	// Move right control point
	pTfPoint->setRightControlPoint(coord);

	// Clamp it
	if(TRANSFERFUNCTION_CLAMP_CONTROL_POINTS)
	{
		clampRightControlPointPosition(pTfPoint);
	}

	// Update function
	functionShouldBeUpdated = GL_TRUE;
}

GLint Transferfunction::addTfPoint(glm::vec2 coord)
{
	GLint handle = tfPointHandleCounter;

	// Handle counter is incremented by internal add method
	TfPoint* pTfPoint = internalAddTfPoint(TFPOINT_LOCATION_NORMAL, coord);

	// Better be cautious
	clampTfPointPosition(pTfPoint);

	// Get values out of existing vectors
	TfPointValue value = pTfPoint->getValue();

	glm::vec4 colorAlpha = colorAlphaFunction[static_cast<GLint>(coord.x * colorAlphaFunction.size())];
	value.color = glm::vec3(colorAlpha.r, colorAlpha.g, colorAlpha.b);

	glm::vec4 ambientSpecular = ambientSpecularFunction[static_cast<GLint>(coord.x * ambientSpecularFunction.size())];
	value.ambientMultiplier = ambientSpecular.r;
	value.specularMultiplier = ambientSpecular.g;
	value.specularSaturation = ambientSpecular.b;
	value.specularPower = ambientSpecular.a;

	glm::vec4 advanced = advancedFunction[static_cast<GLint>(coord.x * advancedFunction.size())];
	value.gradientAlphaMultiplier = advanced.r;
	value.fresnelAlphaMultiplier = advanced.g;
	value.reflectionColorMultiplier = advanced.b;
	value.emissionColorMultiplier = advanced.a;

	pTfPoint->setValue(value),

	// Sort tfPoints
	reorderTfPoints();

	// Update function
	functionShouldBeUpdated = GL_TRUE;

	return handle;
}

GLboolean Transferfunction::deleteTfPoint(GLint handle)
{
	GLboolean success = GL_FALSE;
	GLint index = -1;

	// Get index in vector
	for(GLint i = 0; i != tfPoints.size(); i++)
	{
		if(tfPoints[i].getHandle() == handle)
		{
			index = i;
			break;
		}
	}

	// Delete it if tfPoint with this handle exists
	if(index >=0)
	{
		TfPointLocation location = tfPoints[index].getLocation();

		// Only delete when it is a normal tfPoint
		if(location == TFPOINT_LOCATION_NORMAL)
		{
			// Erase it from vector
			tfPoints.erase(tfPoints.begin() + index);
			success = GL_TRUE;	

			// Sort tfPoints
			reorderTfPoints();

			// Update function
			functionShouldBeUpdated = GL_TRUE;
		}
	}

	return success;
}

GLint Transferfunction::duplicateTfPoint(GLint handle)
{
	GLint handleDest = tfPointHandleCounter;
	
	// Create duplicate
	TfPoint* pTfPointDest = internalAddTfPoint(TFPOINT_LOCATION_NORMAL, glm::vec2(0,0));

	// Get pointer after creation of duplicate because address changes 
	TfPoint* pTfPointSrc = getTfPointByHandle(handle);

	// Fill duplicate (direct using of TfPoint methods allowed because values should be ok)
	pTfPointDest->setPoint(pTfPointSrc->getPoint());
	pTfPointDest->setLeftControlPoint(pTfPointSrc->getLeftControlPoint());
	pTfPointDest->setRightControlPoint(pTfPointSrc->getRightControlPoint());
	pTfPointDest->setValue(pTfPointSrc->getValue());

	// Move already existing tfPoint
	moveTfPoint(handle, glm::vec2(TRANSFERFUNCTION_TFPOINT_DUPLICATE_OFFSET, 0));

	// Sorting and updating functions is already done by moveTfPoint()

	return handleDest;
}

TfPointValue Transferfunction::getValueOfTfPoint(GLint handle)
{
	return getTfPointByHandle(handle)->getValue();
}

void Transferfunction::setValueOfTfPoint(GLint handle, TfPointValue value)
{
	getTfPointByHandle(handle)->setValue(value);
	functionShouldBeUpdated = GL_TRUE;
}

GLuint Transferfunction::getColorAlphaFunctionHandle() const
{
	return colorAlphaFunctionHandle;
}

GLuint Transferfunction::getAmbientSpecularFunctionHandle() const
{
	return ambientSpecularFunctionHandle;
}

GLuint Transferfunction::getAdvancedFunctionHandle() const
{
	return advancedFunctionHandle;
}

GLuint Transferfunction::getColorAlphaPreintegrationHandle() const
{
	return colorAlphaPreintegrationHandle;
}

GLuint Transferfunction::getAmbientSpecularPreintegrationHandle() const
{
	return ambientSpecularPreintegrationHandle;
}

GLuint Transferfunction::getAdvancedPreintegrationHandle() const
{
	return advancedPreintegrationHandle;
}

GLuint Transferfunction::getTextureResolution() const
{
	return TRANSFERFUNCTION_TEXTURES_RES;
}

glm::vec2 Transferfunction::getPositionOfTfPoint(GLint handle)
{
	return getTfPointByHandle(handle)->getPoint();
}

GLboolean Transferfunction::toggleTfPointControlPointsLinked(GLint handle)
{
	TfPoint* pTfPoint = getTfPointByHandle(handle);
	if(pTfPoint != NULL)
	{
		GLboolean toggled = pTfPoint->toggleControlPointsLinked();
		if(toggled)
		{
			functionShouldBeUpdated = GL_TRUE;
			preintegrationShouldBeUpdated = GL_TRUE;
		}
		return toggled;
	}
	else
	{
		return GL_FALSE;
	}
}

void Transferfunction::preintegrationShouldBeUpdate()
{
	preintegrationShouldBeUpdated = GL_TRUE;
}

GLint Transferfunction::getHandle() const
{
	return handle;
}

std::set<GLint> Transferfunction::getRowOfHandles(GLint startHandle, GLint endHandle)
{
	TfPoint* pTfPointStart = getTfPointByHandle(startHandle);
	TfPoint* pTfPointEnd = getTfPointByHandle(endHandle);

	std::set<GLint> result;

	if(pTfPointStart != NULL && pTfPointEnd != NULL)
	{
		TfPoint* pCurrentTfPoint;
		pCurrentTfPoint = pTfPointStart;

		// Check which direction
		if(pTfPointStart < pTfPointEnd)
		{
			// left to right
			while(pCurrentTfPoint != pTfPointEnd)
			{
				result.insert(pCurrentTfPoint->getHandle());
				pCurrentTfPoint++;
			}
		}
		else
		{
			// right to left
			while(pCurrentTfPoint != pTfPointEnd)
			{
				result.insert(pCurrentTfPoint->getHandle());
				pCurrentTfPoint--;
			}
		}

		// Insert last handle
		result.insert(pTfPointEnd->getHandle());
	}

	return result;
}

std::string Transferfunction::getName() const
{
	return name;
}

void Transferfunction::rename(std::string name)
{
	this->name = name;
}

TfPoint* Transferfunction::internalAddTfPoint(TfPointLocation location, glm::vec2 coord)
{
	// Create
	TfPoint tfPoint;
	tfPoint.init(
		tfPointHandleCounter,
		location, 
		coord, 
		&pointShader, 
		&controlPointShader,
		&controlLineShader, 
		pointShaderModelHandle,
		pointShaderViewHandle,
		pointShaderInnerColorHandle,
		pointShaderBorderColorHandle,
		controlPointShaderModelHandle,
		controlPointShaderViewHandle,
		controlLineShaderModelHandle,
		controlLineShaderViewHandle,
		controlLineShaderColorHandle);

	// Increment handle counter
	tfPointHandleCounter++;

	// Add it to vector
	tfPoints.push_back(tfPoint);

	return &tfPoints[tfPoints.size()-1];
}

void Transferfunction::updateFunction()
{
	GLuint tfPointCount = (GLuint)tfPoints.size();

	// Some variable reservations
	TfPoint* pLeft;
	TfPoint* pRight;
	glm::vec2 leftPointPos;
	glm::vec2 leftControlPointPos;
	glm::vec2 rightControlPointPos;
	glm::vec2 rightPointPos;
	GLfloat leftIndex;
	GLfloat rightIndex;
	GLfloat t1;
	GLfloat t2;		
	GLfloat a;
	GLfloat b;
	GLfloat c;
	GLfloat d;
	GLuint bezierSteps;
	GLuint steps;

	// Calulate steps between tfPoint heuristically (at index 0 is step count between tfPoint 0 and 1)
	std::vector<GLint> stepsBetweenTfPoints(tfPointCount-1);
	bezierSteps = 0;

	for(GLuint i = 0; i < (tfPointCount-1); i++)
	{
		steps = static_cast<GLint>((tfPoints[i+1].getPoint().x - tfPoints[i].getPoint().x) * TRANSFERFUNCTION_BEZIER_STEPS);
		steps = glm::max(steps,TRANSFERFUNCTION_BEZIER_MIN_STEPS_BETWEEN_TFPOINTS);
		stepsBetweenTfPoints[i] = steps;
		bezierSteps += steps;
	}

	// Create bezier curve

	// Info in vec3: x,y, tfPoint index (1.5 is half between tfPoint 1 and tfPoint 2) 
	std::vector<glm::vec3> bezierCurve(bezierSteps);
	bezierSteps = 0;

	for(GLuint i = 0; i < (tfPointCount-1); i++)
	{
		pLeft = &tfPoints[i];
		pRight = &tfPoints[i+1];

		// Get their positions
		leftPointPos = pLeft->getPoint();
		leftControlPointPos = pLeft->getRightControlPoint();
		rightControlPointPos = pRight->getLeftControlPoint();
		rightPointPos = pRight->getPoint();
		leftIndex = static_cast<GLfloat>(i);
		rightIndex = static_cast<GLfloat>(i+1);

		// Steps between two tfPoints
		steps = stepsBetweenTfPoints[i];

		// Per tfPoint, do bezier interpolation
		for(GLuint j = 0; j < steps; j++)
		{
			t1 = static_cast<GLfloat>(j)/(steps-1);
			t2 = 1-t1;
			
			a = t2*t2*t2;
			b = 3*t1*t2*t2;
			c = 3*t1*t1*t2;
			d = t1*t1*t1;

			bezierCurve[bezierSteps + j].x = a *leftPointPos.x + b * leftControlPointPos.x + c * rightControlPointPos.x + d * rightPointPos.x;
			bezierCurve[bezierSteps + j].y = a *leftPointPos.y + b * leftControlPointPos.y + c * rightControlPointPos.y + d * rightPointPos.y;
			bezierCurve[bezierSteps + j].z = a *leftIndex + b * leftIndex + c * rightIndex + d * rightIndex;
		}

		bezierSteps += steps;
	}

	
	// Sample bezier curve and get function values
	colorAlphaFunction.resize(TRANSFERFUNCTION_TEXTURES_RES);
	ambientSpecularFunction.resize(TRANSFERFUNCTION_TEXTURES_RES);
	advancedFunction.resize(TRANSFERFUNCTION_TEXTURES_RES);

	GLfloat stepSize = 1.0f/TRANSFERFUNCTION_TEXTURES_RES;
	GLint positionInBezier = 0;
	GLfloat positionInTexture = 0;
	TfPointValue leftValue;
	TfPointValue rightValue;
	GLfloat indexLeft, indexRight;
	GLuint index;

	// Iterate over texutre resolution
	for(GLuint i = 0; i < TRANSFERFUNCTION_TEXTURES_RES; i++)
	{
		// Search position in bezier curve
		while((bezierCurve[positionInBezier].x) <= positionInTexture)
		{
			positionInBezier++;
		}

		// Get left and right tfPoint
		indexLeft = bezierCurve[positionInBezier-1].z;
		indexRight = bezierCurve[positionInBezier].z;
		index = static_cast<GLuint>((indexLeft+indexRight)/2.0f);

		pLeft = &tfPoints[index];
		pRight = &tfPoints[index+1];

		// Calculate interpolation between them (could be even more precise...)
		t1 = ((indexLeft+indexRight)/2.0f) - static_cast<GLfloat>(index);
		t2 = 1-t1;

		// Get values
		leftValue = pLeft->getValue();
		rightValue = pRight->getValue();

		// *** FILL VARIABLES ***

		// Alpha (1D-Bezier)
		GLfloat alpha = (bezierCurve[positionInBezier-1].y + bezierCurve[positionInBezier].y)/2.0f;
		alpha = glm::clamp(alpha, 0.0f, 1.0f);

		// Color (Linear)
		glm::vec3 color = (t2) * leftValue.color + (t1) * rightValue.color;

		// Ambient multiplier
		GLfloat ambientMultiplier = (t2) * leftValue.ambientMultiplier + (t1) * rightValue.ambientMultiplier;

		// Specular multiplier (Linear)
		GLfloat specularMultiplier = (t2) * leftValue.specularMultiplier + (t1) * rightValue.specularMultiplier;

		// Specular saturation (Linear)
		GLfloat specularSaturation = (t2) * leftValue.specularSaturation + (t1) * rightValue.specularSaturation;

		// Specular power (Linear)
		GLfloat specularPower = (t2) * leftValue.specularPower + (t1) * rightValue.specularPower;

		// Gradient alpha multiplier (Linear)
		GLfloat gradientAlphaMultiplier = (t2) * leftValue.gradientAlphaMultiplier + (t1) * rightValue.gradientAlphaMultiplier;

		// Fresnel alpha multiplier (Linear)
		GLfloat fresnelAlphaMultiplier = (t2) * leftValue.fresnelAlphaMultiplier + (t1) * rightValue.fresnelAlphaMultiplier;

		// Reflection color multiplier (Linear)
		GLfloat reflectionColorMultiplier = (t2) * leftValue.reflectionColorMultiplier + (t1) * rightValue.reflectionColorMultiplier;

		// Emission color multiplier (Linear)
		GLfloat emissionColorMultiplier = (t2) * leftValue.emissionColorMultiplier + (t1) * rightValue.emissionColorMultiplier;

		// Put color and alpha in one vector
		colorAlphaFunction[i] = glm::vec4(color, alpha);

		// Put ambient and specular stuff in one vector
		ambientSpecularFunction[i] = glm::vec4(ambientMultiplier, specularMultiplier, specularSaturation, specularPower);

		// Put advanced stuff in one vector
		advancedFunction[i] = glm::vec4(gradientAlphaMultiplier, fresnelAlphaMultiplier, reflectionColorMultiplier, emissionColorMultiplier);

		// Prepare for next iteration
		positionInTexture += stepSize;
	}

	// *** Update textures ***

	// Color and alpha
	glBindTexture(GL_TEXTURE_1D, colorAlphaFunctionHandle);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, TRANSFERFUNCTION_TEXTURES_RES, 0, GL_RGBA, GL_FLOAT, reinterpret_cast<GLfloat*> (&(colorAlphaFunction[0])));
	glBindTexture(GL_TEXTURE_1D, 0);

	// Ambient and specular stuff
	glBindTexture(GL_TEXTURE_1D, ambientSpecularFunctionHandle);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, TRANSFERFUNCTION_TEXTURES_RES, 0, GL_RGBA, GL_FLOAT, reinterpret_cast<GLfloat*> (&(ambientSpecularFunction[0])));
	glBindTexture(GL_TEXTURE_1D, 0);

	// Advanced
	glBindTexture(GL_TEXTURE_1D, advancedFunctionHandle);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, TRANSFERFUNCTION_TEXTURES_RES, 0, GL_RGBA, GL_FLOAT, reinterpret_cast<GLfloat*> (&(advancedFunction[0])));
	glBindTexture(GL_TEXTURE_1D, 0);

	functionShouldBeUpdated = GL_FALSE;
}

void Transferfunction::updatePreintegation()
{
	updatePreintegrationHelper(colorAlphaFunction, colorAlphaPreintegrationHandle);
	updatePreintegrationHelper(ambientSpecularFunction, ambientSpecularPreintegrationHandle);
	updatePreintegrationHelper(advancedFunction, advancedPreintegrationHandle);

	preintegrationShouldBeUpdated = GL_FALSE;
}

void Transferfunction::updatePreintegrationHelper(std::vector<glm::vec4>& function, GLuint& textureHandle)
{
	// Some preparation
	std::vector<glm::vec4> preintegration(TRANSFERFUNCTION_TEXTURES_RES);

	// First element can be simply read out
	preintegration[0] = function[0];

	// Now calculate other values
	for(GLuint i = 1; i < TRANSFERFUNCTION_TEXTURES_RES; i++)
	{
		preintegration[i] = preintegration[i-1] + function[i];
	}

	// Create 2D texture of preintegration
	std::vector<glm::vec4> textureData(TRANSFERFUNCTION_TEXTURES_RES * TRANSFERFUNCTION_TEXTURES_RES);
	glm::vec4 tmp;

	for(GLuint x = 0; x < TRANSFERFUNCTION_TEXTURES_RES; x++)
	{
		for(GLuint y = 0; y < TRANSFERFUNCTION_TEXTURES_RES; y++)
		{
			if(x > y)
			{
				tmp = preintegration[x] - preintegration[y];

				// Normalize
				tmp *= (1.0f / static_cast<GLfloat>(x-y));

				// Store it in texture
				textureData[x + TRANSFERFUNCTION_TEXTURES_RES*y] = tmp;
			}
			if(x < y)
			{
				tmp = preintegration[y] - preintegration[x];

				// Normalize
				tmp *= (1.0f / static_cast<GLfloat>(y-x));

				// Store it in texture
				textureData[x + TRANSFERFUNCTION_TEXTURES_RES*y] = tmp;
			}
			else
			{
				textureData[x + TRANSFERFUNCTION_TEXTURES_RES*y] = function[x];
			}
		}
	}

	// Fill texture
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TRANSFERFUNCTION_TEXTURES_RES, TRANSFERFUNCTION_TEXTURES_RES, 0, GL_RGBA, GL_FLOAT, reinterpret_cast<GLfloat*> (&(textureData[0])));
	glBindTexture(GL_TEXTURE_2D, 0);
}

TfPoint* Transferfunction::getTfPointByHandle(GLint handle)
{
	if(handle < 0 || handle >= tfPointHandleCounter)
	{
		LogError("Tried to get a non-existing point");
	}

	TfPoint* pTfPoint = NULL;

	// Search for tfPoint with that handle
	for(std::vector<TfPoint>::iterator it = tfPoints.begin(); it != tfPoints.end(); ++it)
	{
		if(it->getHandle() == handle)
		{
			pTfPoint = &(*it);
			break;
		}
	}

	return pTfPoint;
}

void Transferfunction::reorderTfPoints()
{
	// Sorting the vector
	std::sort(tfPoints.begin(), tfPoints.end());
}

void Transferfunction::textureInitialization(GLuint& textureHandle, GLenum mode)
{
	// Some OpenGL stuff
	glGenTextures(1, &textureHandle);
	glBindTexture(mode, textureHandle);
	glTexParameteri(mode, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(mode, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	if(TRANSFERFUNCTION_TEXTURES_LINEAR_FILTERING)
	{
		glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	glBindTexture(mode, 0);
}

GLboolean Transferfunction::clampTfPointPosition(TfPoint* pTfPoint)
{
	TfPointLocation location = pTfPoint->getLocation();

	GLboolean clamped = GL_FALSE;

	if(location == TFPOINT_LOCATION_LEFTEND)
	{
		pTfPoint->setPoint(glm::vec2(0, pTfPoint->getPoint().y));
	}

	if(location == TFPOINT_LOCATION_RIGHTEND)
	{
		pTfPoint->setPoint(glm::vec2(1, pTfPoint->getPoint().y));
	}

	if(pTfPoint->getPoint().x < TRANSFERFUNCTION_X_MIN)
	{
		pTfPoint->setPoint(glm::vec2(TRANSFERFUNCTION_X_MIN, pTfPoint->getPoint().y));
		clamped = GL_TRUE;
	}
	else if(pTfPoint->getPoint().x > TRANSFERFUNCTION_X_MAX)
	{
		pTfPoint->setPoint(glm::vec2(TRANSFERFUNCTION_X_MAX, pTfPoint->getPoint().y));
		clamped = GL_TRUE;
	}

	if(pTfPoint->getPoint().y < TRANSFERFUNCTION_Y_MIN)
	{
		pTfPoint->setPoint(glm::vec2(pTfPoint->getPoint().x, TRANSFERFUNCTION_Y_MIN));
		clamped = GL_TRUE;
	}
	else if(pTfPoint->getPoint().y > TRANSFERFUNCTION_Y_MAX)
	{
		pTfPoint->setPoint(glm::vec2(pTfPoint->getPoint().x, TRANSFERFUNCTION_Y_MAX));
		clamped = GL_TRUE;
	}

	return clamped;
}

GLboolean Transferfunction::clampLeftControlPointPosition(TfPoint* pTfPoint)
{
	GLboolean clamped = GL_FALSE;

	if(pTfPoint->getLeftControlPoint().x < TRANSFERFUNCTION_X_MIN)
	{
		pTfPoint->setLeftControlPoint(glm::vec2(TRANSFERFUNCTION_X_MIN, pTfPoint->getLeftControlPoint().y));
		clamped = GL_TRUE;
	}
	else if(pTfPoint->getLeftControlPoint().x > TRANSFERFUNCTION_X_MAX)
	{
		pTfPoint->setLeftControlPoint(glm::vec2(TRANSFERFUNCTION_X_MAX, pTfPoint->getLeftControlPoint().y));
		clamped = GL_TRUE;
	}

	if(pTfPoint->getLeftControlPoint().y < TRANSFERFUNCTION_Y_MIN)
	{
		pTfPoint->setLeftControlPoint(glm::vec2(pTfPoint->getLeftControlPoint().x, TRANSFERFUNCTION_Y_MIN));
		clamped = GL_TRUE;
	}
	else if(pTfPoint->getLeftControlPoint().y > TRANSFERFUNCTION_Y_MAX)
	{
		pTfPoint->setLeftControlPoint(glm::vec2(pTfPoint->getLeftControlPoint().x, TRANSFERFUNCTION_Y_MAX));
		clamped = GL_TRUE;
	}

	return clamped;
}

GLboolean Transferfunction::clampRightControlPointPosition(TfPoint* pTfPoint)
{
	GLboolean clamped = GL_FALSE;

	if(pTfPoint->getRightControlPoint().x < TRANSFERFUNCTION_X_MIN)
	{
		pTfPoint->setRightControlPoint(glm::vec2(TRANSFERFUNCTION_X_MIN, pTfPoint->getRightControlPoint().y));
		clamped = GL_TRUE;
	}
	else if(pTfPoint->getRightControlPoint().x > TRANSFERFUNCTION_X_MAX)
	{
		pTfPoint->setRightControlPoint(glm::vec2(TRANSFERFUNCTION_X_MAX, pTfPoint->getRightControlPoint().y));
		clamped = GL_TRUE;
	}

	if(pTfPoint->getRightControlPoint().y < TRANSFERFUNCTION_Y_MIN)
	{
		pTfPoint->setRightControlPoint(glm::vec2(pTfPoint->getRightControlPoint().x, TRANSFERFUNCTION_Y_MIN));
		clamped = GL_TRUE;
	}
	else if(pTfPoint->getRightControlPoint().y > TRANSFERFUNCTION_Y_MAX)
	{
		pTfPoint->setRightControlPoint(glm::vec2(pTfPoint->getRightControlPoint().x, TRANSFERFUNCTION_Y_MAX));
		clamped = GL_TRUE;
	}

	return clamped;
}

void Transferfunction::deleteTexture(GLuint textureHandle)
{
	glDeleteTextures(1, &textureHandle);
}