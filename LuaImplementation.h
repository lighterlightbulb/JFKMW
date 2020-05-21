#pragma once

/*
lua integration for jfk mario world
*/

// Lua is written in C, so compiler needs to know how to link its libraries

bool lua_loaded = false;

extern "C" {
#include "lua.hpp"
}

lua_State* LUA_STATE;

void lua_print(string text)
{
	cout << lua_color << "[Lua] " << text << white << endl;
}

//lua functions to bind to jfk mario world.
static int lua_write(lua_State* L) {
	const char* str = lua_tostring(L, 1); // get function argument
	lua_print(str);
	return 0; // nothing to return!
}

static int lua_write_ram(lua_State* L) {
	uint_fast32_t n = (uint_fast32_t)lua_tonumber(L, 1);
	uint_fast32_t p = (uint_fast16_t)lua_tointeger(L, 2);
	uint_fast8_t s = (uint_fast8_t)lua_tointeger(L, 3);

	ASM.Write_To_Ram(p, n, s);
	return 0; // nothing to return!
}


extern "C" int lua_get_ram(lua_State* L)
{
	uint_fast32_t p = (uint_fast16_t)lua_tointeger(L, 1); // First argument
	uint_fast8_t s = (uint_fast8_t)lua_tointeger(L, 2); // First argument

	double result = double(int_fast32_t(ASM.Get_Ram(p, s)));

	lua_pushnumber(L, result);
	//lua_pushinteger(L, result);

	return 1; // Count of returned values
}

static int lua_spawn_sprite(lua_State* L) {
	uint_fast8_t n = (uint_fast8_t)lua_tointeger(L, 1); // First argument
	uint_fast8_t s = (uint_fast8_t)lua_tointeger(L, 2); // First argument
	uint_fast16_t x = (uint_fast16_t)lua_tointeger(L, 3); // First argument
	uint_fast16_t y = (uint_fast16_t)lua_tointeger(L, 4); // First argument
	uint_fast8_t direction = (uint_fast8_t)lua_tointeger(L, 5); // First argument
	bool is_l = lua_toboolean(L, 6);

	lua_pushnumber(L, spawnSpriteJFKMarioWorld(n, s, x, y, direction, is_l));


	return 1;
}

static int draw_to_oam(lua_State* L)
{
	uint_fast8_t sprite_index = (uint_fast8_t)lua_tonumber(L, 1);
	uint_fast8_t tile = (uint_fast8_t)lua_tonumber(L, 2);
	uint_fast8_t size = (uint_fast8_t)lua_tonumber(L, 3);
	uint_fast8_t angle = (uint_fast8_t)lua_tonumber(L, 4);
	int offset_x = (int)lua_tonumber(L, 5);
	int offset_y = (int)lua_tonumber(L, 6);

	uint_fast8_t pal = (uint_fast8_t)lua_tonumber(L, 7);
	uint_fast16_t oam_index = 0;
	while (oam_index < 0x400)
	{
		if (ServerRAM.RAM[0x200 + oam_index] == 0 && ServerRAM.RAM[0x206 + oam_index] == 0) //Founmd a empty OAM slot
		{
			break;
		}
		oam_index += 8;
	}


	ServerRAM.RAM[0x200 + oam_index] = tile;
	ServerRAM.RAM[0x201 + oam_index] = size;


	uint_fast16_t sprite_x_position = uint_fast16_t(int(offset_x + ServerRAM.RAM[0x2100 + sprite_index] + int_fast8_t(ServerRAM.RAM[0x2180 + sprite_index]) * 256));
	uint_fast16_t sprite_y_position = uint_fast16_t(int(offset_y + ServerRAM.RAM[0x2280 + sprite_index] + int_fast8_t(ServerRAM.RAM[0x2300 + sprite_index]) * 256));


	ServerRAM.RAM[0x202 + oam_index] = sprite_x_position;
	ServerRAM.RAM[0x203 + oam_index] = sprite_x_position >> 8;

	ServerRAM.RAM[0x204 + oam_index] = sprite_y_position;
	ServerRAM.RAM[0x205 + oam_index] = sprite_y_position >> 8;

	ServerRAM.RAM[0x206 + oam_index] = pal;
	ServerRAM.RAM[0x207 + oam_index] = angle;

	return 0;
}


