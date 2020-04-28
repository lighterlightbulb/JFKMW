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

inline std::ostream& lua_color(std::ostream& s)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, 9);
	return s;
}

void lua_print(std::string text)
{
	std::cout << lua_color << "[Lua] " << text << white << std::endl;
}

//lua functions to bind to jfk mario world.
static int lua_write(lua_State* L) {
	const char* str = lua_tostring(L, 1); // get function argument
	lua_print(str);
	return 0; // nothing to return!
}

static int lua_write_ram(lua_State* L) {
	uint_fast32_t n = (uint_fast32_t)lua_tonumber(L, 1);
	uint_fast16_t p = (uint_fast16_t)lua_tointeger(L, 2);
	uint_fast8_t s = (uint_fast8_t)lua_tointeger(L, 3);

	ASM.Write_To_Ram(p, n, s);
	return 0; // nothing to return!
}


extern "C" int lua_get_ram(lua_State* L)
{
	uint_fast16_t p = (uint_fast16_t)lua_tointeger(L, 1); // First argument
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

	spawnSpriteJFKMarioWorld(n, s, x, y, direction, is_l);

	return 0;
}

/* functions end */

void lua_connect_functions(lua_State* L)
{
	//lua_print("Connected functions to 0x" + int_to_;
	lua_pushcfunction(L, lua_write); lua_setglobal(L, "mario_print");
	lua_pushcfunction(L, lua_write_ram); lua_setglobal(L, "asm_write");
	lua_pushcfunction(L, lua_spawn_sprite); lua_setglobal(L, "spawn_sprite");
	lua_register(L, "asm_read", lua_get_ram);

}



/*
	LUA General
*/


void lua_loadfile(std::string file)
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
		lua_print("Error occurs when calling luaL_dofile() Hint Machine 0x%x\n" + ret);
		lua_print("Error: " + std::string(lua_tostring(LUA_STATE, -1)));
		return;
	}


	//main connectors back to jfk mario world.
	lua_connect_functions(LUA_STATE);

	lua_print("loaded " + path + file);
}

void lua_run_init()
{
	//lua_print("calling init()");
	lua_getglobal(LUA_STATE, "Init");
	lua_call(LUA_STATE, 0, 0); // run script

}

void lua_run_main()
{
	//lua_print("calling main()");
	lua_getglobal(LUA_STATE, "Main");
	lua_call(LUA_STATE, 0, 0); // run script

}