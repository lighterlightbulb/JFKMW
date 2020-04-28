#pragma once

void spawnSpriteJFKMarioWorld(uint_fast8_t sprite_num, uint_fast8_t new_state, uint_fast16_t x, uint_fast16_t y, bool is_lua)
{
	for (uint_fast8_t i = 0; i < 128; i++)
	{
		if (ServerRAM.RAM[0x2000 + i] == 0)
		{
			ServerRAM.RAM[0x2000 + i] = new_state;
			ServerRAM.RAM[0x2080 + i] = sprite_num;
			ServerRAM.RAM[0x2F80 + i] = 0;

			ServerRAM.RAM[0x2100 + i] = uint_fast8_t(x & 0xFF);
			ServerRAM.RAM[0x2180 + i] = uint_fast8_t(x >> 8);
			ServerRAM.RAM[0x2200 + i] = 0;

			ServerRAM.RAM[0x2280 + i] = uint_fast8_t(y & 0xFF);
			ServerRAM.RAM[0x2300 + i] = uint_fast8_t(y >> 8);
			ServerRAM.RAM[0x2380 + i] = 0;

			ServerRAM.RAM[0x2400 + i] = 0;
			ServerRAM.RAM[0x2480 + i] = 0;

			sprite_is_lua[i] = is_lua;
			return;
		}
	}
}