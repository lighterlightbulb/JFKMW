#pragma once
/*
	This should only be done in the client, if there's need for, a better implementation will be done later.
	Why it's not done on the server : It would be laggy
*/
void process_ex_animation()
{
	if (!networking || (networking && isClient))
	{
		/*
			0x3D00-0x3DFF Palette, low b
			0x3E00-0x3EFF Palette, high b
		*/
		uint_fast8_t b = global_frame_counter;
		if ((global_frame_counter & 0x1F) > 0x0F)
		{
			b = 0x10 - (global_frame_counter - 0x10);
		}
		b = b << 4;
		uint_fast16_t col = 0x3FF + ((b >> 3) << 10);
		RAM[0x3D64] = col;
		RAM[0x3E64] = col >> 8;
		//Question block (Corrected)
		memcpy(&RAM[VRAM_Location + (32 * 0x60)], &RAM[VRAM_Location + 0x8000 + (0xC0 * 32) + (((global_frame_counter >> 3) & 3) << 9)], 128);

		//Brown block
		memcpy(&RAM[VRAM_Location + (32 * 0x58)], &RAM[VRAM_Location + 0x8000 + (0xB4 * 32)], 128);

		//Turnblock def
		memcpy(&RAM[VRAM_Location + (32 * 0x6)], &RAM[VRAM_Location + 0x8000 + (0xC8 * 32)], 64);
		memcpy(&RAM[VRAM_Location + (32 * 0x16)], &RAM[VRAM_Location + 0x8000 + (0xCA * 32)], 64);

		//Flippin turnblock
		memcpy(&RAM[VRAM_Location + (32 * 0xEA)], &RAM[VRAM_Location + 0x8000 + (0xC8 * 32) + (((global_frame_counter >> 3) & 3) << 9)], 128);

		//Muncher
		memcpy(&RAM[VRAM_Location + (32 * 0x5C)], &RAM[VRAM_Location + 0x8000 + ((0xF8 + 0x40) * 32) + (((global_frame_counter >> 3) & 1) << 7)], 128);

		//Water
		memcpy(&RAM[VRAM_Location + (32 * 0x70)], &RAM[VRAM_Location + 0x8000 + (0x100 * 32) + (((global_frame_counter >> 3) & 3) << 9)], 128);

		//Lava
		memcpy(&RAM[VRAM_Location + (32 * 0x4C)], &RAM[VRAM_Location + 0x8000 + (0x8C * 32) + (((global_frame_counter >> 3) & 3) << 9)], 128);

		//L.A Coin
		memcpy(&RAM[VRAM_Location + (32 * 0x6C)], &RAM[VRAM_Location + 0x8000 + (0xCC * 32) + (((global_frame_counter >> 3) & 3) << 9)], 128);

		//On/Off Switch
		memcpy(&RAM[VRAM_Location + (32 * 0xDA)], &RAM[VRAM_Location + 0x8000 + ((0xA0 + (RAM[0x14AF] << 4)) * 32)], 128);

		//On/Off Switch Blocks
		memcpy(&RAM[VRAM_Location + (32 * 0xAC)], &RAM[VRAM_Location + 0x8000 + ((0xC8 + (RAM[0x14AF] << 5)) * 32)], 128);
		memcpy(&RAM[VRAM_Location + (32 * 0xCC)], &RAM[VRAM_Location + 0x8000 + ((0xE8 - (RAM[0x14AF] << 5)) * 32)], 128);

		//Midpoint
		memcpy(&RAM[VRAM_Location + (32 * 0x74)], &RAM[VRAM_Location + 0x8000 + (0x104 * 32) + (((global_frame_counter >> 3) & 3) << 9)], 128);
	}
}