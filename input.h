#pragma once

const Uint8 *state = SDL_GetKeyboardState(NULL);
SDL_GameController* gGameController;
bool BUTTONS_GAMEPAD[8];

void init_input()
{
	if (SDL_NumJoysticks() < 1)
	{
		std::cout << cyan << "[SDL] No controllers are plugged in." << endl;
	}
	else
	{
		//Load joystick
		gGameController = SDL_GameControllerOpen(0);
		if (gGameController == NULL)
		{
			std::cout << cyan << "[SDL] This controller ain't workin : " << SDL_GetError() << white << endl;
		}
		std::cout << cyan << "[SDL] Controller 0 is plugged in." << white << endl;
	}
}

void check_input()
{
	state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE])
	{
		quit = true;
	}
	SDL_GetMouseState(&mouse_x, &mouse_y);

	if (gGameController)
	{
		BUTTONS_GAMEPAD[0] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
		BUTTONS_GAMEPAD[1] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		BUTTONS_GAMEPAD[2] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
		BUTTONS_GAMEPAD[3] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);

		BUTTONS_GAMEPAD[4] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_A);
		BUTTONS_GAMEPAD[5] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_B);
		BUTTONS_GAMEPAD[6] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_X);
		BUTTONS_GAMEPAD[7] = SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_Y);

	}
}


bool getKey(int what_want)
{
	if (GetAsyncKeyState(what_want) & 0x7FFF)
	{
		return true;
	}
	return false;
}