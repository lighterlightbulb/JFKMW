#pragma once


void load_configuration()
{
	std::ifstream cFile(path + "game_config.cfg");
	if (cFile.is_open())
	{
		std::string line;
		while (getline(cFile, line)) {
			line.erase(std::remove_if(line.begin(), line.end(), isspace),
				line.end());
			if (line[0] == '#' || line.empty())
				continue;
			auto delimiterPos = line.find("=");
			auto name = line.substr(0, delimiterPos);
			auto value = line.substr(delimiterPos + 1);
			std::cout << cyan << "[CONFIG] Loading " << name << " = " << value << white << '\n';
			if (name == "scale") { scale = stoi(value); }
			if (name == "resolution_x") { resolution_x = stoi(value); }
			if (name == "resolution_y") { resolution_y = stoi(value); }
			if (name == "fullscreen") { fullscreen = value == "true"; }
			if (name == "opengl") { opengl = value == "true"; }
			if (name == "network_update_rate") { network_update_rate = stoi(value); }
			if (name == "packet_wait_time") { packet_wait_time = stoi(value); }
			if (name == "rendering_device") { rendering_device = stoi(value); }
			if (name == "renderer_accelerated") { renderer_accelerated = value == "true"; }
			if (name == "v_sync") { v_sync = value == "true"; }
			if (name == "username") { username = value; }
			if (name == "skin") { my_skin = stoi(value); }
			if (name == "port") { PORT = stoi(value); }

			if (value == "LeftShift") value = "Left Shift";
			if (value == "RightShift") value = "Right Shift";
			const char *v = value.c_str();
			if (name == "button_y") { input_settings[0] = SDL_GetScancodeFromName(v); }
			if (name == "button_b") { input_settings[1] = SDL_GetScancodeFromName(v); }
			if (name == "button_a") { input_settings[2] = SDL_GetScancodeFromName(v); }
			if (name == "button_x") { input_settings[3] = SDL_GetScancodeFromName(v); }
			if (name == "button_left") { input_settings[4] = SDL_GetScancodeFromName(v); }
			if (name == "button_right") { input_settings[5] = SDL_GetScancodeFromName(v); }
			if (name == "button_down") { input_settings[6] = SDL_GetScancodeFromName(v); }
			if (name == "button_up") { input_settings[7] = SDL_GetScancodeFromName(v); }
			if (name == "simulate_SNES_quirks") { simulate_SNES_quirks = value == "true"; }

		}

	}
}