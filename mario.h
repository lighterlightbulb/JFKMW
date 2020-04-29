#pragma once

#define P_METER_REQUIRED 0x70

class MPlayer
{
public:

	int_fast8_t SKIDDING = 0;
	int_fast8_t WALKING_DIR = -1;

	bool CROUCH = false;
	uint_fast16_t INVINCIBILITY_FRAMES = 0;

	bool invisible = false;
	bool ON_FL = false;
	uint_fast16_t P_METER = 0 ;

	double X_SPEED = 0.0;
	double Y_SPEED = 0.0;

	double LAST_X_SPEED_ON_FL = 0.0;

	double CAMERA_X = 0.0;
	double CAMERA_Y = 0.0;
	double FRM = 0;
	float to_scale = 1.f;

	uint_fast8_t COINS = 0;
	double height = 14.0;
	string sprite = "STAND_0";
	//uint_fast8_t STATE = 1;
	double x = 16.0;
	double y = 16.0;

	uint_fast8_t player_index = 0;
	uint_fast8_t skin = 0;
	uint_fast8_t STATE = 1;

	uint_fast8_t GRABBED_SPRITE = 0xFF; //A sprite index from 0 to 7F

	bool DEAD = false;
	bool pressed_y = false;
	bool old_y = false;


	int_fast8_t DEATH_TIMER = 0;

	bool PlayerControlled = false;

	bool pad[total_inputs] = { false,false,false,false,false,false,false };
	bool was_jumpin = false;

	//Sounds
	MPlayer(double newX = 0.0, double newY = 0.0)
	{
		x = newX;
		y = newY;
	}

	void Die()
	{
		if (!DEAD)
		{
			ASM.Write_To_Ram(0x1DFC, 100, 1);
			DEAD = true;
			DEATH_TIMER = 27;
		}
	}

	void Hurt()
	{
		if (!DEAD && INVINCIBILITY_FRAMES == 0)
		{

			if (STATE != 0)
			{
				//PlaySoundF("hurt.ogg", 2);
				STATE = 0;
				INVINCIBILITY_FRAMES = 90;
				ASM.Write_To_Ram(0x1DF9, 4, 1);
			}
			else
			{
				Die();
			}
		}

	}

	void Respawn()
	{
		x = 16.0;
		y = 16.0;
		DEATH_TIMER = 0;
		DEAD = false;
		STATE = 1;
	}

	void getInput()
	{
		//Controller buttons currently held down. Format: byetUDLR.
		//b = A or B; y = X or Y; e = select; t = Start; U = up; D = down; L = left, R = right.


		if (PlayerControlled)
		{
			pad[button_y] = (state[input_settings[0]] || state[input_settings[3]]) || (BUTTONS_GAMEPAD[7] || BUTTONS_GAMEPAD[6]);
			pad[button_b] = state[input_settings[1]] || BUTTONS_GAMEPAD[4];
			pad[button_a] = state[input_settings[2]] || BUTTONS_GAMEPAD[5];
			pad[button_left] = state[input_settings[4]] || BUTTONS_GAMEPAD[3];
			pad[button_right] = state[input_settings[5]] || BUTTONS_GAMEPAD[1];
			pad[button_down] = state[input_settings[6]] || BUTTONS_GAMEPAD[2];;
			pad[button_up] = state[input_settings[7]] || BUTTONS_GAMEPAD[0];
		}

	}

	void Enemy_Jump()
	{
		Y_SPEED = 5.0;
	}

	int DeathProcess()
	{

		x += X_SPEED;
		y += Y_SPEED;

		if (y < -16.0)
		{
			Respawn();
		}

		DEATH_TIMER -= 1;

		if (DEATH_TIMER == 0)
		{
			Y_SPEED = Calculate_Speed(1280.0);
		}
		if (DEATH_TIMER <= 0)
		{
			//cout << DEATH_TIMER % 5;
			if (DEATH_TIMER % 5 == -1)
			{
				to_scale *= -1.f;
			}
			Y_SPEED -= Calculate_Speed(48.0);
		}
		else
		{

			X_SPEED = 0.0;
			Y_SPEED = 0.0;
		}

		Get_Sprite();
		return 1;
	}

