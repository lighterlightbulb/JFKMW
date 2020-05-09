#pragma once

string GAME_VERSION = "A0.3 Slope Test 1";

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
bool need_sync_music = false;
bool kill_music = false;
uint_fast8_t my_skin = 0;
uint_fast32_t global_frame_counter = 0;

chrono::duration<double> total_time_ticks;
chrono::duration<double> latest_server_response;

unsigned int network_update_rate = 16;
unsigned int packet_wait_time = 16;
int mouse_x, mouse_y;
bool mouse_down;
int data_size_current = 0;

int rendering_device = -1;
int controller = 0;
bool fullscreen = false;
bool opengl = false;
bool networking = false;
bool renderer_accelerated = true;
bool v_sync = true;
bool force_sleep = false;

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

string int_to_hex(int T, bool add_0 = false)
{
	stringstream stream;
	if (!add_0)
	{
		stream << hex << T;
	}
	else
	{
		if(T < 16)
		{
			stream << "0";
		}
		stream << hex << T;
	}
	
	return stream.str();
}


#ifdef NDEBUG
string path = "";
#else
string path = "E:/JFKMarioWorld/x64/JFKMarioWorld/Debug/";
#endif

bool quit = false;
bool actuallyquitgame = false;
//NET
bool doing_write = false;
bool doing_read = false;
bool recent_big_change = false;
bool disconnected = false;
bool isClient = false;


//Game
bool smooth_camera = false;
double smooth_camera_speed = 0;
uint_fast8_t SelfPlayerNumber = 1;
int PlayerAmount = 0;

//Obsfucated version check
//string da_epical_function_lol() { string KylesmomistheonethatstartedthatdamncluballbecauseshesabigfatstupidbDontsayitCartmanWEEEEELLLLLDontdoitCartmanWEEELLLLLLImwarningyouOkokImgettingprettysickofhimcallingmymomaWEEELLKylemomsabitchshesabigfatbitchshesthebiggestbitchinthewholewideworldshesastupidbitchifthereeverwasabitchshesabitchtoalltheboysandgirlsSHUTYOURFUCKINGMOUTHCARTMAN = "dlroWoiraMKFJ"; int len = int(KylesmomistheonethatstartedthatdamncluballbecauseshesabigfatstupidbDontsayitCartmanWEEEEELLLLLDontdoitCartmanWEEELLLLLLImwarningyouOkokImgettingprettysickofhimcallingmymomaWEEELLKylemomsabitchshesabigfatbitchshesthebiggestbitchinthewholewideworldshesastupidbitchifthereeverwasabitchshesabitchtoalltheboysandgirlsSHUTYOURFUCKINGMOUTHCARTMAN.length()); int n = len - 1; for (int i = 0; i < (len / 2); i++) { swap(KylesmomistheonethatstartedthatdamncluballbecauseshesabigfatstupidbDontsayitCartmanWEEEEELLLLLDontdoitCartmanWEEELLLLLLImwarningyouOkokImgettingprettysickofhimcallingmymomaWEEELLKylemomsabitchshesabigfatbitchshesthebiggestbitchinthewholewideworldshesastupidbitchifthereeverwasabitchshesabitchtoalltheboysandgirlsSHUTYOURFUCKINGMOUTHCARTMAN[i], KylesmomistheonethatstartedthatdamncluballbecauseshesabigfatstupidbDontsayitCartmanWEEEEELLLLLDontdoitCartmanWEEELLLLLLImwarningyouOkokImgettingprettysickofhimcallingmymomaWEEELLKylemomsabitchshesabigfatbitchshesthebiggestbitchinthewholewideworldshesastupidbitchifthereeverwasabitchshesabitchtoalltheboysandgirlsSHUTYOURFUCKINGMOUTHCARTMAN[n]); n = n - 1; } string nigga = path + KylesmomistheonethatstartedthatdamncluballbecauseshesabigfatstupidbDontsayitCartmanWEEEEELLLLLDontdoitCartmanWEEELLLLLLImwarningyouOkokImgettingprettysickofhimcallingmymomaWEEELLKylemomsabitchshesabigfatbitchshesthebiggestbitchinthewholewideworldshesastupidbitchifthereeverwasabitchshesabitchtoalltheboysandgirlsSHUTYOURFUCKINGMOUTHCARTMAN + ".exe"; char* nigganame = &nigga[0u]; ifstream POOOOOOOOOOOOOOOP(nigganame); stringstream bitch_ass_fuck_ass_i_farted; if (!POOOOOOOOOOOOOOOP.is_open()) { return ""; } uint32_t magic = 3932; char c; while (POOOOOOOOOOOOOOOP.get(c)) { magic = ((magic << 5) + magic) + c; } bitch_ass_fuck_ass_i_farted << hex << setw(8) << setfill('0') << magic; return bitch_ass_fuck_ass_i_farted.str(); }

//Network
string ip = "127.0.0.1"; int PORT = 0;

#if not defined(DISABLE_NETWORK)

sf::TcpSocket socketG; sf::SocketSelector selector; //no idea how this works
sf::TcpListener listener; vector<sf::TcpSocket*> clients;

sf::Packet CurrentPacket; uint8_t CurrentPacket_header;

#endif

//Sprites
#define sprite_size 0x80