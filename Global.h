#pragma once

#define rom_asm_size 0x20000 //128kb
#define location_rom_levelasm 0x00000 //this will put LevelASM on the start of the ROM
#define location_temp_sprite_asm 0x10000

#define TotalBlocksCollisionCheck 2
#define TotalBlocksCollisionCheckSprite 4
#define bounds_x 6.0 //for collision crap
#define bounds_y 6.0
#define button_b 0
#define button_y 1
#define button_a 2
#define button_left 3
#define button_right 4
#define button_up 5
#define button_down 6

#define total_inputs 7

#define SpriteAmount 64
#define ogg_sample_rate 32000

int_fast16_t mapWidth = 256;
int_fast16_t mapHeight = 32;

#define RAM_Size 0x20000
#define VRAM_Size 0x10000
#define VRAM_Location 0x10000

uint_fast16_t palette_array[256];
uint_fast8_t VRAM[VRAM_Size];

#define top 8
#define bottom 4
#define left 2
#define right 1
#define inside 0


//threads

#if not defined(DISABLE_NETWORK)
sf::Thread* thread = 0;
#endif

//ASM


uint_fast8_t scale = 1;
int resolution_x = 320;
int resolution_y = 240;
int sp_offset_x = 32;
int sp_offset_y = 28;
uint_fast8_t my_skin = 0;
uint_fast32_t global_frame_counter = 0;

std::chrono::duration<double> total_time_ticks;
std::chrono::duration<double> latest_server_response;

unsigned int network_update_rate = 16;
unsigned int packet_wait_time = 16;
int mouse_x, mouse_y;
int data_size_current = 0;

int rendering_device = -1;
int controller = 0;
bool fullscreen = false;
bool opengl = false;
bool networking = false;
bool renderer_accelerated = true;
bool v_sync = true;

string username = "No username";

SDL_Scancode input_settings[8] = {
	SDL_SCANCODE_S,
	SDL_SCANCODE_Z,
	SDL_SCANCODE_X,
	SDL_SCANCODE_A,
	SDL_SCANCODE_LEFT,
	SDL_SCANCODE_RIGHT,
	SDL_SCANCODE_DOWN,
	SDL_SCANCODE_UP
};

vector<string> split(const string &s, char delim) {
	vector<string> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

double Calculate_Speed(double speed)
{
	return speed / 256.0;
}
double Calculate_Speed_X(double speed)
{
	return speed / 256.0;
}

std::string int_to_hex(int T, bool add_0 = false)
{
	std::stringstream stream;
	if (!add_0)
	{
		stream << std::hex << T;
	}
	else
	{
		if(T < 10)
		{
			stream << "0";
		}
		stream << std::hex << T;
	}
	
	return stream.str();
}


#ifdef NDEBUG
std::string path = "";
#else
std::string path = "E:/JFKMarioWorld/x64/JFKMarioWorld/Debug/";
#endif

bool quit = false;
bool actuallyquitgame = false;
//NET
bool doing_write = false;
bool doing_read = false;
bool recent_big_change = false;
bool disconnected = false;
bool isClient = false;

uint_fast8_t SelfPlayerNumber = 1;
int PlayerAmount = 0;


std::string da_epical_function_lol(string file) { file = path + file; char *filename = &file[0u]; std::ifstream POOOOOOOOOOOOOOOP(filename); std::stringstream bitch_ass_fuck_ass_i_farted; if (!POOOOOOOOOOOOOOOP.is_open()) { return ""; } uint32_t magic = 5381; char c; while (POOOOOOOOOOOOOOOP.get(c)) { magic = ((magic << 5) + magic) + c; } bitch_ass_fuck_ass_i_farted << std::hex << std::setw(8) << std::setfill('0') << magic; return bitch_ass_fuck_ass_i_farted.str(); }


//Network



string ip = "127.0.0.1"; int PORT = 0;

#if not defined(DISABLE_NETWORK)

sf::TcpSocket socketG; sf::SocketSelector selector; //no idea how this works
sf::TcpListener listener; std::vector<sf::TcpSocket*> clients;

sf::Packet CurrentPacket; uint8_t CurrentPacket_header;

#endif

//Sprites
#define sprite_size 0x80