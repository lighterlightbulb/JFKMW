#pragma once

#define Header_AttemptJoin 0x00
#define Header_UpdatePlayerData 0x01
#define Header_GlobalUpdate 0x02
#define Header_Connection 0x03
#define Header_LevelData 0x04
#define Header_RAM 0x05
#define Delay_Connection 200

string latest_chk = "";
sf::Socket::Status last_status;
sf::Socket::Status receiveWithTimeout(sf::TcpSocket& socket, sf::Packet& packet, sf::Time timeout)
{
	sf::SocketSelector selector;
	selector.add(socket);
	if (selector.wait(timeout))
	{
		last_status = socket.receive(packet);
		return last_status;
	}
	else
	{
		return sf::Socket::NotReady;
	}
}



//Netcode
bool validated_connection = false;

int GetAmountOfPlayers()
{
	PlayerAmount = int(clients.size()); CheckForPlayers();
	return PlayerAmount;
}

void PreparePacket(uint8_t header)
{
	CurrentPacket.clear(); CurrentPacket << header; CurrentPacket_header = header;
}

void server_print(string text)
{
	cout << blue << text << white << endl;
}

/*

Mario Data

*/
void put_mario_data_in(MPlayer& CurrentMario)
{
	//copypaste
	CurrentPacket << CurrentMario.X_SPEED; CurrentPacket << CurrentMario.Y_SPEED;
	CurrentPacket << CurrentMario.x; CurrentPacket << CurrentMario.y;
	CurrentPacket << CurrentMario.STATE; CurrentPacket << CurrentMario.ON_FL;

	CurrentPacket << CurrentMario.DEAD; CurrentPacket << CurrentMario.INVINCIBILITY_FRAMES; CurrentPacket << CurrentMario.DEATH_TIMER;

	CurrentPacket << CurrentMario.to_scale; CurrentPacket << CurrentMario.SKIDDING; CurrentPacket << CurrentMario.P_METER;
	CurrentPacket << CurrentMario.FRM; CurrentPacket << CurrentMario.WALKING_DIR;

	CurrentPacket << CurrentMario.COINS; CurrentPacket << CurrentMario.player_index;

	CurrentPacket << CurrentMario.CAMERA_X; CurrentPacket << CurrentMario.CAMERA_Y;

	CurrentPacket << CurrentMario.skin;
	for (int inputs = 0; inputs < total_inputs; inputs++)
	{
		CurrentPacket << CurrentMario.pad[inputs];
	}
	//copypaste
}

void take_mario_data(MPlayer& CurrentMario)
{
	//copypaste
	CurrentPacket >> CurrentMario.X_SPEED; CurrentPacket >> CurrentMario.Y_SPEED;
	CurrentPacket >> CurrentMario.x; CurrentPacket >> CurrentMario.y;
	CurrentPacket >> CurrentMario.STATE; CurrentPacket >> CurrentMario.ON_FL;

	CurrentPacket >> CurrentMario.DEAD; CurrentPacket >> CurrentMario.INVINCIBILITY_FRAMES; CurrentPacket >> CurrentMario.DEATH_TIMER;

	CurrentPacket >> CurrentMario.to_scale; CurrentPacket >> CurrentMario.SKIDDING; CurrentPacket >> CurrentMario.P_METER;
	CurrentPacket >> CurrentMario.FRM; CurrentPacket >> CurrentMario.WALKING_DIR;

	CurrentPacket >> CurrentMario.COINS; CurrentPacket >> CurrentMario.player_index;

	CurrentPacket >> CurrentMario.CAMERA_X; CurrentPacket >> CurrentMario.CAMERA_Y;

	CurrentPacket >> CurrentMario.skin;
	for (int inputs = 0; inputs < total_inputs; inputs++)
	{
		CurrentPacket >> CurrentMario.pad[inputs];
	}
	//copypaste
}


/*

Pack Mario Data

*/

void pack_mario_data(int skip = 1)
{
	if (!isClient)
	{
		CurrentPacket << PlayerAmount;

		int plrNum = 1;
		for (std::list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item) { 
			if (plrNum != skip)
			{
				put_mario_data_in(*item);
			}
			plrNum += 1;
		}
	}
	else
	{

		CurrentPacket << SelfPlayerNumber; 
		put_mario_data_in(get_mario(SelfPlayerNumber));
	}
}


/*

Disconnection handler

*/


void HandleDisconnection(sf::TcpSocket* ToSend = nullptr)
{

	cout << blue << "[Server] " << ToSend->getRemoteAddress() << " has disconnected." << white << endl;

	clients.erase(std::remove(clients.begin(), clients.end(), ToSend), clients.end());
	selector.remove(*ToSend);

	ToSend->disconnect();

	//cout << blue << "[Server] Amount of players : " << clients.size() << white << endl;

	last_status = sf::Socket::Error;

	delete ToSend;

}


