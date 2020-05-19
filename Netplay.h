#pragma once

#define Header_AttemptJoin 0x00
#define Header_UpdatePlayerData 0x01
#define Header_GlobalUpdate 0x02
#define Header_Connection 0x03
#define Header_ConnectData 0x04
#define Header_RAM 0x05
#define Header_MusicData 0x06
#define Header_FailedToConnect 0x07

#define Delay_Connection 200

string latest_error = "";

sf::Socket::Status last_network_status;
sf::Socket::Status receiveWithTimeout(sf::TcpSocket& socket, sf::Packet& packet, sf::Time timeout, bool blocking)
{
	sf::SocketSelector selector;
	selector.add(socket);
	if (selector.wait(timeout)){
		socket.setBlocking(blocking);
		last_network_status = socket.receive(packet);
		return last_network_status;
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
	CurrentPacket.clear(); CurrentPacket << header;
	CurrentPacket_header = header;
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

	CurrentPacket << CurrentMario.player_index;

	CurrentPacket << CurrentMario.CAMERA_X; CurrentPacket << CurrentMario.CAMERA_Y;

	CurrentPacket << CurrentMario.SLOPE_TYPE;

	CurrentPacket << CurrentMario.jump_is_spin;

	CurrentPacket << CurrentMario.KO_counter; CurrentPacket << CurrentMario.WO_counter;

	CurrentPacket << CurrentMario.skin; CurrentPacket << CurrentMario.in_pipe; CurrentPacket << CurrentMario.pipe_speed;
	for (int inputs = 0; inputs < total_inputs; inputs++)
	{
		CurrentPacket << CurrentMario.pad[inputs];
	}
	for (int plr_name = 0; plr_name < player_name_size; plr_name++)
	{
		CurrentPacket << uint_fast8_t(CurrentMario.player_name_cut[plr_name]);
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

	CurrentPacket >> CurrentMario.player_index;

	CurrentPacket >> CurrentMario.CAMERA_X; CurrentPacket >> CurrentMario.CAMERA_Y;

	CurrentPacket >> CurrentMario.SLOPE_TYPE;

	CurrentPacket >> CurrentMario.jump_is_spin;

	CurrentPacket >> CurrentMario.KO_counter; CurrentPacket >> CurrentMario.WO_counter;

	CurrentPacket >> CurrentMario.skin; CurrentPacket >> CurrentMario.in_pipe; CurrentPacket >> CurrentMario.pipe_speed;
	for (int inputs = 0; inputs < total_inputs; inputs++)
	{
		CurrentPacket >> CurrentMario.pad[inputs];
	}
	for (int plr_name = 0; plr_name < player_name_size; plr_name++)
	{
		uint_fast8_t new_l = 0;
		CurrentPacket >> new_l;
		CurrentMario.player_name_cut[plr_name] = char(new_l);
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
	last_network_status = sf::Socket::Error;
	delete ToSend;
}


/*

Packet Sending

*/

void send_not_blocking(sf::TcpSocket* ToSend = nullptr) {
	if (ToSend->send(CurrentPacket) == sf::Socket::Disconnected) {
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
			ToSend->setBlocking(false);
			send_not_blocking(ToSend);
			ToSend->setBlocking(true);
		}
		else
		{
			for (int i = 0; i < clients.size(); ++i) {
				ToSend = clients[i];	
				ToSend->setBlocking(false);
				send_not_blocking(ToSend);
				ToSend->setBlocking(true);
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
	//cout << last_network_status << endl;
	CurrentPacket >> CurrentPacket_header;


	data_size_current += int(CurrentPacket.getDataSize());

	if (for_validating == true)
	{
		validated_connection = false;
		if (CurrentPacket_header == Header_AttemptJoin && CurrentPacket.getDataSize() < 64) //Why would the verification packet be bigger than 64 bytes? It's only username and checksum so.
		{
			cout << blue << "[Client] Receiving verification.." << white << endl;

			string validation;

			CurrentPacket >> username;
			CurrentPacket >> validation;
			if (validation == GAME_VERSION)
			{
				cout << blue << "[Client] " << username << " has passed verification." << white << endl;
				validated_connection = true;
				return;
			}
			cout << blue << "[Client] " << username << " failed verification. Outdated version or invalid chk? V/C = " << validation << white << endl;
			latest_error = "Outdated version";
		}
		return;
	}
	if (!isClient && CurrentPacket.getDataSize() != player_expected_packet_size) //Player only sends things to update their data, so they shouldn't send stuff that big.
	{
		cout << blue << "[Network] Something's weird, " << whoSentThis.getRemoteAddress() << " sent a packet that wasn't " + to_string(player_expected_packet_size) + " bytes! (" << dec << CurrentPacket.getDataSize() << " bytes) Disconnecting!" << white << endl;
		HandleDisconnection(&whoSentThis);
		return;
	}
	/*
	SERVER BEHAVIOUR
	*/
	if (!isClient)
	{

		/*

		UPDATE PLAYER DATA

		*/
		if (CurrentPacket_header == Header_UpdatePlayerData)
		{
			uint_fast8_t PlrNum = 1; CurrentPacket >> PlrNum;
			if (PlrNum > (clients.size()+1) || PlrNum == 0)
			{
				cout << blue << "[Network] Something's weird, " << whoSentThis.getRemoteAddress() << " sent a invalid player packet. Disconnecting!" << white << endl;
				HandleDisconnection(&whoSentThis);
				return; //once again fuck off
			}
			
			PlayerAmount = int(clients.size()); CheckForPlayers();
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
			Sync_Server_RAM(false);
		}

		if (CurrentPacket_header == Header_GlobalUpdate)
		{
			int_fast32_t timestamp = 0;
			CurrentPacket >> timestamp;
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
				);
			latest_server_response = int_fast32_t(ms.count()) - timestamp;
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
			//cout << blue << "[Client] Received connection data." << white << endl;


			CurrentPacket >> PlayerAmount; //Update Plr Amount
			CheckForPlayers(); //have to update the mario list. so it fits.
			//cout << blue << "[Client] Receiving server RAM" << white << endl;
			Sync_Server_RAM(false);
			//cout << blue << "[Client] Receiving music" << white << endl;
			ReceiveMusic(true);
			validated_connection = true;
			cout << blue << "[Client] Received." << white << endl;
		}

		if (CurrentPacket_header == Header_FailedToConnect)
		{
			string msg;
			CurrentPacket >> msg;
			cout << red << "[Network] Received disconnection reason from server : " << msg << white << endl;
			validated_connection = false;
		}

	}
}

bool receive_all_packets(sf::TcpSocket& socket, bool slower = false, bool for_validating = false)
{
	int current_pack = 0;

	while (receiveWithTimeout(socket, CurrentPacket, sf::milliseconds(slower ? 2000 : packet_wait_time), !for_validating) != sf::Socket::NotReady)
	{
		if (last_network_status == sf::Socket::Disconnected)
		{
			if (!isClient) {
				HandleDisconnection(&socket);
				return false;
			}
			else {
				disconnected = true;
				return false;
			}
		}
		current_pack += 1;
		ReceivePacket(socket, for_validating);
		if (isClient && validated_connection)
		{
			return true;
		}
	}
	if (for_validating && !validated_connection)
	{
		return false;
	}
	if (!isClient)
	{
		return !((last_network_status == sf::Socket::Error) || (last_network_status == sf::Socket::Disconnected));
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
		username = "Unknown";

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
			cout << blue << "[Server] " << username << " (" << client->getRemoteAddress() << ", Player " << dec << int(NewPlayerNumber) << ") Timed out or sent invalid information. Disconnecting." << white << endl;
			PreparePacket(Header_FailedToConnect);
			CurrentPacket << latest_error;
			SendPacket(client);

			latest_error = "Unknown";

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
			
			if (clients.size() < 1 || last_network_status == sf::Socket::Error || last_network_status == sf::Socket::Disconnected)
			{
				//cout << blue << "[Server] Attempting to recover from player failure. Will stop communication." << white << endl;
				break;
			}


			PreparePacket(Header_GlobalUpdate);
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
				);

			CurrentPacket << int_fast32_t(ms.count());
			CurrentPacket << PlrNumber;
			pack_mario_data(PlrNumber);

			CurrentPacket << recent_big_change;
			Push_Server_RAM(!recent_big_change);
			SendPacket(&client);




			if (clients.size() < 1 || last_network_status == sf::Socket::Error || last_network_status == sf::Socket::Disconnected)
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
			Push_Server_RAM(false);
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
			receive_all_packets(socketG);
		}

		cout << blue << "[Network] Connected to server. " << int(PlayerAmount) << " player(s) connected." << endl;
		while (!quit && !disconnected)
		{
			receive_all_packets(socketG);

			PreparePacket(Header_UpdatePlayerData); pack_mario_data(); SendPacket();
		}
	}
}

bool ConnectClient(void)
{
	if (socketG.connect(ip, PORT) != sf::Socket::Disconnected)
	{
		PreparePacket(Header_AttemptJoin);
		CurrentPacket << username; //CurrentPacket << da_epical_function_lol();
		CurrentPacket << GAME_VERSION;
		SendPacket();
		sf::sleep(sf::milliseconds(500));
		receive_all_packets(socketG, true);
		validated_connection = false;
		if (disconnected)
		{
			return false;
		}
		CheckForPlayers();
		cout << blue << "[Network] Connected to " << ip << ":" << dec <<  PORT << white << endl;
		return true;
	}
	else
	{
		socketG.disconnect();
		return false;
	}
}