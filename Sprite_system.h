#pragma once
/*
0x2000 - Sprite Status :
0 - Non existant
1 - Main
2 - Grabbable (Not grabbed)
3 - Grabbable (Grabbed by a player)
4 - Kicked


0x2080 - Sprite Number
0x2100 - Sprite X Position (L)
0x2180 - Sprite X Position (H)
0x2200 - Sprite X Position (F)
0x2280 - Sprite Y Position (L)
0x2300 - Sprite Y Position (H)
0x2380 - Sprite Y Position (F)
0x2400 - Sprite X Speed
0x2480 - Sprite Y Speed
0x2500 - Sprite Size X
0x2580 - Sprite Size Y
0x2600 - Sprite Flags HSGO---
0x2680 - Sprite Direction
0x2700 - Sprite interacing with... (player number in hex)
0x2780 - Sprite block flags

0x2800 - Sprite is lua/asm type
0x2E80 - Unused, extra property for grabbed sprites, which props/palettes it uses
0x2F00 - Unused, extra property for grabbed sprites, which tile it uses
0x2F80 - Sprite Initialized


Flags :
H = Hurts
S = Solid
G = Gravity
T = Jumpable
UDLR = Solid on up/down/left/right
*/



class sprite_system
{
public:
	double x, y;
	lua_State* SPR_STATE[128];

	void process_sprite_logic(uint_fast8_t entry = 0)
	{
		x = double(ServerRAM.RAM[0x2100 + entry] + double(ServerRAM.RAM[0x2180 + entry]) * 256.0) + double(ServerRAM.RAM[0x2200 + entry]) / 256.0;
		y = double(ServerRAM.RAM[0x2280 + entry] + double(ServerRAM.RAM[0x2300 + entry]) * 256.0) + double(ServerRAM.RAM[0x2380 + entry]) / 256.0;

		if (ServerRAM.RAM[0x2600 + entry] & 0b100000) //if gravity bit is on
		{
			if (int_fast8_t(ServerRAM.RAM[0x2480 + entry]) > -82)
			{
				ServerRAM.RAM[0x2480 + entry] -= 3;
			}
		}


		double xMove = double(double(int_fast8_t(ServerRAM.RAM[0x2400 + entry]))*16)/256.0;
		double yMove = double(double(int_fast8_t(ServerRAM.RAM[0x2480 + entry]))*16)/256.0;

		double x_size = double(ServerRAM.RAM[0x2500 + entry]);
		double y_size = double(ServerRAM.RAM[0x2580 + entry]);

		
		if (ServerRAM.RAM[0x2600 + entry] & 0b1000000) //if solid bit is on
		{
			ServerRAM.RAM[0x2780 + entry] = 0;
			if (!Move(xMove, 0.0, x_size, y_size))
			{
				//ServerRAM.RAM[0x2680 + entry] *= -1;
				ServerRAM.RAM[0x2780 + entry] |= 0b00000001;
			}

			if (!Move(0.0, yMove, x_size, y_size))
			{
				//ServerRAM.RAM[0x2480 + entry] = 0;
				ServerRAM.RAM[0x2780 + entry] |= 0b00000010;
			}
		}
		else
		{
			ServerRAM.RAM[0x2780 + entry] = 0;
			x += xMove;
			y += yMove;
		}

		if (y < -64.0)
		{
			for (int i = 0; i < 32; i++)
			{
				ServerRAM.RAM[0x2000 + entry + (i * 128)] = 0;
			}
			return;
		}

		ServerRAM.RAM[0x2100 + entry] = int_fast8_t(int(x) % 256); ServerRAM.RAM[0x2180 + entry] = int_fast8_t(int(x) / 256); ServerRAM.RAM[0x2200 + entry] = int_fast8_t(int(x*256.0) % 256);
		ServerRAM.RAM[0x2280 + entry] = int_fast8_t(int(y) % 256); ServerRAM.RAM[0x2300 + entry] = int_fast8_t(int(y) / 256); ServerRAM.RAM[0x2380 + entry] = int_fast8_t(int(y*256.0) % 256);
	}

