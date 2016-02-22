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
 * BarVariable
 *--------------
 * Encapsulates the variable which is
 * controlled by the bar. Check every frame
 * whether its value was changed.
 *
 */

#ifndef BARVARIABLE_H_
#define BARVARIABLE_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"

template <class T> class BarVariable
{
public:
    BarVariable();
    virtual ~BarVariable();

    /** Used to refresh "changed" */
    void update();

    /** Get state of variable */
    GLboolean hasChanged() const;

    /** Get the value */
    T getValue() const;

    /** Set the value */
    void setValue(T value);

    /** AntTweakBar needs reference... */
    T value;

protected:
    T previousValue;
    GLboolean changed;
};

/** Definition of methods */

template <class T>
BarVariable<T>::BarVariable()
{
    changed = GL_FALSE;
}

template <class T>
BarVariable<T>::~BarVariable()
{
}

template <class T>
void BarVariable<T>::update()
{
    if(value != previousValue)
    {
        changed = GL_TRUE;
    }
    else if(changed)
    {
        changed = GL_FALSE;
    }
    previousValue = value;
}

template <class T>
GLboolean BarVariable<T>::hasChanged() const
{
    return changed;
}


template <class T>
T BarVariable<T>::getValue() const
{
    return value;
}

template <class T>
void BarVariable<T>::setValue(T value)
{
    this->value = value;
    this->previousValue = value;
    changed = GL_FALSE;
}

#endif
