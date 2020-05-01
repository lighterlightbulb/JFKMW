#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <list>
void Sleep(int time) {
	usleep(time * 1000);
}
#define DISABLE_NETWORK
#endif

#if defined(DARWIN)
#define DISABLE_NETWORK
#error "get the fuck out mac users"
#endif


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

istream& getline(istream& stream, string& str)
{
	char ch;
	str.clear();
	while (stream.get(ch) && ch != '\n')
		str.push_back(ch);
	return stream;
}

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>


#if not defined(DISABLE_NETWORK)
#include <SFML/Network.hpp>
#endif

#include <lz4.h>


#include "cout.h"

#include "Global.h"
#include "ASM/ASM.h"
#include "config.h"

#include "Sprite_shared.h"

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


#if not defined(DISABLE_NETWORK)
#include "Netplay.h"
#endif

#include "sprite.h"
#include "renderer.h"

#include "zsnes_ui.h"

#if not defined(DISABLE_NETWORK)
#include "server.h"
#endif
#include "main.h"

int main(int argc, char* argv[])
{
	load_configuration();

#if not defined(DISABLE_NETWORK)
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
#else
	player_code();
#endif
	cout << yellow << "[JFKMW] Quitting JFK mario world. Thanks for testing!" << white << endl;
	end_game();
	Sleep(1000);
	return 0;
}