static int draw_to_oam_direct(lua_State* L)
{
	uint_fast8_t tile = (uint_fast8_t)lua_tonumber(L, 1);
	uint_fast8_t size = (uint_fast8_t)lua_tonumber(L, 2);
	uint_fast8_t angle = (uint_fast8_t)lua_tonumber(L, 3);
	uint_fast16_t sprite_x_position = (uint_fast16_t)lua_tonumber(L, 4);
	uint_fast16_t sprite_y_position = (uint_fast16_t)lua_tonumber(L, 5);
	uint_fast8_t pal = (uint_fast8_t)lua_tonumber(L, 6);
	uint_fast16_t oam_index = 0;
	while (oam_index < 0x400)
	{
		if (ServerRAM.RAM[0x200 + oam_index] == 0 && ServerRAM.RAM[0x206 + oam_index] == 0) //Founmd a empty OAM slot
		{
			break;
		}
		oam_index += 8;
	}

	ServerRAM.RAM[0x200 + oam_index] = tile;
	ServerRAM.RAM[0x201 + oam_index] = size;

	ServerRAM.RAM[0x202 + oam_index] = sprite_x_position;
	ServerRAM.RAM[0x203 + oam_index] = sprite_x_position >> 8;

	ServerRAM.RAM[0x204 + oam_index] = sprite_y_position;
	ServerRAM.RAM[0x205 + oam_index] = sprite_y_position >> 8;

	ServerRAM.RAM[0x206 + oam_index] = pal;
	ServerRAM.RAM[0x207 + oam_index] = angle;

	return 0;
}



static int lua_bitand(lua_State* L) {
	uint_fast8_t a = (uint_fast8_t)lua_tointeger(L, 1); 
	uint_fast8_t b = (uint_fast8_t)lua_tointeger(L, 2);
	
	uint_fast8_t result = 0;
	uint_fast8_t bitval = 1;
	while (a > 0 && b > 0)
	{
		if ((a % 2) == 1 && (b % 2) == 1)
		{
			result = result + bitval;
		}
		bitval = bitval << 1;
		a = a >> 1;
		b = b >> 1;
	}
	lua_pushnumber(L, result);
	return 1;
}

static int drawtohud(lua_State* L)
{
	uint_fast8_t tile = (uint_fast8_t)lua_tonumber(L, 1) & 0x7F;
	uint_fast8_t prop = (uint_fast8_t)lua_tonumber(L, 2) & 0x0F;
	uint_fast8_t x = (uint_fast8_t)lua_tonumber(L, 3) & 0x1F;
	uint_fast8_t y = (uint_fast8_t)lua_tonumber(L, 4) & 0x1F;

	ServerRAM.RAM[0x1B800 + ((x % 32) + (y * 32))*2] = tile & 0x7F;
	ServerRAM.RAM[0x1B801 + ((x % 32) + (y * 32))*2] = prop;
	return 0;
}

/* functions end */

void lua_connect_functions(lua_State* L)
{
	//lua_print("Connected functions to 0x" + int_to_;
	lua_pushcfunction(L, lua_write); lua_setglobal(L, "mario_print");
	lua_pushcfunction(L, lua_write_ram); lua_setglobal(L, "asm_write");
	lua_pushcfunction(L, lua_spawn_sprite); lua_setglobal(L, "spawn_sprite");
	lua_pushcfunction(L, draw_to_oam); lua_setglobal(L, "draw_to_oam");
	lua_pushcfunction(L, draw_to_oam_direct); lua_setglobal(L, "draw_to_oam_direct");
	lua_pushcfunction(L, lua_bitand); lua_setglobal(L, "bitand");
	lua_pushcfunction(L, drawtohud); lua_setglobal(L, "draw_to_hud");
	lua_register(L, "asm_read", lua_get_ram);


}



/*
	LUA General
*/


void lua_loadfile(string file)
{
	if (LUA_STATE)
	{
		lua_close(LUA_STATE);
	}

	LUA_STATE = luaL_newstate();
	luaL_openlibs(LUA_STATE); // load default Lua libs


	int ret = luaL_dofile(LUA_STATE, (path + file).c_str());
	if (ret != 0)
	{
		lua_print("Error occurred when calling luaL_dofile()");
		lua_print("Error: " + string(lua_tostring(LUA_STATE, -1)));
		lua_close(LUA_STATE);
		return;
	}



	//main connectors back to jfk mario world.
	lua_connect_functions(LUA_STATE);

	lua_print("loaded " + path + file);
}

void lua_run_init()
{
	lua_getglobal(LUA_STATE, "Init");
	lua_pcall(LUA_STATE, 0, 0, 0); // run script
}

void lua_run_main()
{
	lua_getglobal(LUA_STATE, "Main");
	lua_pcall(LUA_STATE, 0, 0, 0); // run script
}