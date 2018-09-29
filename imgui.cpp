﻿#include "imgui.h"
#include "buffer.h"
#include "pixie.h"
#include "font.h"
#include <assert.h>

using namespace Pixie;

struct State
{
	enum Flags
	{
		Started = 1 << 0,
	};

	bool HasStarted() { return (flags & Flags::Started) != 0; }
	int GetNextId() { return nextId++; }

	int flags;
	int nextId;
	int hoverId;
	int focusId;
	int keyboardCursorPosition;
	float keyRepeatTimer;
	float keyRepeatTime;
	float cursorBlinkTimer;
	uint32 defaultTextColour;

	Window* window;
	Font* font;
};

static State s_state = { 0 };

void ImGui::Begin(Window* window, Font* font)
{
	assert(window);
	assert(font);

	s_state.flags = State::Flags::Started;
	s_state.nextId = 1;
	s_state.hoverId = 0;
	s_state.window = window;
	s_state.font = font;
	s_state.defaultTextColour = MAKE_RGB(200, 200, 200);
}

void ImGui::End()
{
	s_state.flags = 0;

	if (s_state.window->HasMouseGoneDown(Pixie::MouseButton_Left))
	{
		// If mouse has gone down over empty space, clear the current focus.
		if (s_state.hoverId == 0)
		{
			s_state.focusId = 0;
			s_state.keyboardCursorPosition = 0;
		}
	}

	s_state.window = 0;
}

void ImGui::Label(const char* text, int x, int y, uint32 colour)
{
	assert(text);
	assert(s_state.HasStarted());
	s_state.font->DrawColour(text, x, y, colour, s_state.window->GetBuffer());
}

bool ImGui::Button(const char* label, int x, int y, int width, int height)
{
	assert(s_state.HasStarted());

	Window* window = s_state.window;
	int id = s_state.GetNextId();

	const uint32 NormalColour = MAKE_RGB(32, 50, 77);
	const uint32 HoverColour = MAKE_RGB(39, 73, 114);
	const uint32 PressedColour = MAKE_RGB(22, 40, 67);
	const uint32 BorderColour = MAKE_RGB(68, 79, 103);
	const uint32 FocusBorderColour = MAKE_RGB(200, 200, 229);

	int mouseX = window->GetMouseX();
	int mouseY = window->GetMouseY();

	bool hover = mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
	bool pressed = false;

	if (hover)
	{
		s_state.hoverId = id;

		// Mouse has just gone down over this element, so give it focus.
		if (window->HasMouseGoneDown(Pixie::MouseButton_Left))
			s_state.focusId = id;

		// If mouse is still down over this element and it has focus, then it is pressed.
		pressed = window->IsMouseDown(Pixie::MouseButton_Left) && s_state.focusId == id;
	}

	uint32 buttonColour = pressed ? PressedColour : hover ? HoverColour : NormalColour;
	uint32 borderColour = s_state.focusId == id ? FocusBorderColour : BorderColour;

	Buffer* buffer = window->GetBuffer();
	uint32* pixels = buffer->GetPixels();
	int bufferWidth = buffer->GetWidth();
	int bufferHeight = buffer->GetHeight();

	FilledRect(x, y, width, height, buttonColour, borderColour);

	if (label)
	{
		Font* font = s_state.font;
		int textX = x + ((width - font->GetStringWidth(label)) >> 1);
		int textY = y + ((height - font->GetCharacterHeight()) >> 1);

		Label(label, textX, textY, s_state.defaultTextColour);
	}

	return hover && s_state.focusId == id && window->HasMouseGoneUp(Pixie::MouseButton_Left);
}

