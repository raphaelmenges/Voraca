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
 * TfMananger
 *--------------
 * Manages transferfunctions. It is not allowed to delete
 * transferfunctions. Otherwise one has to change the implementation
 * of getTf().
 *
 */

#ifndef TFMANAGER_H_
#define TFMANAGER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"

#include <map>
#include <string>

#include "Logger.h"
#include "Transferfunction.h"
#include "TfCreator.h"
#include "Utilities.h"

const std::string TFMANAGER_NEW_TF_NAME = "newTf";

class TfManager
{
public:
    TfManager();
    ~TfManager();

    /** Initialization */
    void init();

    /** Create new transferfunction */
    GLint newTf();

    /** Reload already loaded transferfunction */
    void reloadTf(GLint handle);

    /** Save transferfunction */
    GLboolean saveTf(GLint handle, GLboolean overwriteExisting);

    /** Load transferfunction. Returns -1 if it fails */
    GLint loadTf(std::string name);

    /** Returns pointer to function, for one-time-use only! */
    Transferfunction* getTf(GLint handle) const;

    /** Returns latest tfHandle */
    GLint getLatestTfHandle() const;

protected:
    /** Latest used handle */
    GLint latestTfHandle;

    /** Map with transferfunctions */
    std::map<GLint, Transferfunction*> transferfunctions;

    /** Counter for next handle that is free */
    GLint tfHandleCounter;

    /** Own creator for loading and saving */
    TfCreator tfCreator;

    /** Counter for new transferfunctions*/
    GLint newTfCounter;
};

#endif
