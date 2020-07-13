#pragma once

void ResetDMAandHDMA()
{
	//Disable all HDMA and DMA channels.
	ServerRAM.RAM[0x420C] = 0;
	ServerRAM.RAM[0x420B] = 0;
}

void ProcessHDMA()
{
	/*
		Reset Horizontal Tables
	*/
	for (uint_fast8_t i = 0; i < 224; i++)
	{
		layer1_shift[i] = 0;
	}

	for (uint_fast8_t c = 0; c < 8; c++)
	{
		uint_fast8_t channel = c << 4;
		if ((ServerRAM.RAM[0x420C] >> c) & 1) //This HDMA channel is enabled
		{
			
			uint_fast8_t mode = ServerRAM.RAM[0x4300 + channel];
			uint_fast8_t size = mode + 1;
			uint_fast8_t reg = ServerRAM.RAM[0x4301 + channel];
			uint_fast32_t bank = ServerRAM.RAM[0x4302 + channel] + (ServerRAM.RAM[0x4303 + channel] << 8) + (ServerRAM.RAM[0x4304 + channel] << 16);
			uint_fast16_t i = 0;
			uint_fast16_t scanline = 0;

			while (scanline < 224)
			{
				uint_fast8_t scanlines = ServerRAM.RAM[bank + i];
				if (scanlines == 0)
				{
					break;
				}
				else
				{
					for (uint_fast8_t l = 0; l < scanlines; l++)
					{
						int value = ServerRAM.RAM[bank + 1 + i] + (size > 2 ? (ServerRAM.RAM[bank + 2 + i] << 8) : 0);
						if (value >= 0x8000) { value = -(0x10000 - value); }

						if (scanline < 224)
						{
							if (reg == 0x0F)
							{
								layer1_shift[scanline] = int_fast16_t(value);
							}
						}
						scanline++;
					}
					i += size;
				}
			}

		}
	}
}