void ImGui::Input(char* text, int textBufferLength, int x, int y, int width, int height)
{
	assert(text);
	assert(s_state.HasStarted());

	Window* window = s_state.window;
	int id = s_state.GetNextId();

	const int LeftMargin = 8;
	const uint32 NormalColour = MAKE_RGB(64, 68, 71);
	const uint32 HoverColour = MAKE_RGB(74, 78, 81);
	const uint32 BorderColour = MAKE_RGB(104, 108, 111);
	const uint32 FocusBorderColour = MAKE_RGB(200, 200, 200);
	const uint32 CursorColour = MAKE_RGB(220, 220, 220);
	const int CursorWidth = 8;
	const float KeyRepeatTimeInit = 0.2f;
	const float KeyRepeatTimeRepeat = 0.05f;
	const float CursorBlinkTime = 1.0f;

	int mouseX = window->GetMouseX();
	int mouseY = window->GetMouseY();

	bool hover = mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
	bool pressed = false;
	int textLength = strlen(text);

	int textX = x + LeftMargin;

	if (hover)
	{
		s_state.hoverId = id;

		// Mouse has just gone down over this element, so give it focus.
		if (window->HasMouseGoneDown(Pixie::MouseButton_Left))
		{
			if (s_state.focusId != id)
			{
				s_state.keyRepeatTimer = 0.0f;
				s_state.cursorBlinkTimer = 0.0f;
				s_state.focusId = id;
			}

			// Move the cursor to whereever the user clicked.
			s_state.keyboardCursorPosition = min((mouseX - textX) / s_state.font->GetCharacterWidth(), textLength);
		}

		// If mouse is still down over this element and it has focus, then it is pressed.
		pressed = window->IsMouseDown(Pixie::MouseButton_Left) && s_state.focusId == id;
	}

	uint32 boxColour = pressed || hover || s_state.focusId == id ? HoverColour : NormalColour;
	uint32 borderColour = s_state.focusId == id ? FocusBorderColour : BorderColour;

	// Draw the input field.
	FilledRect(x, y, width, height, boxColour, borderColour);

	int textY = y + ((height - s_state.font->GetCharacterHeight()) >> 1);
	Label(text, textX, textY, s_state.defaultTextColour);

	if (s_state.focusId == id)
	{
		float delta = window->GetDelta();

		// Input field has focus, draw the keyboard cursor and process input.
		s_state.cursorBlinkTimer -= delta;
		if (s_state.cursorBlinkTimer >= CursorBlinkTime * 0.5f)
			FilledRect(textX + (s_state.keyboardCursorPosition * s_state.font->GetCharacterWidth()), textY, CursorWidth, s_state.font->GetCharacterHeight(), CursorColour, CursorColour);
		if (s_state.cursorBlinkTimer <= 0.0f)
			s_state.cursorBlinkTimer = CursorBlinkTime;


		if (window->HasAnyKeyGoneDown())
		{
			s_state.keyRepeatTimer = 0.0f;
			s_state.keyRepeatTime = KeyRepeatTimeInit;
		}

		s_state.keyRepeatTimer -= delta;
		if (s_state.keyRepeatTimer <= 0.0f)
		{
			s_state.keyRepeatTimer = s_state.keyRepeatTime;
			s_state.keyRepeatTime = KeyRepeatTimeRepeat;

			if (window->IsKeyDown(Pixie::Key::Left))
			{
				s_state.keyboardCursorPosition = max(s_state.keyboardCursorPosition - 1, 0);
			}
			else if (window->IsKeyDown(Pixie::Key::Right))
			{
				s_state.keyboardCursorPosition = min(s_state.keyboardCursorPosition + 1, textLength);
			}
			else if (window->IsKeyDown(Pixie::Key::Backspace))
			{
				// Move cursor back.
				s_state.keyboardCursorPosition--;

				if (s_state.keyboardCursorPosition >= 0)
				{
					// Copy everything after the current position to the current position.
					int position = s_state.keyboardCursorPosition;
					int copyAmount = strlen(text + position + 1) + 1;
					memcpy(text + position, text + position + 1, copyAmount);
				}
				else
				{
					s_state.keyboardCursorPosition = 0;
				}
			}
			else if (window->IsKeyDown(Pixie::Key::Delete))
			{
				// Delete the current character.
				int position = s_state.keyboardCursorPosition;
				if (position < textLength)
				{
					// Copy everything after the current position to the current position.
					int copyAmount = strlen(text + position + 1) + 1;
					memcpy(text + position, text + position + 1, copyAmount);
				}
			}
			else if (window->IsKeyDown(Pixie::Key::Home))
			{
				s_state.keyboardCursorPosition = 0;
			}
			else if (window->IsKeyDown(Pixie::Key::End))
			{
				s_state.keyboardCursorPosition = textLength;
			}
			else if (s_state.keyboardCursorPosition < textBufferLength - 1)
			{
				for (int i = Pixie::Key::A; i < Pixie::Key::Nine; i++)
				{
					if (window->IsKeyDown((Pixie::Key)i))
					{
						// Ignore the numbers if shift is held down.
						if ((Pixie::Key)i >= Pixie::Key::Zero && window->IsKeyDown(Pixie::Key::LeftShift) || window->IsKeyDown(Pixie::Key::RightShift))
							continue;

						// Overwrite the character at the current position.
						int position = s_state.keyboardCursorPosition;
						char character = window->GetChar((Pixie::Key)i);
						text[position] = character;

						// If at the end of the string, add a null because we've just extended the string.
						if (position == textLength)
							text[position+1] = 0;

						// Move the cursor.
						s_state.keyboardCursorPosition = min(s_state.keyboardCursorPosition + 1, textBufferLength);
						break;
					}
				}
			}
		}
	}
}