	void ProcessGrabbed()
	{
		if (GRABBED_SPRITE != 0xFF)
		{
			//cout << "Player 1 is holding sprite " << int(GRABBED_SPRITE) << endl;
			if (!pad[button_y] && !DEAD)
			{
				
				uint_fast16_t x_position = uint_fast16_t(double(x + to_scale * -15.0));
				uint_fast16_t y_position = uint_fast16_t(double(y - (STATE > 0 ? 13.0 : 16.0)) + 16.0);
				ServerRAM.RAM[0x2100 + GRABBED_SPRITE] = uint_fast8_t(x_position & 0xFF);
				ServerRAM.RAM[0x2180 + GRABBED_SPRITE] = uint_fast8_t(x_position >> 8);
				ServerRAM.RAM[0x2200 + GRABBED_SPRITE] = 0x00;

				ServerRAM.RAM[0x2280 + GRABBED_SPRITE] = uint_fast8_t(y_position & 0xFF);
				ServerRAM.RAM[0x2300 + GRABBED_SPRITE] = uint_fast8_t(y_position >> 8);
				ServerRAM.RAM[0x2380 + GRABBED_SPRITE] = 0x00;
				ServerRAM.RAM[0x2E00 + GRABBED_SPRITE] = 0x18;
				ServerRAM.RAM[0x2780 + GRABBED_SPRITE] = 0;

				ServerRAM.RAM[0x2000 + GRABBED_SPRITE] = 0x02;



				if (pad[button_down])
				{
					ServerRAM.RAM[0x2480 + GRABBED_SPRITE] = 0x00;
					ServerRAM.RAM[0x2400 + GRABBED_SPRITE] = uint_fast8_t(int_fast8_t(to_scale * -4));
				}

				if (pad[button_up])
				{
					ServerRAM.RAM[0x2480 + GRABBED_SPRITE] = 0x70;
					ServerRAM.RAM[0x2400 + GRABBED_SPRITE] = uint_fast8_t(int_fast8_t(X_SPEED * 8.0));
				}

				if (!pad[button_up] && !pad[button_down])
				{
					ServerRAM.RAM[0x2680 + GRABBED_SPRITE] = int_fast8_t(to_scale);
					ServerRAM.RAM[0x2000 + GRABBED_SPRITE] = 0x04;
					ServerRAM.RAM[0x2480 + GRABBED_SPRITE] = 0x00;
				}

				GRABBED_SPRITE = 0xFF;
				ASM.Write_To_Ram(0x1DF9, 0x3, 1);



			}
		}
	}