/*

Packet Sending

*/

void send_not_blocking(sf::TcpSocket* ToSend = nullptr)
{
	if (ToSend->send(CurrentPacket) != sf::Socket::Done)
	{

		if (!isClient)
		{
			//cout << "sending packet of header " << int(CurrentPacket_header) << endl;
			HandleDisconnection(ToSend); //Partial or disconnected will simply just disconnect the player.
		}
		else
		{
			disconnected = true;
		}
	}

}

void SendPacket(sf::TcpSocket* ToSend = nullptr)
{
	//Server will send a packet.
	//Players will receive it.
	//Client will send a packet to server, server receives it.

	if (!isClient)
	{
		if (ToSend != nullptr) {
			send_not_blocking(ToSend);
		}
		else
		{
			for (int i = 0; i < clients.size(); ++i) {

				ToSend = clients[i];
				
				send_not_blocking(ToSend);
			}
		}
	}
	else
	{
		send_not_blocking(&socketG);
	}
}



void ReceivePacket(sf::TcpSocket &whoSentThis, bool ignore_status = false)
{
	//cout << last_status << endl;
	if (!ignore_status)
	{
		if (last_status == sf::Socket::Disconnected || last_status == sf::Socket::Error)
		{
			if (!isClient)
			{
				HandleDisconnection(&whoSentThis);
			}
			else
			{
				disconnected = true;
			}
			return;
		}
	}
	CurrentPacket >> CurrentPacket_header;

	data_size_current += int(CurrentPacket.getDataSize());
	//cout << "header = " << CurrentPacket_header << endl;

	//cout << blue << "[Network] Received Packet of " << dec << int(CurrentPacket.getDataSize()) << " header : " << CurrentPacket_header << white << endl;

	/*
	SERVER BEHAVIOUR
	*/
	if (!isClient)
	{

		//cout << blue << "[Network] Received packet (" << CurrentPacket_header << ") from " << whoSentThis.getRemoteAddress() << white << endl;
		if (CurrentPacket_header == Header_AttemptJoin)
		{
			//validated_connection = true;
			/*string bruh;
			CurrentPacket >> bruh;
			validated_connection = bruh == da_epical_function_lol("JFKMarioWorld.exe");
			latest_chk = bruh;
			CurrentPacket >> username;*/
		}
		/*

		UPDATE PLAYER DATA

		*/
		if (CurrentPacket_header == Header_UpdatePlayerData)
		{
			int PlrNum = 1; CurrentPacket >> PlrNum; 
			if (PlrNum > PlayerAmount)
			{
				PlayerAmount = PlrNum;
				CheckForPlayers();
			}
			take_mario_data(get_mario(PlrNum));
			//cout << "attempting to update player " << PlrNum << endl;
		}
	}
	else
	/*
	CLIENT BEHAVIOUR
	*/
	{
		/*

		UPDATE PLAYER DATA

		*/
		if (CurrentPacket_header == Header_GlobalUpdate)
		{
			CurrentPacket >> SelfPlayerNumber; //Me
			CurrentPacket >> PlayerAmount; //Update Plr Amount

			CheckForPlayers(); //have to update the mario list. so it fits.



			int num = 1;
			for (std::list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item) {
				if (num != SelfPlayerNumber) { 
					take_mario_data(*item); 
					//cout << "Updated player " << num << endl;
				}
				num += 1;
			}

			CurrentPacket >> recent_big_change;
			Sync_Server_RAM(!recent_big_change);

		}

		if (CurrentPacket_header == Header_Connection)
		{
			CurrentPacket >> SelfPlayerNumber; PlayerAmount = SelfPlayerNumber; CheckForPlayers();
			//cout << blue << "[Client] Validating connection.. (You are player " << SelfPlayerNumber << ")" << white << endl;

			PreparePacket(Header_AttemptJoin); 
			CurrentPacket << da_epical_function_lol("JFKMarioWorld.exe");
			CurrentPacket << username;
			SendPacket();
		}

		if (CurrentPacket_header == Header_LevelData)
		{
			//cout << blue << "[Client] Received map_data." << white << endl;
			Sync_Server_RAM();
		}

	}
}

bool receive_all_packets(sf::TcpSocket& socket, bool slower = false)
{
	int current_pack = 0;

	while (receiveWithTimeout(socket, CurrentPacket, sf::milliseconds(slower ? 100 : packet_wait_time)) != sf::Socket::NotReady)
	{
		current_pack += 1;
		ReceivePacket(socket);
	}
	if (!isClient)
	{
		return !((last_status == sf::Socket::Error) || (last_status == sf::Socket::Disconnected));
	}
	return !disconnected;
}

