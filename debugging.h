#pragma once

void dump_ram()
{
	ofstream fp;
	fp.open(path + "ramdump.bin", ios::out | ios::binary);
	fp.write((char*)RAM, sizeof(RAM));
	cout << red << "[ASM] RAM has been dumped to ramdump.bin" << white << endl;
}

void dump_level_data()
{

}

void debugging_functions()
{
	/*
	Debug input checks
	*/
	bool stat = false;
	if (!Chatting)
	{
		stat = state[input_settings[11]];
		if (stat != pressed_hide) {
			pressed_hide = stat;
			if (stat) {
				drawHud = !drawHud;
			}
		}

		stat = state[input_settings[12]];
		if (stat != pressed_diag) {
			pressed_diag = stat;
			if (stat) {
				drawDiag = !drawDiag;
			}
		}

		stat = state[input_settings[13]];
		if (stat != pressed_drawl1) {
			pressed_drawl1 = stat;
			if (stat) {
				drawL1 = !drawL1;
			}
		}

		stat = state[input_settings[14]];
		if (stat != pressed_bg) {
			pressed_bg = stat;
			if (stat) {
				drawBg = !drawBg;
			}
		}

		stat = state[input_settings[15]];
		if (stat != pressed_drawsprites) {
			pressed_drawsprites = stat;
			if (stat) {
				drawSprites = !drawSprites;
			}
		}

		stat = state[input_settings[16]];
		if (stat != pressed_dumpram) {
			pressed_dumpram = stat;
			if (stat) {
				dump_ram();
			}
		}
	}
}