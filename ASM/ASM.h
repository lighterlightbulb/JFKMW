#pragma once
//something something this fucking sucks

struct ASM_RAM
{
	ASM_RAM() {}

	uint_fast8_t RAM[RAM_Size];
};

bool asm_loaded = false;

ASM_RAM ServerRAM;
ASM_RAM ServerRAM_D;
ASM_RAM ServerRAM_old;


class JFKASM
{
public:
	uint_fast32_t x, y, a, temp_, temp__;
	uint_fast8_t access_size = 1;
	//int client_access = 0;

	bool flags[4];

	uint_fast8_t rom[rom_asm_size]; //This is basically a read-only memory.
	bool crashed = false;

	void CMP()
	{
		for (uint_fast8_t i = 0; i < 4; i++)
		{
			flags[i] = false;
		}

		if (temp_ == temp__) { flags[0] = true; }
		if (temp_ != temp__) { flags[1] = true; }
		if (temp_ >= temp__) { flags[2] = true; }
		if (temp_ <= temp__) { flags[3] = true; }

	}
	void start_JFK_thread(int pointer = 0x000000) //This runs the ASM.
	{

		string broke_reason = "invalid opcode";
		while (pointer < rom_asm_size)
		{
			uint_fast8_t opcode = rom[pointer];
			//cout << red << "[ASM] Opcode " << hex << int(opcode) << white << endl;

			pointer += 1;
			switch (opcode) {

			/*
			
				Note : can someone help make this not look so bad? jesus christ

				LDA/X/Y (Value)
			*/
			case 0xA0:
			case 0xA2:
			case 0xA9:
				temp_ = 0;
				for (int i = 0; i < access_size; i++) {
					temp_ += rom[pointer + i] * int(pow(2, i * 8));
				}

				pointer += access_size;

				if (opcode == 0xA9) { a = temp_; } if (opcode == 0xA0) { x = temp_; } if (opcode == 0xA2) { y = temp_; }

				break;
			/*
				LDA/X/Y (RAM)
			*/
			case 0xA5:
			case 0xA6:
			case 0xA4:
			case 0xAD:
			case 0xAE:
			case 0xAC:
				temp_ = Get_Ram(rom[pointer] + (opcode < 0xAA ? 0 : rom[pointer + 1] * 256), access_size);
				pointer += (opcode < 0xAA ? 1 : 2);

				if (opcode == 0xAD) { a = temp_; } if (opcode == 0xAE) { x = temp_; } if (opcode == 0xAC) { y = temp_; }
				break;
			/*
				STA/X/Y (Value)
			*/
			case 0x85:
			case 0x86:
			case 0x84:
			case 0x8D:
			case 0x8E:
			case 0x8C:
				if (opcode == 0x8D) { temp_ = a; } if (opcode == 0x8E) { temp_ = x; } if (opcode == 0x8C) { temp_ = y; }
				Write_To_Ram(rom[pointer] + rom[pointer + 1] * 256, temp_, access_size);
				pointer += (opcode < 0x8A ? 1 : 2);
				break;
			/*
				STA $xx/$xxxx,x
			*/
			case 0x9D:
				Write_To_Ram((rom[pointer] + rom[pointer + 1] * 256) + x, a, access_size);
				pointer += 2;
				break;
			/*
				STA $xxxx,y
			*/
			case 0x99:
				Write_To_Ram((rom[pointer] + rom[pointer + 1] * 256) + y, a, access_size);
				pointer += 2;
				break;
			/*
				LDA $xxxx,x
			*/
			case 0xBD:
				temp_ = Get_Ram((rom[(pointer)] + rom[pointer + 1] * 256) + x, access_size);
				pointer += 2;

				a = temp_;
				break;

			/*
				LDA $xxxx,y
			*/
			case 0xB9:
				temp_ = Get_Ram((rom[(pointer)] + rom[pointer + 1] * 256) + y, access_size);
				pointer += 2;

				a = temp_;
				break;

			/*
				CLC/SEC
			*/
			case 0x18:
			case 0x38:
				break;
			/*
				ADC/SBC (RAM)
			*/
			case 0xED:
			case 0x6D:
				temp_ = Get_Ram(rom[pointer] + rom[pointer + 1] * 256, access_size); pointer += 2;
				if (opcode == 0xED) { a += temp_; }
				else { a -= temp_; }
				break;
			/*
				ADC/SBC (VALUE)
			*/
			case 0x69:
			case 0xE9:
				temp_ = 0;
				for (int i = 0; i < access_size; i++) {
					temp_ += rom[pointer + i] * int(pow(2, i * 8));
				}

				pointer += access_size;

				if (opcode == 0x69) { a += temp_; }
				else { a -= temp_; }
				break;
			/*
				INC A/X/Y
			*/
			case 0x1A:
				a += 1; break;
			case 0xE8:
				x += 1; break;
			case 0xC8:
				y += 1; break;
			/*
				DEC A/X/Y
			*/
			case 0x3A:
				a -= 1; break;
			case 0xCA:
				x -= 1; break;
			case 0x88:
				y -= 1; break;
			/*
				TA X/Y TX A/Y TY A/X
			*/
			case 0xAA: x = a; break;
			case 0xA8: y = a; break;
			case 0x8A: a = x; break;
			case 0x9B: y = x; break;
			case 0x98: a = y; break;
			case 0xBB: x = y; break;
			/*
				CMP/X/Y (Value)
			*/
			case 0xC9:
			case 0xE0:
			case 0xC0:
				temp__ = 0;
				for (int i = 0; i < access_size; i++) {
					temp__ += rom[pointer + i] * int(pow(2, i * 8));
				}
				if (opcode == 0xC9) { temp_ = a; CMP(); }
				if (opcode == 0xE0) { temp_ = x; CMP(); }
				if (opcode == 0xC0) { temp_ = y; CMP(); }

				pointer += access_size;
				break;
			/*
				EOR
			*/
			case 0x49:
				temp__ = 0;
				for (int i = 0; i < access_size; i++) {
					temp__ += rom[pointer + i] * int(pow(2, i * 8));
				}


				a = uint_fast32_t(int_fast8_t(a)*-1);

				pointer += access_size;
				break;
			/*
				AND
			*/
			case 0x29:
				temp__ = 0;
				for (int i = 0; i < access_size; i++) {
					temp__ += rom[pointer + i] * int(pow(2, i * 8));
				}

				a &= temp__;

				pointer += access_size;
				break;
			/*
				BRA/BEQ/BNE/BCS/BCC
			*/
			case 0x80:
			case 0xF0:
			case 0xD0:
			case 0xB0:
			case 0x90:
				pointer += 1;
				if (opcode == 0xF0 && !flags[0]) { break; }
				if (opcode == 0xD0 && !flags[1]) { break; }
				if (opcode == 0xB0 && !flags[2]) { break; }
				if (opcode == 0x90 && !flags[3]) { break; }

				pointer += int_fast8_t(rom[pointer-1]);
				break;
			/*
				REP/SEP
			*/
			case 0xC2:
				access_size = rom[pointer] / 16;
				pointer += 1;
				break;
			case 0xE2:
				access_size = 1;
				pointer += 1;
				break;
			/*
				ASL
			*/
			case 0x0A:
				a = a << 1;
				break;
			/*
				LSR
			*/
			case 0x4A:
				a = a >> 1;
				break;
			/*
				NOP
			*/
			case 0xEA:
				break;
			/*
				RTL/RTS
			*/
			case 0x60:
			case 0x6B:
				return;
			/*
			unknown opcode
			*/
			default:
				crashed = true;
				cout << red << "[ASM] Crash at pointer 0x" << hex << pointer << " : " << broke_reason << endl; 
				cout << "Information about crash (probably won't help much, you probably did something wrong)" << endl;
				cout << "Current opcode : " << int(opcode) << endl;
				cout << "A, X, Y = " << a << ", " << x << ", " << y << endl;
				cout << "Flags = ";
				for (int i = 0; i < 8; i++)
				{
					cout << flags[i];
				}
				cout << white << endl;
				return;
			}

		}
	}

