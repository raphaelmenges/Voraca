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

#include "Viewport.h"

Viewport::Viewport()
{
}

Viewport::~Viewport()
{
}

void Viewport::init(GLint handle, std::string title, glm::vec3 color, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive)
{	
	// Simple member initializations
	this->handle = handle;
	this->title = title;
	this->color = color;
	this->relativePosX = posX;
	this->relativePosY = posY;
	this->relativeWidth = width;
	this->relativeHeight = height;
	this->barActive = barActive;

	// Bar initialization (before resizing!)
	std::string barTitle = UT::to_string(this->handle) + " - " + this->title;
	pBar = TwNewBar(barTitle.c_str());

	// Type is set by constructor
	bar_type = type;

	// Viewport resizing and bar positioning
	viewportResize(windowWidth, windowHeight);

	// Set bar color
	GLint barColorArray[] = {static_cast<GLint>(color.r * 255), static_cast<GLint>(color.g * 255), static_cast<GLint>(color.b * 255)};
	TwSetParam(pBar, NULL, "color", TW_PARAM_INT32, 3, barColorArray);


	// Configurate viewport type enumeration
	TwEnumVal viewportTypesEV[] = { {VIEWPORT_TFEDITOR, "TfEditor"}, {VIEWPORT_RENDERER, "Renderer"}, {VIEWPORT_SLICER, "Slicer"} };
	TwType viewportTypes = TwDefineEnum("Viewport Types", viewportTypesEV, 3);

	TwAddVarRW(pBar, "Type", viewportTypes, &bar_type, NULL);

	// Set first bar entries
	TwAddSeparator(pBar, NULL, "");
}

ViewportType Viewport::update(GLfloat tpf, InputData inputData, GLboolean selected)
{
	// Set selected
	this->selected = selected;

	// Update bar variables
	if(barActive)
	{
		updateBarVariables();
		useBarVariables();
	}

	// Only if selected, handle input
	if(selected)
	{
		handleInput(inputData);
	}

	return VIEWPORT_NONE;
}

void Viewport::draw()
{
	// Fill bars with new data (after everything was updated)
	if(barActive)
	{
		fillBarVariables();
	}

	glEnable(GL_SCISSOR_TEST);

	// Clear with border color
	glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
	glScissor(viewportX, viewportY, viewportWidth, viewportHeight);
	if(selected)
	{
		glClearColor(color.r, color.g, color.b, 1);
	}
	else
	{
		glClearColor(color.r*VIEWPORT_BORDER_COLOR_UNSELECTED_MULTIPLIER, color.g*VIEWPORT_BORDER_COLOR_UNSELECTED_MULTIPLIER, color.b*VIEWPORT_BORDER_COLOR_UNSELECTED_MULTIPLIER, 1);
	}
	glClear(GL_COLOR_BUFFER_BIT);

	// Clear inner area
	glViewport(viewportX+VIEWPORT_BORDER_WIDTH, viewportY+VIEWPORT_BORDER_WIDTH, 
		viewportWidth-2*VIEWPORT_BORDER_WIDTH, viewportHeight-2*VIEWPORT_BORDER_WIDTH);
	glScissor(viewportX+VIEWPORT_BORDER_WIDTH, viewportY+VIEWPORT_BORDER_WIDTH, 
		viewportWidth-2*VIEWPORT_BORDER_WIDTH, viewportHeight-2*VIEWPORT_BORDER_WIDTH);
	glClearColor(VIEWPORT_INNER_COLOR.r, VIEWPORT_INNER_COLOR.g, VIEWPORT_INNER_COLOR.b, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_SCISSOR_TEST);
}

void Viewport::terminate()
{
	TwDeleteBar(pBar);
}

void Viewport::viewportResize(GLint windowWidth, GLint windowHeight)
{
	this->viewportX = static_cast<GLint>(relativePosX*windowWidth);
	this->viewportY = static_cast<GLint>(relativePosY*windowHeight);
	
	// Avoid empty spaces
	if((this->relativePosX+this->relativeWidth) == 1)
	{
		this->viewportWidth = windowWidth - this->viewportX;
	}
	else
	{
		this->viewportWidth = static_cast<GLint>(relativeWidth*windowWidth);
	}

	if((this->relativePosY+this->relativeHeight) == 1)
	{
		this->viewportHeight = windowHeight - this->viewportY;
	}
	else
	{
		this->viewportHeight = static_cast<GLint>(relativeHeight*windowHeight);
	}

	// Set bar to position of viewport
	GLint barPositionArray[] = {static_cast<GLint>(relativePosX*windowWidth), static_cast<GLint>((1-relativePosY-relativeHeight)*windowHeight)};
	TwSetParam(pBar, NULL, "position", TW_PARAM_INT32, 2, barPositionArray);
}

void Viewport::unselect()
{
}

GLboolean Viewport::intersect(glm::vec2 coord)
{
	GLboolean result = GL_FALSE;

	if((coord.x >= viewportX) &&(coord.x <= viewportX + viewportWidth))
	{
		if((coord.y >= viewportY) &&(coord.y <= viewportY + viewportHeight))
		{
			result = GL_TRUE;
		}
	}
	return result;
}


GLint Viewport::getHandle() const
{
	return this->handle;
}

GLfloat Viewport::getAspectRatio() const
{
	return static_cast<GLfloat>(viewportWidth)/viewportHeight;
}

void Viewport::setBarActive(GLboolean barActive)
{
	this->barActive = barActive;
}

ViewportType Viewport::getType() const
{
	return type;
}

GLfloat Viewport::getRelativePosX() const
{
	return relativePosX;
}

GLfloat Viewport::getRelativePosY() const
{
	return relativePosY;
}
	
GLfloat Viewport::getRelativeWidth() const
{
	return relativeWidth;
}

GLfloat Viewport::getRelativeHeight() const
{
	return relativeHeight;
}

void Viewport::handleInput(InputData inputData)
{
	// Calculate relative cursor position in viewport
	relativeCursorPos.x = (inputData.cursor_pos.x - viewportX) / viewportWidth;
	relativeCursorPos.y = (inputData.cursor_pos.y - viewportY) / viewportHeight;

	// Delta cursor position
	relativeCursorPosDelta.x = inputData.cursor_pos_delta.x / viewportWidth;
	relativeCursorPosDelta.y = inputData.cursor_pos_delta.y / viewportHeight;
}

void Viewport::updateBarVariables()
{
}

void Viewport::useBarVariables()
{
}

void Viewport::fillBarVariables()
{
}

void Viewport::showBarVariable(std::string name, GLboolean show)
{
	GLchar* barParam;
	if(show)
	{
		barParam = "true";
	}
	else
	{
		barParam = "false";
	}
	TwSetParam(pBar, name.c_str(), "visible", TW_PARAM_CSTRING, 1, barParam);
}