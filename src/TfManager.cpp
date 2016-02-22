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

#include "TfManager.h"

TfManager::TfManager()
{
	tfHandleCounter = 0;
	newTfCounter = 0;
}

TfManager::~TfManager()
{
	// Delete all transferfunctions
	for(std::map<GLint, Transferfunction*>::iterator it = transferfunctions.begin(); it != transferfunctions.end(); ++it)
	{
		if((it->second) != NULL)
		{
			delete it->second;
			it->second = NULL;
		}

	}

	// Clear map
	transferfunctions.clear();
}

void TfManager::init()
{
}

GLint TfManager::newTf()
{
	// Generate name
	std::string name = TFMANAGER_NEW_TF_NAME + UT::to_string(newTfCounter);
	newTfCounter++;

	// Logging
	LogInfo("Create transferfunction: " + name);

	// Let creator create new transferfunction
	Transferfunction* pTransferfunction = tfCreator.create(tfHandleCounter, name);

	// Add transferfunction to map
	transferfunctions[tfHandleCounter] = pTransferfunction;

	// Set latest handle
	latestTfHandle = tfHandleCounter;

	// Increment handle counter
	tfHandleCounter++;

	return latestTfHandle;
}

void TfManager::reloadTf(GLint handle)
{
	// Logging
	LogInfo("Reload transferfunction: " + getTf(handle)->getName());

	Transferfunction* pOldTransferfunction = getTf(handle);

	Transferfunction* pReloadedTransferfunction = tfCreator.readFromFile(pOldTransferfunction->getName(), handle);

	// If there exists a XML-File with same name as transferfunction, proceed
	if(pReloadedTransferfunction != NULL)
	{
		delete pOldTransferfunction;
		transferfunctions[handle] = pReloadedTransferfunction;
	}
}

GLboolean TfManager::saveTf(GLint handle, GLboolean overwriteExisting)
{
	// Logging
	LogInfo("Save transferfunction: " + getTf(handle)->getName());

	// Forward to creator
	return tfCreator.writeToFile(getTf(handle), overwriteExisting);
}

GLint TfManager::loadTf(std::string name)
{
	// Logging
	LogInfo("Load transferfunction: " + name);

	// Read transferfunction from file
	Transferfunction* pTransferfunction = tfCreator.readFromFile(name, tfHandleCounter);

	// Check wether loading was successful
	if(pTransferfunction != NULL)
	{
		// Add to map
		transferfunctions[tfHandleCounter] = pTransferfunction;

		// Set latest TfHandle
		latestTfHandle = tfHandleCounter;

		// Increment tfHandle counter
		tfHandleCounter++;

		// Return handle
		return latestTfHandle;
	}
	else
	{
		return -1;
	}	
}

Transferfunction* TfManager::getTf(GLint handle) const
{
	// Only possible because transferfunctions may not be deleted
	if(transferfunctions.size() > static_cast<GLuint>(handle))
	{
		return transferfunctions.at(handle);
	}
	else
	{
		return NULL;
	}
}

GLint TfManager::getLatestTfHandle() const
{
	return latestTfHandle;
}