	void load_asm(std::string file, int offset_pointer = 0x000000)
	{
		file = path + file;
		std::ifstream input(file, std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

		int current_byte = 0;
		for (auto &v : buffer) {
			rom[offset_pointer + current_byte] = uint8_t(v); current_byte += 1;
			
		}
		//cout << red << "[ASM] Loaded " << file << " to offset " << hex << offset_pointer << ", " << dec << current_byte << " bytes loaded. " << white << endl;
	}

	void Write_To_Ram(uint_fast16_t pointer, uint_fast32_t value, uint_fast8_t size = 1)
	{
		for (uint_fast8_t i = 0; i < size; i++) {
			ServerRAM.RAM[pointer + i] = uint_fast8_t(value >> (i * 8));
		}
	}

	uint_fast32_t Get_Ram(uint_fast16_t pointer, uint_fast8_t size = 1)
	{
		uint_fast32_t temp = 0;
		for (uint_fast8_t i = 0; i < size; i++) {
			temp += ServerRAM.RAM[pointer + i] << (i * 8);
		}
		return temp;
	}


	//ASM.Write_To_Ram(0x1DF9, 0x1);

	void Reset_ASM_Variables_Server() //This only resets some of the important variables.
	{
		//cout << red << "[ASM] Reset variables." << white << endl;
		Write_To_Ram(0x1DFC, 0, 1);
		Write_To_Ram(0x1DF9, 0, 1);

	}

};


JFKASM ASM;


void Sync_Server_RAM(bool compressed = false)
{
	if (!compressed)
	{
		for (uint_fast16_t i = 0; i < RAM_Size; i++)
		{
			CurrentPacket >> ServerRAM.RAM[i];
		}
	}
	else
	{
		uint_fast16_t entries = 0;
		CurrentPacket >> entries;
		for (uint_fast16_t i = 0; i < entries; i++)
		{
			uint_fast16_t pointer;
			uint_fast8_t new_value;
			CurrentPacket >> pointer;
			CurrentPacket >> new_value;
			ServerRAM.RAM[pointer] = new_value;
		}

		for (uint_fast16_t i = 0; i < 0x200; i++)
		{
			CurrentPacket >> ServerRAM.RAM[0x200 + i];
		}
		for (uint_fast16_t i = 0; i < 0x200; i++)
		{
			CurrentPacket >> ServerRAM.RAM[0x2100 + i];
			CurrentPacket >> ServerRAM.RAM[0x2280 + i];
		}
	}
	//cout << red << "received ram" << white << endl;
}

void Push_Server_RAM(bool compress = false)
{
	if (!compress)
	{
		for (uint_fast16_t i = 0; i < RAM_Size; i++)
		{
			CurrentPacket << ServerRAM_D.RAM[i];
		}
	}
	else
	{
		uint_fast16_t entries = 0;
		for (uint_fast16_t i = 0; i < RAM_Size; i++)
		{
			if ((i < 0x200 || i > 0x400) && (i < 0x2100 || i > 0x2400))
			{
				if (ServerRAM_D.RAM[i] != ServerRAM_old.RAM[i])
				{
					entries += 1; //you stupid //no i not //whats 9 + 10 //twenty one.
				}
			}
		}

		CurrentPacket << entries;

		for (uint_fast16_t i = 0; i < RAM_Size; i++)
		{

			if ((i < 0x200 || i > 0x400) && (i < 0x2100 || i > 0x2400))
			{
				if (ServerRAM_D.RAM[i] != ServerRAM_old.RAM[i])
				{
					CurrentPacket << i; CurrentPacket << ServerRAM_D.RAM[i];
				}
			}
		}

		//do not compress OAm please do not let's send the entire thing

		for (uint_fast16_t i = 0; i < 0x200; i++)
		{
			CurrentPacket << ServerRAM_D.RAM[0x200 + i];
		}

		//Also not sprites either lol
		for (uint_fast16_t i = 0; i < 0x200; i++)
		{
			CurrentPacket << ServerRAM.RAM[0x2100 + i];
			CurrentPacket << ServerRAM.RAM[0x2280 + i];
		}
		
	}


	//cout << red << "[ASM] Pushed SE-RAM to packet." << white << endl;
}

void Set_Server_RAM()
{
	memcpy(&ServerRAM_old.RAM, &ServerRAM_D.RAM, RAM_Size * sizeof(uint_fast8_t));
}