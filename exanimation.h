#pragma once


void process_ex_animation()
{
	if (!networking || (networking && !isClient))
	{
		//Bruh crap
		for (int i = 0; i < 4; i++)
		{
			for (int b = 0; b < 32; b++)
			{
				ServerRAM.RAM[VRAM_Location + (32 * 0x60) + (i * 32) + b] = ServerRAM.RAM[VRAM_Location + 0x8000 + (0xC0 * 32) + b + (i * 32) + (((global_frame_counter / 8) % 4) * 0x10 * 32)];
			}
		}

		//? Question block
		for (int i = 0; i < 4; i++)
		{
			for (int b = 0; b < 32; b++)
			{
				ServerRAM.RAM[VRAM_Location + (32 * 0x58) + (i * 32) + b] = ServerRAM.RAM[VRAM_Location + 0x8000 + (0xB4 * 32) + b + (i * 32)];
			}
		}

		//Muncher
		for (int i = 0; i < 4; i++)
		{
			for (int b = 0; b < 32; b++)
			{
				ServerRAM.RAM[VRAM_Location + (32 * 0x5C) + (i * 32) + b] = ServerRAM.RAM[VRAM_Location + 0x8000 + b + ((0xF8 + 0x40) * 32) + (i * 32) + ((global_frame_counter / 8) % 2) * 0x80];
			}
		}

		//L.A Coin
		for (int i = 0; i < 4; i++)
		{
			for (int b = 0; b < 32; b++)
			{
				ServerRAM.RAM[VRAM_Location + (32 * 0x6C) + (i * 32) + b] = ServerRAM.RAM[VRAM_Location + 0x8000 + (0xCC * 32) + b + (i * 32) + (((global_frame_counter / 8) % 4) * 0x10 * 32)];
			}
		}
	}
}