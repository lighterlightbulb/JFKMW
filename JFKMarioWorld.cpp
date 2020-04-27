#include "pch.h"
#include <Windows.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <ratio>
#include <chrono>
#include <ctype.h>
#include <cstring>
#include <cassert>

using namespace std;


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SFML/Network.hpp>

#include <lz4.h>


#include "cout.h"

#include "Global.h"
#include "ASM/ASM.h"
#include "config.h"



#include "LuaImplementation.h"

#include "Sound.h"


#include "Global.h"
#include "SDL_General.h"


#include "map16block.h"
#include "exanimation.h"
#include "Level.h"

#include "input.h"

#include "Sprite_system.h"
#include "mario.h"

#include "game.h"

#include "Netplay.h"
#include "sprite.h"
#include "renderer.h"

#include "zsnes_ui.h"

#include "server.h"
#include "main.h"

int main(int argc, char* argv[])
{
	load_configuration();

	bool hosting = false;
	if (argc > 1)
	{
		if (strcmp(argv[1], "-h") == 0) {
			hosting = true;
		}
		if (strcmp(argv[1], "-c") == 0) {
			hosting = false;
		}
	}
	if (hosting) {
		if (argc > 2) {
			server_code(string(argv[2]));
		}
		else {
			server_code();
		}
	}
	else {
		player_code();
	}
	cout << green << "Quitting JFK mario world..." << white << endl;
	end_game();
	Sleep(1000);
	return 0;
}