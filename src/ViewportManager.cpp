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

#include "ViewportManager.h"

ViewportManager::ViewportManager()
{
	selectedViewportHandle = -1;
	viewportHandleCounter = 0;
}

ViewportManager::~ViewportManager()
{
	deleteAllViewports();
}

void ViewportManager::init(GLint windowWidth, GLint windowHeight, TfManager* pTfManager, RcManager* pRcManager, VolumeManager* pVolumeManager, GLboolean barsActive)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->pTfManager = pTfManager;
	this->pRcManager = pRcManager;
	this->pVolumeManager = pVolumeManager;
	this->barsActive = barsActive;
}

void ViewportManager::update(GLfloat tpf, InputData inputData)
{
	// Save previous handle
	GLint prevSelectedViewportHandle = selectedViewportHandle;

	// If any mouse button is pressed, check which viewport should be active
	if(inputData.mouse_action == GLFW_PRESS)
	{
		selectedViewportHandle = getViewportAt(inputData.cursor_pos);

		// Tell previous selected viewport about unselection
		if(prevSelectedViewportHandle != selectedViewportHandle)
		{
			if(prevSelectedViewportHandle >= 0)
			{
				getViewport(prevSelectedViewportHandle)->unselect();
			}
		}
	}

	// Update viewports
	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		Viewport* pViewport = it->second;

		ViewportType origType = pViewport->getType();
		ViewportType newType;

		if(it->first == selectedViewportHandle)
		{
			newType = pViewport->update(tpf, inputData, GL_TRUE);
			
		}
		else
		{
			newType = pViewport->update(tpf, inputData, GL_FALSE);
		}

		// Viewports want to be someone else
		if(newType != origType)
		{
			GLfloat posX = pViewport->getRelativePosX();
			GLfloat posY = pViewport->getRelativePosY();
			GLfloat width = pViewport->getRelativeWidth();
			GLfloat height = pViewport->getRelativeHeight();
			GLint handle = pViewport->getHandle();
			deleteViewport(handle);
			addViewport(newType, posX, posY, width, height, handle);

			// Iteration over map must be stopped
			break;
		}	
	}
}

void ViewportManager::draw()
{
	// Draw all viewports
	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		it->second->draw();
	}

	// Reset glViewport
	glViewport(0, 0, windowWidth, windowHeight);
}

void ViewportManager::terminate()
{
	deleteAllViewports();
}

GLint ViewportManager::addViewport(ViewportType type, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint handle)
{
	GLint newHandle;
	if(handle >= 0)
	{
		newHandle = handle;
	}
	else
	{
		newHandle = viewportHandleCounter;

		// Increment handle counter
		viewportHandleCounter++;
	}

	// Create new viewport object and inititialize them
	Viewport* pViewport;
	glm::vec3 color = viewportColors[newHandle % (sizeof(viewportColors)/sizeof(glm::vec3))];
	

	if(type == VIEWPORT_RENDERER)
	{
		Renderer* pRenderer = new Renderer();
		pRenderer->init(pTfManager, pRcManager, pVolumeManager, newHandle, color, "Renderer", posX, posY, width, height, windowWidth, windowHeight, barsActive);
		pViewport = pRenderer;
	}
	else if(type == VIEWPORT_TFEDITOR)
	{
		TfEditor* pTfEditor = new TfEditor();
		pTfEditor->init(pTfManager, pVolumeManager, newHandle, color, "TfEditor", posX, posY, width, height, windowWidth, windowHeight, barsActive);
		pViewport = pTfEditor;
	}
	else
	{
		Slicer* pSlicer = new Slicer();
		pSlicer->init(pVolumeManager, newHandle, color, "Slicer", posX, posY, width, height, windowWidth, windowHeight, barsActive);
		pViewport = pSlicer;
	}

	// Put it into map
	viewports[newHandle] = pViewport;

	// Set new viewport as active
	unselectSelectedViewport();
	selectedViewportHandle = newHandle;

	// Return handle
	return newHandle;
}

void ViewportManager::deleteViewport(GLint handle)
{
	// Unselect if selected
	if(selectedViewportHandle == handle)
	{
		unselectSelectedViewport();
	}

	// Terminate, delete and remove pointer from map
	if(getViewport(handle) != NULL)
	{
		getViewport(handle)->terminate();
		delete getViewport(handle);
		viewports.erase(handle);
	}
}

void ViewportManager::deleteAllViewports()
{
	// Delete all viewports
	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		if((it->second) != NULL)
		{
			it->second->unselect();
			it->second->terminate();
			delete it->second;
		}
	}

	// Clear map
	viewports.clear();

	// Set active viewport handle to -1
	unselectSelectedViewport();

	// Reset handle counter
	viewportHandleCounter = 0;
}

void ViewportManager::windowResize(GLint windowWidth, GLint windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	// Adjust all viewports
	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		it->second->viewportResize(windowWidth, windowHeight);
	}
}

Viewport* ViewportManager::getViewport(GLint handle)
{
	return viewports[handle];
}

void ViewportManager::unselectSelectedViewport()
{
	if(selectedViewportHandle >= 0)
	{
		// Deselect only if something exists to deselect
		if(viewports.size() > 0)
		{
			getViewport(selectedViewportHandle)->unselect();
		}
		selectedViewportHandle = -1;
	}
}

void ViewportManager::setBarsActive(GLboolean barsActive)
{
	this->barsActive = barsActive;

	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		it->second->setBarActive(barsActive);
	}
}

void ViewportManager::setVolumeInAllViewports(GLint volumeHandle)
{
	// Ugly reinterpret casting, viewport stuff may need a rewrite
	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		ViewportType type = it->second->getType();
		switch(type)
		{
		case VIEWPORT_RENDERER:
			{
				Renderer* pRenderer = reinterpret_cast<Renderer*>(it->second);
				pRenderer->setVolumeHandle(volumeHandle);
				break;
			}
		case VIEWPORT_TFEDITOR:
			{
				TfEditor* pTfEditor = reinterpret_cast<TfEditor*>(it->second);
				pTfEditor->setVolumeHandle(volumeHandle);
				break;
			}
		case VIEWPORT_SLICER:
			{
				Slicer* pSlicer = reinterpret_cast<Slicer*>(it->second);
				pSlicer->setVolumeHandle(volumeHandle);
				break;
			}
		}
	}
}

GLint ViewportManager::getViewportAt(glm::vec2 coord)
{
	GLint result = -1;

	// Find viewport at coordinates
	for(std::map<GLint, Viewport*>::iterator it = viewports.begin(); it != viewports.end(); ++it)
	{
		if(it->second->intersect(coord))
		{
			result = it->first;
			break;
		}
	}

	return result;
}