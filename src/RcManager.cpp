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

#include "RcManager.h"

RcManager::RcManager()
{
	rcHandleCounter = 0;
	newRcCounter = 0;
}

RcManager::~RcManager()
{
	// Delete all raycaster
	for(std::map<GLint, Raycaster*>::iterator it = raycaster.begin(); it != raycaster.end(); ++it)
	{
		if((it->second) != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	
	}

	// Clear map
	raycaster.clear();
}

void RcManager::init()
{
}

GLint RcManager::newRc()
{
	// Generate name
	std::string name = RCMANAGER_NEW_RC_NAME + UT::to_string(newRcCounter);
	newRcCounter++;

	// Logging
	LogInfo("Create raycaster: " + name);

	// Let creator create new transferfunction
	Raycaster* pRaycaster = rcCreator.create(rcHandleCounter, name);

	// Add raycaster to map
	raycaster[rcHandleCounter] = pRaycaster;

	// Set latest handle
	latestRcHandle = rcHandleCounter;

	// Increment handle counter
	rcHandleCounter++;

	return latestRcHandle;
}

void  RcManager::reloadRc(GLint handle)
{
	// Logging
	LogInfo("Reload raycaster: " + getRc(handle)->getName());

	Raycaster* pOldRaycaster = getRc(handle);

	Raycaster* pReloadedRaycaster = rcCreator.readFromFile(pOldRaycaster->getName(), handle);

	// If there exists a XML-File with same name as raycaster, proceed
	if(pReloadedRaycaster != NULL)
	{
		delete pOldRaycaster;
		raycaster[handle] = pReloadedRaycaster;
	}
}

GLboolean RcManager::saveRc(GLint handle, GLboolean overwriteExisting)
{
	// Logging
	LogInfo("Save raycaster: " + getRc(handle)->getName());

	// Forward to creator
	return rcCreator.writeToFile(getRc(handle), overwriteExisting);
}

GLint RcManager::loadRc(std::string name)
{
	// Logging
	LogInfo("Load raycaster: " + name);

	// Read raycaster from file
	Raycaster* pRaycaster = rcCreator.readFromFile(name, rcHandleCounter);

	// Check wether loading was successful
	if(pRaycaster != NULL)
	{
		// Add to map
		raycaster[rcHandleCounter] = pRaycaster;

		// Set latest rcHandle
		latestRcHandle = rcHandleCounter;

		// Increment rcHandle counter
		rcHandleCounter++;

		// Return handle
		return latestRcHandle;
	}
	else
	{
		return -1;
	}	
}

Raycaster* RcManager::getRc(GLint handle) const
{
	// Only possible because raycaster may not be deleted
	if(raycaster.size() > static_cast<GLuint>(handle))
	{
		return raycaster.at(handle);
	}
	else
	{
		return NULL;
	}
}

GLint RcManager::getLatestRcHandle() const
{
	return latestRcHandle;
}