	bool Move(double xMove, double yMove, bool do_change = false)
	{
		double NewPositionX = x + xMove;
		double NewPositionY = y + yMove;
		bool willreturn = true;
		int_fast16_t PosXBlock = int_fast16_t(NewPositionX / 16);
		int_fast16_t PosYBlock = int_fast16_t(NewPositionY / 16);
		int_fast16_t startX = PosXBlock - TotalBlocksCollisionCheck;
		if (startX < 0)
			startX = 0;
		int_fast16_t startY = PosYBlock - TotalBlocksCollisionCheck;
		if (startY < 0)
			startY = 0;
		/*
		Shitty block collision. Surprised this doesn't lag.
		*/
		for (int_fast16_t xB = startX; xB < PosXBlock + TotalBlocksCollisionCheck; xB++)
		{
			for (int_fast16_t yB = startY; yB < PosYBlock + TotalBlocksCollisionCheck + (STATE > 0); yB++)
			{
				map16_handler.update_map_tile(xB, yB);


				double BelowBlock = double(yB * 16) - (height);
				double AboveBlock = double(yB * 16) + 16.0;
				double RightBlock = double(xB * 16) + 16.0;
				double LeftBlock = double(xB * 16) - 16.0;

				bool checkLeft = map16_handler.logic[3];
				bool checkRight = map16_handler.logic[2];
				bool checkBottom = map16_handler.logic[1];
				bool checkTop = map16_handler.logic[0];

				if (NewPositionX < RightBlock && NewPositionX > LeftBlock && NewPositionY < AboveBlock && NewPositionY > BelowBlock)
				{
					if (map16_handler.logic[4])
					{
						Hurt();
					}

					map16_handler.process_block(xB, yB, inside);
					if (xMove < 0.0 && checkRight)
					{
						if (NewPositionX < RightBlock && NewPositionX > RightBlock - bounds_x)
						{
							NewPositionX = RightBlock;
							willreturn = false;

							map16_handler.process_block(xB, yB, right, pressed_y);
						}
					}
					if (xMove > 0.0 && checkLeft)
					{
						if (NewPositionX > LeftBlock && NewPositionX < LeftBlock + bounds_x)
						{
							NewPositionX = LeftBlock;
							willreturn = false;

							map16_handler.process_block(xB, yB, left, pressed_y);
						}
					}
					if (yMove < 0.0 && checkTop)
					{
						if (NewPositionY < AboveBlock && NewPositionY > AboveBlock - bounds_y)
						{
							NewPositionY = AboveBlock;
						
							willreturn = false;

							map16_handler.process_block(xB, yB, top, pressed_y);
						}

					}
					if (yMove > 0.0 && checkBottom)
					{
						if (NewPositionY > BelowBlock && NewPositionY < BelowBlock + bounds_y)
						{
							NewPositionY = BelowBlock;
							willreturn = false;


							ASM.Write_To_Ram(0x1DF9, 0x1, 1);

							map16_handler.process_block(xB, yB, bottom);
						}
					}

				}
			}
		}

		/*
			Enemy Collision

			0x2000 - Sprite Status
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


			H = Hurts
			S = Solid
			G = Gravity
			O = Offscreen Processing
		*/
		for (uint_fast8_t sprite = 0; sprite < 128; sprite++)
		{
			if (ServerRAM.RAM[0x2000 + sprite] != 0)
			{
				double sprite_x = double(ServerRAM.RAM[0x2100 + sprite] + int(ServerRAM.RAM[0x2180 + sprite]) * 256) + double(ServerRAM.RAM[0x2200 + sprite]) / 256.0;
				double sprite_y = double(ServerRAM.RAM[0x2280 + sprite] + int(ServerRAM.RAM[0x2300 + sprite]) * 256) + double(ServerRAM.RAM[0x2380 + sprite]) / 256.0;
				double sprite_x_size = double(ServerRAM.RAM[0x2500 + sprite]);
				double sprite_y_size = double(ServerRAM.RAM[0x2580 + sprite]);

				double BelowSprite = sprite_y - height;
				double AboveSprite = sprite_y + sprite_y_size;
				double RightSprite = sprite_x + sprite_x_size;
				double LeftSprite = sprite_x - 16.0;

				bool checkRight = ServerRAM.RAM[0x2600 + sprite] & 0b1;
				bool checkLeft = ServerRAM.RAM[0x2600 + sprite] & 0b10;
				bool checkBottom = ServerRAM.RAM[0x2600 + sprite] & 0b100;
				bool checkTop = ServerRAM.RAM[0x2600 + sprite] & 0b1000;

				bool results[4] = { false,false,false,false };



				if (NewPositionX < RightSprite && NewPositionX > LeftSprite && NewPositionY < AboveSprite && NewPositionY > BelowSprite)
				{
					if (ServerRAM.RAM[0x2600 + sprite] & 0b10000000)
					{
						if (ServerRAM.RAM[0x2600 + sprite] & 0b10000)
						{
							if (yMove < 0.0 && NewPositionY > (AboveSprite-5))
							{
								NewPositionY += 1;
								Enemy_Jump();
								results[2] = true;
							}
							else
							{
								Hurt();
							}
						}
						else
						{
							Hurt();
						}
					}

					if (checkRight && xMove < 0.0 && NewPositionX < RightSprite && NewPositionX > RightSprite - bounds_x)
					{
						NewPositionX = RightSprite;
						willreturn = false;
						results[0] = true;
					}
					if (checkLeft && xMove > 0.0 && NewPositionX > LeftSprite && NewPositionX < LeftSprite + bounds_x)
					{
						NewPositionX = LeftSprite;
						willreturn = false;
						results[1] = true;
					}
					if (checkTop && yMove < 0.0 && NewPositionY < AboveSprite && NewPositionY > AboveSprite - bounds_y)
					{
						NewPositionY = AboveSprite;
						willreturn = false;
						if (do_change)
						{
							NewPositionX += double(int_fast8_t(ServerRAM.RAM[0x2400 + sprite]) * 16) / 256.0;
							NewPositionY += double(int_fast8_t(ServerRAM.RAM[0x2480 + sprite]) * 16) / 256.0;
						}
						results[2] = true;

					}
					if (checkBottom && yMove > 0.0 && NewPositionY > BelowSprite && NewPositionY < BelowSprite + bounds_y)
					{
						NewPositionY = BelowSprite;
						willreturn = false;
						results[3] = true;
					}

					ServerRAM.RAM[0x2700 + sprite] = 0;
					for (uint_fast8_t i = 0; i < 4; i++)
					{
						ServerRAM.RAM[0x2700 + sprite] += results[i] << i;
					}

					//Grabbing
					if (GRABBED_SPRITE == 0xFF)
					{
						if (ServerRAM.RAM[0x2000 + sprite] == 2 && ServerRAM.RAM[0x2E00 + sprite] == 0)
						{
							if (pad[button_y])
							{
								ServerRAM.RAM[0x2000 + sprite] = 3;
								GRABBED_SPRITE = sprite;
							}
							else
							{
								ServerRAM.RAM[0x2680 + sprite] = int_fast8_t(to_scale);
								ServerRAM.RAM[0x2000 + sprite] = 4;
								ServerRAM.RAM[0x2E00 + sprite] = 0x10;

								ASM.Write_To_Ram(0x1DF9, 3, 1);
							}
						}
					}
					
				}
			}
		}


		x = NewPositionX;
		y = NewPositionY;
		return willreturn;
	}
	void Get_Sprite()
	{
		string NewSprite = "STAND";

		if (DEAD)
		{
			sprite = "DEAD";
			return;
		}
		if (!CROUCH)
		{
			if (Y_SPEED != 0.0)
			{
				if (abs(LAST_X_SPEED_ON_FL) > Calculate_Speed_X(750.0))
				{
					NewSprite = "JUMPB";
				}
				else
				{
					if (Y_SPEED > 0.0)
					{
						NewSprite = "JUMP";
					}
					else
					{
						NewSprite = "FALL";
					}
				}
			}
			else
			{
				if (SKIDDING == 0.0)
				{
					if (X_SPEED != 0)
					{
						FRM += X_SPEED / 5;
						int Frame = abs(int(FRM) % (2 + (STATE > 0)));
						if (abs(X_SPEED) > Calculate_Speed_X(750.0))
						{
							NewSprite = "RUN" + std::to_string(Frame);
						}
						else
						{
							NewSprite = "WALK" + std::to_string(Frame);
						}
					}
				}
				else
				{
					NewSprite = "SKID";
				}
			}
		}
		else
		{
			NewSprite = "CROUCH";
		}
		if (X_SPEED > 0)
		{
			to_scale = -1.f;
		}
		if (X_SPEED < 0)
		{
			to_scale = 1.f;
		}
		sprite = NewSprite + "_" + std::to_string(STATE);
	}
	int Process()
	{
		getInput();

		height = (STATE > 0 && CROUCH == 0) ? 28.0 : 14.0;

		if (DEAD)
		{
			return DeathProcess();
		}
		if (y < -16.0)
		{
			Die();
		}

		if (INVINCIBILITY_FRAMES > 0.0)
		{
			INVINCIBILITY_FRAMES -= 1;
		}

		double GRAV = -96.0;
		bool RUN = false;
		bool MOV = false;
		bool SLIGHT_HIGH_SPEED = false;
		bool CAN_SPRINT = false;


		if (INVINCIBILITY_FRAMES > 0.0)
		{
			invisible = !invisible;
		}
		else
		{
			invisible = false;
		}

		ON_FL = false;
		if (!Move(0.0, -1.0, true)) //Detected a floor below
		{
			ON_FL = true;
		}
		else
		{
			Move(0.0, 1.0, false);
		}

		SKIDDING = 0;
		if (abs(X_SPEED) >= Calculate_Speed_X(576.0) && pad[button_y])
		{
			SLIGHT_HIGH_SPEED = true;
			if (ON_FL && !CROUCH)
			{
				P_METER += 2;
				if (P_METER > P_METER_REQUIRED)
				{
					P_METER = P_METER_REQUIRED;
				}
			}
		}
		else
		{
			if (ON_FL)
			{
				if (P_METER > 0)
				{
					P_METER -= 1;
				}
			}
		}

		if (P_METER >= P_METER_REQUIRED && pad[button_y])
		{
			CAN_SPRINT = true;
		}
		if (pad[button_left])
		{
			WALKING_DIR = -1;
			MOV = true;
		}
		if (pad[button_right])
		{
			WALKING_DIR = 1;
			MOV = true;
		}
		if (pad[button_down])
		{
			if (ON_FL)
			{
				WALKING_DIR = 0;
				MOV = true;
				CROUCH = true;
			}
		}
		else
		{
			if (ON_FL)
			{
				CROUCH = false;
			}
		}
		if (pad[button_y])
		{
			RUN = 1.0;
		}

		pressed_y = false;
		if (pad[button_y] != old_y)
		{
			old_y = pad[button_y];
			if (old_y)
			{
				pressed_y = true;
			}
		}
		if (pad[button_b] != was_jumpin)
		{
			was_jumpin = pad[button_b];
			if (was_jumpin && ON_FL)
			{
				Y_SPEED = Calculate_Speed(1232.0 + (148.0 * SLIGHT_HIGH_SPEED) + (20.0 * CAN_SPRINT));
				ASM.Write_To_Ram(0x1DFC, 0x35, 1);
			}
		}
		if (pad[button_b])
		{
			GRAV = GRAV * 0.5;
		}

		double SPEED_X_TO_SET = Calculate_Speed_X(320.0 + (RUN*256.0) + (CAN_SPRINT*192.0))*WALKING_DIR;
		double SPEED_ACCEL_X = Calculate_Speed_X(24.0);
		double STOPPING_DECEL = Calculate_Speed_X(16.0);
		double SKID_ACCEL = Calculate_Speed_X(16.0 + (24.0*RUN) + (CAN_SPRINT*40.0));

		if (MOV != 0)
		{
			if (X_SPEED > 0.0 && SPEED_X_TO_SET < 0.0 && WALKING_DIR == -1)
			{
				SKIDDING = -1;
				X_SPEED -= SKID_ACCEL;
			}
			if (X_SPEED < 0.0 && SPEED_X_TO_SET > 0.0 && WALKING_DIR == 1)
			{
				SKIDDING = 1;
				X_SPEED += SKID_ACCEL;
			}
			if (!ON_FL)
			{
				SKIDDING = 0;
			}
			//if (SKIDDING == 0)
			//{
				if (X_SPEED > SPEED_X_TO_SET)
				{
					X_SPEED -= SPEED_ACCEL_X;
					if (X_SPEED < SPEED_X_TO_SET)
					{
						X_SPEED = SPEED_X_TO_SET;
					}
				}

				if (X_SPEED < SPEED_X_TO_SET)
				{
					X_SPEED += SPEED_ACCEL_X;
					if (X_SPEED > SPEED_X_TO_SET)
					{
						X_SPEED = SPEED_X_TO_SET;
					}
				}
			//}
		}
		else
		{
			if (ON_FL == 1.0)
			{
				if (X_SPEED > 0.0)
				{
					X_SPEED -= STOPPING_DECEL;
					if (X_SPEED < 0.0)
					{
						X_SPEED = 0.0;
					}
				}
				if (X_SPEED < 0.0)
				{
					X_SPEED += STOPPING_DECEL;
					if (X_SPEED > 0.0)
					{
						X_SPEED = 0.0;
					}
				}
			}
		}


		if (!ON_FL)
		{
			Y_SPEED += Calculate_Speed(GRAV);
		}
		else
		{
			LAST_X_SPEED_ON_FL = X_SPEED;
		}
		if (Y_SPEED < Calculate_Speed(-1312.0))
		{
			Y_SPEED = Calculate_Speed(-1312.0);
		}

		if (!Move(X_SPEED + Calculate_Speed(double(int_fast8_t(ServerRAM.RAM[0x7B]))*16.0), 0.0))
		{
			X_SPEED = 0.0;
		}
		if (!Move(0.0, Y_SPEED + Calculate_Speed(double(int_fast8_t(ServerRAM.RAM[0x7D]))*16.0)))
		{
			Y_SPEED = 0.0;
		}

		Get_Sprite();

		if (ServerRAM.RAM[0x1411] == 0)
		{
			int min_x = ServerRAM.RAM[0x1462] + ServerRAM.RAM[0x1463] * 256;
			if (int(x) < (min_x + 8))
			{
				x = double(min_x + 8);
			}
			if (int(x) > (min_x + 256 - 24))
			{
				x = double(min_x + 256 - 24);
			}
			CAMERA_X = double(120 + ServerRAM.RAM[0x1462] + ServerRAM.RAM[0x1463] * 256);
		}
		else
		{
			//Camera lol

			
			if (CAMERA_X < x)
			{
				CAMERA_X += 5.0;
				if (CAMERA_X > x)
				{
					CAMERA_X = x;
				}
			}

			if (CAMERA_X > x)
			{
				CAMERA_X -= 5.0;
				if (CAMERA_X < x)
				{
					CAMERA_X = x;
				}
			}

			//CAMERA_X = x;
			//CAMERA_X += (x - CAMERA_X) / 20.0;
		}

		if (ServerRAM.RAM[0x1412] == 0) {
			CAMERA_Y = double(104 + ServerRAM.RAM[0x1464] + ServerRAM.RAM[0x1465] * 256);
		}
		else {
			double new_y = (y + 16);
			if (CAMERA_Y < new_y)
			{
				CAMERA_Y += 4.0;
				if (CAMERA_Y > new_y)
				{
					CAMERA_Y = new_y;
				}
			}

			if (CAMERA_Y > new_y)
			{
				CAMERA_Y -= 4.0;
				if (CAMERA_Y < new_y)
				{
					CAMERA_Y = new_y;
				}
			}
		}

		if (x < 8.0) { x = 8.0; }
		if (x > double(-24 + mapWidth * 16)) { x = double(-24 + mapWidth * 16); }

		ProcessGrabbed();
		return 1;
	}
};


