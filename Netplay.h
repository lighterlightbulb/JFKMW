#pragma once

#define Header_AttemptJoin 0x00
#define Header_UpdatePlayerData 0x01
#define Header_GlobalUpdate 0x02
#define Header_Connection 0x03
#define Header_ConnectData 0x04
#define Header_RAM 0x05
#define Header_MusicData 0x06

#define Delay_Connection 200

string latest_chk = "";
sf::Socket::Status last_status;
sf::Socket::Status receiveWithTimeout(sf::TcpSocket& socket, sf::Packet& packet, sf::Time timeout, bool blocking)
{
	sf::SocketSelector selector;
	selector.add(socket);
	if (selector.wait(timeout)){
		socket.setBlocking(blocking);
		last_status = socket.receive(packet);
		return last_status;
	}
	else {
		return sf::Socket::NotReady;
	}
}

bool validated_connection = false;

uint_fast8_t GetAmountOfPlayers() {
	PlayerAmount = uint_fast8_t(clients.size()); CheckForPlayers();
	return PlayerAmount;
}

void PreparePacket(uint8_t header) {
	CurrentPacket.clear(); CurrentPacket << header; CurrentPacket_header = header;
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

	CurrentPacket << CurrentMario.skin; CurrentPacket << CurrentMario.in_pipe; CurrentPacket << CurrentMario.pipe_speed;
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

	CurrentPacket >> CurrentMario.skin; CurrentPacket >> CurrentMario.in_pipe; CurrentPacket >> CurrentMario.pipe_speed;
	for (int inputs = 0; inputs < total_inputs; inputs++)
	{
		CurrentPacket >> CurrentMario.pad[inputs];
	}
	//copypaste
}



/*

Pack Mario Data

*/

void pack_mario_data(uint_fast8_t skip = 1) {
	if (!isClient) {
		CurrentPacket << PlayerAmount;
		uint_fast8_t plrNum = 1;
		for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item) { 
			if (plrNum != skip) {
				put_mario_data_in(*item);
			}
			else
			{
				MPlayer& CurrentMario = *item;
				CurrentPacket << CurrentMario.GRABBED_SPRITE;
			}
			plrNum += 1;
		}
	}
	else {
		CurrentPacket << SelfPlayerNumber; 
		put_mario_data_in(get_mario(SelfPlayerNumber));
	}
}


/*

Disconnection handler

*/


void HandleDisconnection(sf::TcpSocket* ToSend = nullptr) {
	cout << blue << "[Server] " << ToSend->getRemoteAddress() << " has disconnected." << white << endl;

	clients.erase(remove(clients.begin(), clients.end(), ToSend));
	selector.remove(*ToSend);

	ToSend->disconnect();
	last_status = sf::Socket::Error;
	delete ToSend;
}


/*

Packet Sending

*/

void send_not_blocking(sf::TcpSocket* ToSend = nullptr) {
	if (ToSend->send(CurrentPacket) != sf::Socket::Done) {
		if (!isClient) {
			HandleDisconnection(ToSend);
		}
		else {
			disconnected = true;
		}
	}
}