bool ImGui::Checkbox(const char* label, bool checked, int x, int y)
{
	assert(label);
	assert(s_state.HasStarted());

	const int TextLeftMargin = 8;
	const int BoxSize = 18;
	const int CheckSize = 8;

	Font* font = s_state.font;
	int charHeight = font->GetCharacterHeight();

	int textY = y + ((BoxSize - charHeight) >> 1) + 1;
	Label(label, x + BoxSize + TextLeftMargin, textY, s_state.defaultTextColour);
	bool wasChecked = checked;
	if (Button(0, x, y, BoxSize, BoxSize))
		checked = !checked;

	if (wasChecked)
	{
		// Draw check mark.
		int checkX = x + ((BoxSize - CheckSize) >> 1);
		int checkY = y + ((BoxSize - CheckSize) >> 1);
		Buffer* buffer = s_state.window->GetBuffer();
		int bufferWidth = buffer->GetWidth();
		uint32* pixels = buffer->GetPixels();
		y = checkY;
		for (int yy = y*bufferWidth, x = 0; y < checkY + CheckSize; y++, x++, yy += bufferWidth)
		{
			pixels[checkX + x + yy] = MAKE_RGB(255, 255, 255);
			pixels[checkX + CheckSize - x - 1 + yy] = MAKE_RGB(255, 255, 255);
		}
	}

	return checked;
}

bool ImGui::RadioButton(const char* label, bool checked, int x, int y)
{
	assert(label);
	assert(s_state.HasStarted());

	const int TextLeftMargin = 8;
	const int BoxSize = 18;
	const int CheckSize = 8;
	const uint32 FontColour = s_state.defaultTextColour;

	Font* font = s_state.font;
	int charHeight = font->GetCharacterHeight();

	int textY = y + ((BoxSize - charHeight) >> 1) + 1;
	Label(label, x + BoxSize + TextLeftMargin, textY, FontColour);
	bool wasChecked = checked;
	if (Button(0, x, y, BoxSize, BoxSize))
		checked = !checked;

	if (wasChecked)
	{
		// Draw radio button mark.
		int checkX = x + ((BoxSize - CheckSize) >> 1);
		int checkY = y + ((BoxSize - CheckSize) >> 1);
		FilledRect(checkX, checkY, CheckSize, CheckSize, FontColour, FontColour);
	}

	return checked;
}

void ImGui::Rect(int x, int y, int width, int height, uint32 borderColour)
{
	assert(s_state.HasStarted());
	Buffer* buffer = s_state.window->GetBuffer();
	uint32* pixels = buffer->GetPixels();
	int bufferWidth = buffer->GetWidth();
	int bufferHeight = buffer->GetHeight();

	pixels += x + (y*bufferWidth);

	for (int j = 0, ypos = y; j < height && ypos < bufferHeight; j++, ypos++)
	{
		int left = x;
		if (left >= 0 && left < bufferWidth)
			*pixels = borderColour;

		int right = x + width - 1;
		if (right >= 0 && right < bufferWidth)
			*(pixels + width - 1) = borderColour;

		pixels += bufferWidth;
	}

	pixels = buffer->GetPixels() + x + (y*bufferWidth);
	for (int i = 0, xpos = x; i < width; i++, xpos++)
	{
		int top = y;
		if (top >= 0 && top < bufferHeight)
			*pixels = borderColour;

		int bottom = y + height - 1;
		if (bottom >= 0 && bottom < bufferHeight)
			*(pixels + ((height-1)*bufferWidth)) = borderColour;

		pixels++;
	}
}

void ImGui::FilledRect(int x, int y, int width, int height, uint32 colour, uint32 borderColour)
{
	assert(s_state.HasStarted());
	Buffer* buffer = s_state.window->GetBuffer();
	uint32* pixels = buffer->GetPixels();
	int bufferWidth = buffer->GetWidth();
	int bufferHeight = buffer->GetHeight();

	pixels += x + (y*bufferWidth);

	for (int j = 0, ypos = y; j < height && ypos < bufferHeight; j++, ypos++)
	{
		for (int i = 0, xpos = x; i < width; i++, xpos++)
		{
			if (xpos < 0 || xpos >= bufferWidth || ypos < 0 || ypos >= bufferHeight)
				continue;
			*pixels = (i == 0 || i == width - 1 || j == 0 || j == height - 1) ? borderColour : colour;
			pixels++;
		}

		pixels += bufferWidth - width;
	}
}

void ImGui::FilledRoundedRect(int x, int y, int width, int height, uint32 colour, uint32 borderColour)
{
	assert(s_state.HasStarted());
	Buffer* buffer = s_state.window->GetBuffer();
	uint32* pixels = buffer->GetPixels();
	int bufferWidth = buffer->GetWidth();
	int bufferHeight = buffer->GetHeight();

	const int NumPixels = 1;

	pixels += x + (y*bufferWidth);

	for (int j = 0, ypos = y; j < height && ypos < bufferHeight; j++, ypos++)
	{
		for (int i = 0, xpos = x; i < width; i++, xpos++)
		{
			if (xpos < 0 || xpos >= bufferWidth || ypos < 0 || ypos >= bufferHeight)
				continue;
			if ((i < NumPixels || i >= width - NumPixels) && (j < NumPixels || j >= height - NumPixels))
			{
				pixels++;
				continue;
			}

			*pixels = (i == 0 || i == width - 1 || j == 0 || j == height - 1) ? borderColour : colour;
			pixels++;
		}

		pixels += bufferWidth - width;
	}
}
