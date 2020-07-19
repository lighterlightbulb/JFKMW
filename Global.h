#pragma once

string GAME_VERSION = "0.9.0a";

#define rom_asm_size 0x008000 //32kb, 1 bank ($00:8000 to $00:FFFF)
#define location_rom_levelasm 0x008000 //this will put LevelASM on the start of the ROM, this is a SNES PC btw


/*
	These conversions will do for now, but when we expand the ROM, we have to fix these.
*/
uint_fast32_t snestopc(uint_fast32_t snes) {
	return snes - 0x8000;
}

uint_fast32_t pctosnes(uint_fast32_t pc) {
	return pc + 0x8000;
}

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

#define camBoundX 32.0
#define camBoundY 32.0

#define total_inputs 7

#define SpriteAmount 64
#define ogg_sample_rate 44100

#define player_expected_packet_size 99 //Strings apparently add 4 so we have to make sure of this so it wont crash.
#define player_name_size 11

#define MAX_L3_TILES 0x3E

#define level_ram_decay_time 40 //Server ticks before level data RAM becomes invalid to send

uint_fast16_t mapWidth = 256;
uint_fast16_t mapHeight = 32;

#define RAM_Size 0x20000
#define VRAM_Size 0x10000
#define VRAM_Location 0x10000


uint_fast16_t hdma_size[8];
uint_fast32_t palette_array[256]; //These are cached lol
uint_fast8_t VRAM[VRAM_Size];

#define top 8
#define bottom 4
#define left 2
#define right 1
#define inside 0

//DMA/DHMA
int_fast16_t layer1_shift[224];

//threads

#if not defined(DISABLE_NETWORK)
sf::Thread* thread = 0;
#endif

//ASM
bool need_sync_music = false;
bool kill_music = false;
uint_fast8_t my_skin = 0;
uint_fast32_t global_frame_counter = 0;

chrono::duration<double> total_time_ticks;
int latest_server_response;

unsigned int network_update_rate = 16;
unsigned int packet_wait_time = 16;
int mouse_x, mouse_y;
bool mouse_down;
bool mouse_down_r;
bool mouse_w_up;
bool mouse_w_down;
bool forced_scale = false;
bool experimental_features = false;
int data_size_current = 0;
int data_size_now = 0;

int rendering_device = -1;
int controller = 0;
int haptic = 0;

#define spawn_bound_x 192 //Sprite spawn bound for camera. This should be good for now.

bool fullscreen = false;
bool opengl = false;
bool networking = false;
bool renderer_accelerated = true;
bool v_sync = true;
bool force_sleep = false;

string testing_level = "";

//Stuff hud toggles here and hud stuff
bool drawHud = true;
bool drawDiag = false;
bool drawBg = true;
bool drawSprites = true;
bool drawL1 = true;

int fps_diag[128];
int ping_diag[112];
int block_diag[112];
int ram_diag[16];
int kbs_diag[112];
int blocks_on_screen = 0;

//Yep

string username = "No username";

SDL_Scancode input_settings[18] = {
	SDL_SCANCODE_S,
	SDL_SCANCODE_Z,
	SDL_SCANCODE_X,
	SDL_SCANCODE_A,
	SDL_SCANCODE_LEFT,
	SDL_SCANCODE_RIGHT,
	SDL_SCANCODE_DOWN,
	SDL_SCANCODE_UP,
	SDL_SCANCODE_RSHIFT,
	SDL_SCANCODE_RETURN,
	SDL_SCANCODE_T,
	SDL_SCANCODE_1,
	SDL_SCANCODE_2,
	SDL_SCANCODE_3,
	SDL_SCANCODE_4,
	SDL_SCANCODE_5,
	SDL_SCANCODE_6,
	SDL_SCANCODE_7
};
bool BUTTONS_GAMEPAD[10];


vector<string> split(const string &s, char delim) {
	vector<string> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

void replaceAll(string& str, const string& from, const string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

bool is_file_exist(const char* fileName)
{
	ifstream infile(fileName);
	return infile.good();
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
string path = "E:/JFKMarioWorld/Debug/";
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
bool pvp = true;
bool Chatting = false;
string Typing_In_Chat = "";
int_fast16_t CameraX, CameraY;
uint_fast8_t curr_bg = 0xFF;
uint_fast8_t screen_darken = 0;

bool showing_player_list;
bool pressed_select;
bool pressed_start;

bool smooth_camera = false;
bool midway_activated = false;
double smooth_camera_speed = 0;
uint_fast8_t SelfPlayerNumber = 1;
int PlayerAmount = 0;

uint_fast8_t death_timer[256];

//Network
string ip = "127.0.0.1"; int PORT = 0;

#if not defined(DISABLE_NETWORK)

sf::TcpSocket socketG; sf::SocketSelector selector; //no idea how this works
sf::TcpListener listener; vector<sf::TcpSocket*> clients;

uint_fast8_t CurrentPacket_header;
//TO-DO : add packet compression.
sf::Packet CurrentPacket;
#endif

//Rendering
uint_fast16_t int_res_x = 256;
uint_fast16_t int_res_y = 224;
int resolution_x = 320;
int resolution_y = 240;
uint_fast8_t scale = 1;
int sp_offset_x = 32;
int sp_offset_y = 28;


unordered_map<uint_fast32_t, SDL_Texture*> SpriteTextures;
SDL_Texture* loadSprTexture(uint_fast32_t fl)
{
	auto entry = SpriteTextures.find(fl);
	if (entry != SpriteTextures.end())
	{
		return entry->second;
	}
	return NULL;
}

void addSprTexture(uint_fast32_t fl, SDL_Texture* newTex)
{
	SpriteTextures.insert(make_pair(fl, newTex));
}

void ClearSpriteCache()
{
	if (SpriteTextures.size() > 0)
	{
		cout << yellow << "[GFX] Clearing Sprite Cache" << white << endl;
		for (unordered_map<uint_fast32_t, SDL_Texture*>::iterator it = SpriteTextures.begin(); it != SpriteTextures.end(); ++it)
		{
			SDL_DestroyTexture(it->second);
		}
		SpriteTextures.clear();
	}
}

/*

Discord Logging

*/


#if defined(_WIN32)
string discord_webhook;

void do_d_msg(string msg)
{
	time_t currentTime;
	struct tm localTime;

	time(&currentTime);                   // Get the current time
	localtime_s(&localTime, &currentTime);  // Convert the current time to the local time

	int Hour = localTime.tm_hour;
	int Min = localTime.tm_min;
	int Sec = localTime.tm_sec;

	string H, M, S;
	H = Hour < 10 ? ("0" + to_string(Hour)) : to_string(Hour);
	M = Min < 10 ? ("0" + to_string(Min)) : to_string(Min);
	S = Sec < 10 ? ("0" + to_string(Sec)) : to_string(Sec);

	msg = "[" + H + ":" + M + ":" + S + "] " + msg;
	string cmd = "curl --silent -o nul -i -H \"Accept: application/json\" -H \"Content-Type:application/json\" -X POST --data \"{\\\"content\\\": \\\"" + msg + "\\\"}\" " + discord_webhook;
	system(cmd.c_str());
	return;
}

void discord_message(string msg)
{
	if (discord_webhook != "")
	{
		sf::Thread t1(do_d_msg, msg);
		t1.launch();
		cout << lua_color << "[Logging] Thread started with \"" << msg << "\"" << white << endl;
	}
}
#else
string discord_webhook;
void discord_message(string msg)
{
}
#endif