void SendPacket(sf::TcpSocket* ToSend = nullptr) {
	//Server will send a packet.
	//Players will receive it.
	//Client will send a packet to server, server receives it.

	if (!isClient) {
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



void ReceivePacket(sf::TcpSocket &whoSentThis, bool for_validating = false)
{
	//cout << last_status << endl;
	CurrentPacket >> CurrentPacket_header;
	data_size_current += int(CurrentPacket.getDataSize());

	if (for_validating == true)
	{
		if (CurrentPacket_header == Header_AttemptJoin)
		{
			cout << blue << "[Client] Receiving verification.." << white << endl;

			string validation;
			CurrentPacket >> username;

			cout << blue << "[Client] " << username << " has passed verification." << white << endl;
			validated_connection = true;

		}
		return;
	}
	/*
	SERVER BEHAVIOUR
	*/
	if (!isClient)
	{

		//cout << blue << "[Network] Received packet (" << CurrentPacket_header << ") from " << whoSentThis.getRemoteAddress() << white << endl;
		/*

		UPDATE PLAYER DATA

		*/
		if (CurrentPacket_header == Header_UpdatePlayerData)
		{
			uint_fast8_t PlrNum = 1; CurrentPacket >> PlrNum; 
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

		if (CurrentPacket_header == Header_MusicData)
		{
			ReceiveMusic();
		}

		if (CurrentPacket_header == Header_GlobalUpdate)
		{
			//cout << "Received Global Update Packet Of Size " << CurrentPacket.getDataSize() << endl;
			CurrentPacket >> SelfPlayerNumber; //Me
			CurrentPacket >> PlayerAmount; //Update Plr Amount

			CheckForPlayers(); //have to update the mario list. so it fits.



			uint_fast8_t num = 1;
			for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item) {
				if (num != SelfPlayerNumber) {
					take_mario_data(*item);
				}
				else
				{
					MPlayer& CurrentMario = *item;
					CurrentPacket >> CurrentMario.GRABBED_SPRITE;
				}
				num += 1;
			}

			CurrentPacket >> recent_big_change;
			Sync_Server_RAM(!recent_big_change);

		}

		if (CurrentPacket_header == Header_ConnectData)
		{
			cout << blue << "[Client] Received connection data." << white << endl;

			CurrentPacket >> PlayerAmount; //Update Plr Amount
			CheckForPlayers(); //have to update the mario list. so it fits.

			cout << blue << "[Client] Receiving server RAM" << white << endl;
			Sync_Server_RAM(false);
			cout << blue << "[Client] Receiving music" << white << endl;
			ReceiveMusic(true);
			cout << blue << "[Client] Received." << white << endl;
		}

	}
}

bool receive_all_packets(sf::TcpSocket& socket, bool slower = false, bool for_validating = false)
{
	while (doing_read || doing_write)
	{
		sf::sleep(sf::milliseconds(1));
	}
	int current_pack = 0;

	while (receiveWithTimeout(socket, CurrentPacket, sf::milliseconds(slower ? 2000 : packet_wait_time), !for_validating) != sf::Socket::NotReady)
	{

		current_pack += 1;
		ReceivePacket(socket, for_validating);
	}
	if (for_validating && !validated_connection)
	{
		return false;
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
	//cout << blue << "[Server] Pending connection..." << white << endl;


	sf::TcpSocket* client = new sf::TcpSocket;
	if (listener.accept(*client) == sf::Socket::Done)
	{
		uint_fast8_t NewPlayerNumber = GetAmountOfPlayers() + 1;

		cout << blue << "[Server] A client (assigned to Player " << int(NewPlayerNumber) << ") is trying to connect... (" << client->getRemoteAddress() << ")" << white << endl;

		PreparePacket(Header_Connection); CurrentPacket << NewPlayerNumber; SendPacket(client);


		validated_connection = false;
		receive_all_packets(*client, true, true);

		if (validated_connection)
		{

			// Add the new client to the clients list
			clients.push_back(client); selector.add(*client); GetAmountOfPlayers();
			cout << blue << "[Server] " << username << " (" << client->getRemoteAddress() << ", Player " << dec << int(NewPlayerNumber) << ") has connected." << white << endl;

			PreparePacket(Header_ConnectData);
			CurrentPacket << PlayerAmount;
			Push_Server_RAM(false);
			SendMusic();
			SendPacket(client);


			GetAmountOfPlayers(); //Update player amount.
		}
		else
		{
			cout << blue << "[Server] " << username << " (" << client->getRemoteAddress() << ", " << latest_chk << ", Player " << dec << int(NewPlayerNumber) << ") Timed out or sent invalid information. Disconnecting." << white << endl;
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
		uint_fast8_t PlrNumber = 1;

		for (int i = 0; i < clients.size(); ++i) {
			sf::TcpSocket& client = *clients[i];
			receive_all_packets(client);
			if (clients.size() < 1 || last_status == sf::Socket::Error || last_status == sf::Socket::Disconnected)
			{
				//cout << blue << "[Server] Attempting to recover from player failure. Will stop communication." << white << endl;
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
				//cout << blue << "[Server] Attempting to recover from player failure. Will stop communication." << white << endl;
				break;
			}

			PlrNumber += 1;

		}

		if (need_sync_music)
		{
			PreparePacket(Header_MusicData);
			SendMusic();
			SendPacket();
			need_sync_music = false;
		}

		if ((int(PlrNumber)-1) == clients.size()) //Yea we did it.
		{
			ASM.Reset_ASM_Variables_Server();
			Set_Server_RAM();
		}

		if (recent_big_change)
		{
			recent_big_change = false;
			cout << green << "[Network] Synced RAM and game." << endl;
		}


	}
}


void NetWorkLoop()
{
	if (!isClient)
	{
		listener.listen(PORT); selector.add(listener);
		cout << blue << "[Server] Server is running on port " << dec << PORT << white << endl;
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
		while (Mario.size() == 0) {
			chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
			receive_all_packets(socketG);
			chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
			latest_server_response = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
		}

		cout << blue << "[Network] Connected to server. " << int(PlayerAmount) << " player(s) connected." << endl;
		while (!quit && !disconnected)
		{
			chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
			receive_all_packets(socketG);
			chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
			latest_server_response = chrono::duration_cast<chrono::duration<double>>(t2 - t1);

			PreparePacket(Header_UpdatePlayerData); pack_mario_data(); SendPacket();
		}
	}
}

bool ConnectClient(void)
{
	if (socketG.connect(ip, PORT) == sf::Socket::Done)
	{
		PreparePacket(Header_AttemptJoin);
		CurrentPacket << username;
		SendPacket();

		sf::sleep(sf::milliseconds(500));

		receive_all_packets(socketG, true);

		if (last_status == sf::Socket::Error || last_status == sf::Socket::Disconnected)
		{
			return false;
		}

		CheckForPlayers();
		cout << blue << "[Network] Connected to " << ip << ":" << dec <<  PORT << white << endl;
		return true;
	}
	else
	{
		cout << blue << "[Network] Failed to connect. Falling back to normal mode." << white << endl;
		return false;
	}
}