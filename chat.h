#pragma once

string Curr_ChatString = "";
string Old_ChatString = "";
int Time_ChatString = 0; //How long it will take for this string to disappear, set to 300 every time a message is sent. In frames

void Chat_ServerSide()
{
	for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrPlayer = *item;
		if (CurrPlayer.last_chat_string != CurrPlayer.curr_chat_string)
		{
			CurrPlayer.last_chat_string = CurrPlayer.curr_chat_string;
			Curr_ChatString = CurrPlayer.curr_chat_string;
		}
	}
}

void Chat_ClientSide()
{
	if (Old_ChatString != Curr_ChatString)
	{
		Old_ChatString = Curr_ChatString;
		Time_ChatString = 300;
	}

	if (Time_ChatString > 0)
	{
		Time_ChatString--;
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