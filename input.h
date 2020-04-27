#pragma once

const Uint8 *state = SDL_GetKeyboardState(NULL);

void check_input()
{
	state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE])
	{
		quit = true;
	}
	SDL_GetMouseState(&mouse_x, &mouse_y);
}


bool getKey(int what_want)
{
	if (GetAsyncKeyState(what_want) & 0x7FFF)
	{
		return true;
	}
	return false;
}