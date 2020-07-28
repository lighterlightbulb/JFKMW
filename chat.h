#pragma once

#define chat_onscreen_timer 420
string Curr_ChatString[6] = {"","","",""};

string Curr_PChatString = "";
string Old_ChatString = "";
int Time_ChatString[6] = {0,0,0,0}; //How long it will take for this string to disappear, set to 300 every time a message is sent. In frames

void Add_Chat(string c)
{
	for (int i = 5; i > 1; i--)
	{
		Curr_ChatString[i] = Curr_ChatString[i - 1];
		Time_ChatString[i] = Time_ChatString[i - 1];
	}

	Curr_ChatString[0] = c;
	Time_ChatString[0] = chat_onscreen_timer;
}
void Chat_ServerSide()
{
	int p = 1;
	for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrPlayer = *item;
		if (CurrPlayer.last_chat_string != CurrPlayer.curr_chat_string && CurrPlayer.curr_chat_string != "")
		{
			Curr_PChatString = CurrPlayer.curr_chat_string;
			Time_ChatString[0] = chat_onscreen_timer;

			CurrPlayer.last_chat_string = CurrPlayer.curr_chat_string;
			cout << lua_color << "[Chat S] " << Curr_PChatString << white << endl;
			lua_on_chatted(CurrPlayer.curr_chat_string, p);
			//Forward message to discord.
			if (!isClient && networking)
			{
				string New_ChatString = CurrPlayer.curr_chat_string;
				replaceAll(New_ChatString, "<", "");
				replaceAll(New_ChatString, ">", ":");
				replaceAll(New_ChatString, ",", "");

				discord_message(New_ChatString);
			}
		}
		p++;
	}
	
	if (Time_ChatString[0] > 0 && networking)
	{
		Time_ChatString[0]--;
		if (!Time_ChatString[0])
		{
			Curr_PChatString = "";
		}
	}
}

void Chat_ClientSide()
{
	if (Old_ChatString != Curr_PChatString && Curr_PChatString != "")
	{
		Add_Chat(Curr_PChatString);
		Old_ChatString = Curr_PChatString;

		cout << lua_color << "[Chat C] " << Curr_PChatString << white << endl;
	}

	for (int i = 0; i < 6; i++)
	{
		if (Time_ChatString[i] > 0)
		{
			Time_ChatString[i]--;
			if (!Time_ChatString[i])
			{
				Curr_ChatString[i] = "";
			}
		}
	}

	if (Chatting)
	{
		for (int i = 0x41; i <= 0x5A; i++)
		{
			if (getKey(i))
			{
				Typing_In_Chat += (i-0x10) + '0';
			}
		}
		//Numbers
		for (int i = 0x30; i <= 0x39; i++)
		{
			if (getKey(i))
			{
				Typing_In_Chat += (i - 0x30) + '0';
			}
		}

		if (getKey(0x20))
		{
			Typing_In_Chat += " ";
		}

		//Delete last
		if (getKey(0x08) && Typing_In_Chat.size() > 0) { Typing_In_Chat.pop_back(); }

		Typing_In_Chat = Typing_In_Chat.substr(0, min(55, int(Typing_In_Chat.length())));
	}
	bool stat = Chatting ? (state[input_settings[9]]) : state[input_settings[10]];
	if (stat != pressed_start)
	{
		pressed_start = stat;
		if (stat)
		{
			Chatting = !Chatting;
			if (Chatting == false)
			{

				MPlayer& LocalPlayer = get_mario(SelfPlayerNumber);
				LocalPlayer.Chat(Typing_In_Chat);
			}
			else
			{
#if defined(_WIN32)
				KeyStates[0x54] = true;
#endif
			}
			Typing_In_Chat = "";
		}
	}
}

void ProcessChat()
{
	if (!isClient)
	{
		Chat_ServerSide();
	}

	if (isClient || !networking)
	{
		Chat_ClientSide();
	}
}