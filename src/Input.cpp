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

#include "Input.h"

// Global static pointer to the single instance
Input* Input::pInstance = NULL;

Input::Input()
{
}

Input::~Input()
{
}

Input* Input::instantiate(GLFWwindow* pWindow, GLint windowWidth, GLint windowHeight)
{
	// Exists an instance?
	if(pInstance == NULL)
	{
		// Create instance of it on stack
		static Input input;
		input.init(pWindow, windowWidth, windowHeight);
		pInstance = &input;
	}
	
	return pInstance;
}

void Input::init(GLFWwindow* pWindow, GLint windowWidth, GLint windowHeight)
{
	// Members
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	// initialize inputData
	inputData.reset();

	// Set GLFW callbacks to this class
	glfwSetKeyCallback(pWindow, keyCallback);
	glfwSetCursorPosCallback(pWindow, cursorCallback);
	glfwSetMouseButtonCallback(pWindow, buttonsCallback);
	glfwSetScrollCallback(pWindow, scrollCallback);
}

InputData Input::getInputData() const
{
	return inputData;
}

void Input::resetInputData()
{
	inputData.reset();
}

void Input::key(GLint key, GLint action, GLint mods)
{
	inputData.key_int = key;
	inputData.key_action = action;
	inputData.key_mods = mods;
	inputData.key_changed = true;

	inputData.key_int_old = getGLFW2Key(key, mods);
}

void Input::cursor(GLfloat xpos, GLfloat ypos)
{
	inputData.cursor_pos = glm::vec2(xpos, windowHeight - 1 - ypos);
	inputData.cursor_pos_changed = true;
}

void Input::buttons(GLint button, GLint action, GLint mods)
{
	inputData.mouse_button = button;
	inputData.mouse_action = action;
	inputData.mouse_mod = mods;
	inputData.mouse_changed = true;

	// Left button
	if(button == GLFW_MOUSE_BUTTON_1)
	{
		if(action == GLFW_PRESS)
		{
			inputData.mouse_drag_left = true;
		}
		else if(action == GLFW_RELEASE)
		{
			inputData.mouse_drag_left = false;
		}
	}

	// Right button
	if(button == GLFW_MOUSE_BUTTON_2)
	{
		if(action == GLFW_PRESS)
		{
			inputData.mouse_drag_right = true;
		}
		else if(action == GLFW_RELEASE)
		{
			inputData.mouse_drag_right = false;
		}
	}

	// Middle button
	if(button == GLFW_MOUSE_BUTTON_3)
	{
		if(action == GLFW_PRESS)
		{
			inputData.mouse_drag_middle = true;
		}
		else if(action == GLFW_RELEASE)
		{
			inputData.mouse_drag_middle = false;
		}
	}
}

void Input::scroll(GLfloat xoffset, GLfloat yoffset)
{
	inputData.scroll = glm::vec2(xoffset, yoffset);
	inputData.scroll_changed = true;
}

void Input::keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods)
{
	Input::pInstance->key(key, action, mods);
}

void Input::cursorCallback (GLFWwindow* pWindow, GLdouble xpos, GLdouble ypos)
{
	Input::pInstance->cursor((GLfloat)xpos, (GLfloat)ypos);
}

void Input::buttonsCallback(GLFWwindow* pWindow, GLint button, GLint action, GLint mods)
{
	Input::pInstance->buttons(button, action, mods);
}

void Input::scrollCallback(GLFWwindow* pWindow, GLdouble xoffset, GLdouble yoffset)
{
	Input::pInstance->scroll((GLfloat)xoffset, (GLfloat)yoffset);
}

void Input::windowResize(GLint windowWidth, GLint windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}

GLint Input::getGLFW2Key(GLint key, GLint mods) const
{
	switch (key)
	{
	// *** SPECIAL LETTERS ***
	case GLFW_KEY_ENTER:
		return 256 + 38;
	case GLFW_KEY_BACKSPACE:
		return 256 + 39;

	// *** LETTERS ***
	case GLFW_KEY_A:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'A';
		}
		return 'a';
	case GLFW_KEY_B:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'B';
		}
		return 'b';
	case GLFW_KEY_C:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'C';
		}
		return 'c';
	case GLFW_KEY_D:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'D';
		}
		return 'd';
	case GLFW_KEY_E:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'E';
		}
		return 'e';
	case GLFW_KEY_F:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'F';
		}
		return 'f';
	case GLFW_KEY_G:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'G';
		}
		return 'g';
	case GLFW_KEY_H:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'H';
		}
		return 'h';
	case GLFW_KEY_I:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'I';
		}
		return 'i';
	case GLFW_KEY_J:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'J';
		}
		return 'j';
	case GLFW_KEY_K:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'K';
		}
		return 'k';
	case GLFW_KEY_L:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'L';
		}
		return 'l';
	case GLFW_KEY_M:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'M';
		}
		return 'm';
	case GLFW_KEY_N:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'N';
		}
		return 'n';
	case GLFW_KEY_O:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'O';
		}
		return 'o';
	case GLFW_KEY_P:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'P';
		}
		return 'p';
	case GLFW_KEY_Q:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'Q';
		}
		return 'q';
	case GLFW_KEY_R:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'R';
		}
		return 'r';
	case GLFW_KEY_S:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'S';
		}
		return 's';
	case GLFW_KEY_T:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'T';
		}
		return 't';
	case GLFW_KEY_U:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'U';
		}
		return 'u';
	case GLFW_KEY_V:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'V';
		}
		return 'v';
	case GLFW_KEY_W:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'W';
		}
		return 'w';
	case GLFW_KEY_X:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'X';
		}
		return 'x';
	case GLFW_KEY_Y:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'Y';
		}
		return 'y';
	case GLFW_KEY_Z:
		if(inputData.key_mods == GLFW_MOD_SHIFT)
		{
			return 'Z';
		}
		return 'z';
	}

	return key;
}