void PendingConnection()
{
	// The listener is ready: there is a pending connection
	//std::cout << blue << "[Server] Pending connection..." << white << std::endl;


	sf::TcpSocket* client = new sf::TcpSocket;
	if (listener.accept(*client) == sf::Socket::Done)
	{
		int NewPlayerNumber = GetAmountOfPlayers() + 1;

		std::cout << blue << "[Server] A client (assigned to Player " << NewPlayerNumber << ") is trying to connect... (" << client->getRemoteAddress() << ")" << white << std::endl;

		PreparePacket(Header_Connection); CurrentPacket << NewPlayerNumber; SendPacket(client);

		validated_connection = true;
		/*username = "Unknown";
		latest_chk = "Invalid";

		int attempts = 0;
		while (client->receive(CurrentPacket) != sf::Socket::Done && attempts < 10)
		{
			attempts += 1;
			Sleep(500);
			std::cout << blue << "[Server] Attempting connection validation.." << white << std::endl;
		}
		
		if (attempts < 10)
		{
			ReceivePacket(*client, true);
		}*/
		
		if (validated_connection)
		{

			// Add the new client to the clients list
			clients.push_back(client); selector.add(*client); GetAmountOfPlayers();
			std::cout << blue << "[Server] " << username << " (" << client->getRemoteAddress() << ", " << latest_chk << ", Player " << dec << NewPlayerNumber << ") has connected." << white << std::endl;

			PreparePacket(Header_LevelData);
			Push_Server_RAM();
			SendPacket(client);


			GetAmountOfPlayers(); //Update player amount.
		}
		else
		{
			cout << blue << "[Server] " << username << " (" << client->getRemoteAddress() << ", " << latest_chk << ", Player " << dec << NewPlayerNumber << ") Timed out or sent invalid information. Disconnecting." << white << std::endl;
			client->disconnect(); 
			delete client;
		}

	}
	else
	{
		// Error, we won't get a new connection, delete the socket
		delete client;
	}
}


void Server_To_Clients()
{
	//data_size_current = 0;
	GetAmountOfPlayers();

	if (clients.size() > 0)
	{
		int PlrNumber = 1;


		for (int i = 0; i < clients.size(); ++i) {
			sf::TcpSocket& client = *clients[i];
			receive_all_packets(client);
			if (clients.size() < 1 || last_status == sf::Socket::Error || last_status == sf::Socket::Disconnected)
			{
				//cout << blue << "[Server] Attempting to recover from player failure. Will stop communication." << white << std::endl;
				break;
			}


			PreparePacket(Header_GlobalUpdate); 
			CurrentPacket << PlrNumber; 
			pack_mario_data(PlrNumber); 

			CurrentPacket << recent_big_change;
			Push_Server_RAM(!recent_big_change);

			SendPacket(&client);



			if (clients.size() < 1 || last_status == sf::Socket::Error || last_status == sf::Socket::Disconnected)
			{
				//cout << blue << "[Server] Attempting to recover from player failure. Will stop communication." << white << std::endl;
				break;
			}

			PlrNumber += 1;
			
		}

		if ((PlrNumber-1) == clients.size()) //Yea we did it.
		{
			ASM.Reset_ASM_Variables_Server();
			Set_Server_RAM();

			if (recent_big_change)
			{
				recent_big_change = false;
				cout << green << "[Network] Synced RAM and game." << endl;
			}
		}


	}
}


void NetWorkLoop()
{
	if (!isClient)
	{
		listener.listen(PORT); selector.add(listener);
		std::cout << blue << "[Server] Server is running on port " << dec << PORT << white << std::endl;
		// Endless loop that waits for new connections
		while (!quit)
		{
			if (selector.wait(sf::milliseconds(network_update_rate)))
			{
				// Test the listener
				if (selector.isReady(listener))
				{
					PendingConnection();
				}
				else
				{
					Server_To_Clients();
				}
			}
		}
	}
	else
	{
		/*
		CLIENT MODE
		*/

		while (!quit && !disconnected)
		{
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			receive_all_packets(socketG);
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			latest_server_response = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

			PreparePacket(Header_UpdatePlayerData); pack_mario_data(); SendPacket();
		}
	}
}

bool ConnectClient(void)
{
	if (socketG.connect(ip, PORT) == sf::Socket::Done)
	{

		receive_all_packets(socketG, false);

		if (last_status == sf::Socket::Error || last_status == sf::Socket::Disconnected)
		{
			return false;
		}

		CheckForPlayers();
		std::cout << blue << "[Network] Connected to " << ip << ":" << dec <<  PORT <<  ", " << PlayerAmount << " Players connected." << white << std::endl;
		return true;
	}
	return false;
}