	/*
		Shitty Movement Code
	*/
	bool Move(double xMove, double yMove, double x_size, double y_size)
	{
		bool finna_return = true;
		double NewPositionX = x + xMove;
		double NewPositionY = y + yMove;

		int_fast16_t PosXBlock = int_fast16_t(NewPositionX / 16); 
		int_fast16_t PosYBlock = int_fast16_t(NewPositionY / 16);
		int_fast16_t startX = PosXBlock - TotalBlocksCollisionCheckSprite - int(x_size / 16.0);
		if (startX < 0) { startX = 0; }
		int_fast16_t startY = PosYBlock - TotalBlocksCollisionCheckSprite - int(y_size / 16.0);
		if (startY < 0) { startY = 0; }
		for (int_fast16_t xB = startX; xB < PosXBlock + TotalBlocksCollisionCheckSprite + int(x_size/16.0); xB++)
		{
			for (int_fast16_t yB = startY; yB < PosYBlock + TotalBlocksCollisionCheckSprite + int(y_size / 16.0); yB++)
			{
				map16_handler.update_map_tile(xB, yB);


				double BelowBlock = double(yB * 16) - y_size;
				double AboveBlock = double(yB * 16) + 16.0;
				double RightBlock = double(xB * 16) + 16.0;
				double LeftBlock = double(xB * 16) - x_size;

				bool checkLeft = map16_handler.logic[3];
				bool checkRight = map16_handler.logic[2];
				bool checkBottom = map16_handler.logic[1];
				bool checkTop = map16_handler.logic[0];

				if (NewPositionX < RightBlock && NewPositionX > LeftBlock && NewPositionY < AboveBlock && NewPositionY > BelowBlock)
				{

					if (xMove < 0.0 && checkRight)
					{
						if (NewPositionX < RightBlock && NewPositionX > RightBlock - bounds_x)
						{
							NewPositionX = RightBlock;
							finna_return = false;
						}
					}
					if (xMove > 0.0 && checkLeft)
					{
						if (NewPositionX > LeftBlock && NewPositionX < LeftBlock + bounds_x)
						{
							NewPositionX = LeftBlock;
							finna_return = false;
						}
					}
					if (yMove < 0.0 && checkTop)
					{
						if (NewPositionY < AboveBlock && NewPositionY > AboveBlock - bounds_y)
						{
							NewPositionY = AboveBlock;
							finna_return = false;
						}

					}
					if (yMove > 0.0 && checkBottom)
					{
						if (NewPositionY > BelowBlock && NewPositionY < BelowBlock + bounds_y)
						{
							NewPositionY = BelowBlock;
							finna_return = false;
						}
					}

				}
			}
		}

		x = NewPositionX;
		y = NewPositionY;
		return finna_return;
	}

	void call_sprite_lua(int index)
	{
		lua_getglobal(SPR_STATE[index], "Main");
		lua_pushinteger(SPR_STATE[index], index);
		lua_call(SPR_STATE[index], 1, 0); // run script
	}

	void init_sprite_lua(int index, string file)
	{
		if (SPR_STATE[index])
		{
			lua_close(SPR_STATE[index]);
		}

		SPR_STATE[index] = luaL_newstate();
		luaL_openlibs(SPR_STATE[index]); // load default Lua libs


		int ret = luaL_dofile(SPR_STATE[index], (path + file).c_str());
		if (ret != 0)
		{
			lua_print("Error occurs when calling luaL_dofile() Hint Machine 0x%x\n" + ret);
			lua_print("Error: " + std::string(lua_tostring(SPR_STATE[index], -1)));
			return;
		}

		lua_connect_functions(SPR_STATE[index]);
		//cout << purple << "[Sprites] Initialized Sprite Slot " << int(index) << " " << path << file << white << endl;
	}

	void process_all_sprites()
	{
		for (uint_fast32_t i = 0; i < 128; i++)
		{
			if (ServerRAM.RAM[0x2000 + i] != 0) //If sprite exists..
			{
				if (ServerRAM.RAM[0x2F80 + i] == 0)
				{
					if (ServerRAM.RAM[0x2800 + i])
					{
						init_sprite_lua(int(i), "Code/Sprites/" + int_to_hex(ServerRAM.RAM[0x2080 + i], true) + ".lua");
						lua_getglobal(SPR_STATE[i], "Init");
						lua_pushinteger(SPR_STATE[i], i);
						lua_call(SPR_STATE[i], 1, 0); // run script
					}
					ServerRAM.RAM[0x2F80 + i] = 1;
				}
				if (ServerRAM.RAM[0x2800 + i])
				{
					call_sprite_lua(i);
				}
				else
				{
					ASM.load_asm("Code/Sprites/" + to_string(ServerRAM.RAM[0x2080 + i]) + ".jasm", location_temp_sprite_asm);
					ASM.x = i;
					ASM.start_JFK_thread(location_temp_sprite_asm);
					if (ASM.crashed)
					{
						ServerRAM.RAM[0x2000 + i] = 0;
					}

				
				}
				process_sprite_logic(uint_fast8_t(i));
			}
		}
	}
};


sprite_system Sprites;