//Mario management

std::list<MPlayer> Mario; //This is how much players exist. Starts at 0.
void AddNewPlayer() { MPlayer NewPlayer = MPlayer(16.0, 16.0); Mario.push_back(NewPlayer); }
void RemovePlayer() { Mario.pop_back(); }
void CheckForPlayers() //Have to be careful when fucking with this. Or else memory might leak.
{
	uint_fast8_t player_am = uint_fast8_t(Mario.size());
	if (player_am != PlayerAmount) //example, player am is 0, and amount is 1.
	{
		int_fast8_t distance = PlayerAmount - player_am; //1 - 0 = 1, needs to add!
		while (distance > 0) { distance -= 1; AddNewPlayer(); }
		while (distance < 0) { distance += 1; RemovePlayer(); }

	}
}

// Get Mario

MPlayer& get_mario(uint_fast8_t number)
{
	CheckForPlayers();
	
	uint_fast8_t player = 1;
	for (std::list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		if (player == number)
		{
			return *item;
		}
		player += 1;

	}

	return *Mario.begin();
}

//player interaction with other players, for now it's just mario combat
void PlayerInteraction()
{
	for (std::list<MPlayer>::iterator curr_p = Mario.begin(); curr_p != Mario.end(); ++curr_p)
	{
		MPlayer& CurrPlayer = *curr_p;
		if (CurrPlayer.DEAD)
		{
			continue;
		}
		else
		{
			for (std::list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
			{
				MPlayer& PlrInteract = *item;
				if ((&PlrInteract == &CurrPlayer || PlrInteract.DEAD) || PlrInteract.INVINCIBILITY_FRAMES > 0)
				{
					continue;
				}
				else
				{
					double LeftBound = PlrInteract.x - 16.0;
					double RightBound = PlrInteract.x + 16.0;
					double UpBound = PlrInteract.y + (PlrInteract.STATE == 0 ? 16.0 : 32.0);
					double DownBound = PlrInteract.y - (CurrPlayer.STATE == 0 ? 16.0 : 32.0);
					if (CurrPlayer.x > LeftBound && CurrPlayer.x < RightBound && CurrPlayer.y < UpBound && CurrPlayer.y > DownBound && CurrPlayer.Y_SPEED < -2.0)
					{
						CurrPlayer.Enemy_Jump();
						PlrInteract.Hurt();
					}
				}

			}
		}

	}
}