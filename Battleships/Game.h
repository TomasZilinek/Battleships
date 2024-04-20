#pragma once
#include "SFML\Graphics.hpp"
#include "SFML\Network.hpp"
#include "GameState.h"
#include "FunctionHolder.h"
#include "StructureClasses.h"
#include <vector>

#ifndef GAME_H_
#define GAME_H_

const uint8_t GAMESTATES_TO_CREATE_NUMBER = 5;

namespace Net {
	enum network_events {
		broadcast_server_presence, try_connecting_to_server, connection_approved, send_ready_state, start_game, end_of_game, shoot
	};
}

class Game {
public:
	Game();
	~Game();
	void run();
	ResourceHolder<sf::Texture, Textures::ID>	texture_holder;
	ResourceHolder<sf::SoundBuffer, Sounds::ID>		sound_holder;
	ResourceHolder<sf::Font, Fonts::ID>			font_holder;
	TimeManager time_manager;
private:
	void end();
	void processEvents();
	void render();
	void getTextFromEntryAndGoToMenu(TextEntry* entry);
	void load_resources();
	bool gamestateExists(string gamestate_name);
	void changeGameState(string gamestate_name);
	void processNetworkingEvent(sf::IpAddress addr, sf::Uint16 port);
	template<typename T> bool receiveFromPacket(sf::Packet& pack, T& container);
	GameState* createAndSetupGamestate(string gamestate_name);
		////variables////
	enum button_flags_names { mouse_click, mouse_over };
	string ship_names[5] = { "T_ship", "U_ship", "long_ship", "middle_ship", "short_ship" };
	string gamestates_names_to_load[GAMESTATES_TO_CREATE_NUMBER] = { "menu_gamestate" , "multiplayer_menu_gamestate", "credits_gamestate",
																	 "search_gamestate", "ship_placing_gamestate" };
	sf::String player_name;
	bool in_match;
	bool has_server;
	bool shift_pressed;
	bool me_shooting;
	map <string, int> GameType_to_index_map;
	vector <MyButton*> active_buttons_vector;
	vector <GameState*> all_gamestates;
	vector <sf::Thread*> threads_to_delete;
	GameState* active_gamestate;
	uint16_t resolution[2];
	sf::Event event;
	sf::Mouse mouse;
	sf::RenderWindow Window;
	sf::UdpSocket udp_socket;
	sf::Packet packet;

	// Packet communication formats:
	// 
	// broadcast_server_presence -> [string action_type, string my_ip_address, sf::Uint16 my_port, string server_name]
	// try_connecting_to_server -> [string action_type, string my_ip_address, sf::Uint16 my_port, string player_name]
	// connection_approved -> [string action_type, bool me_starting]
	// send_ready_state -> [string action_type, bool ready_state]
	// start_game -> [string action_type]
	// end_of_game -> [string action_type]
	// shoot -> [string action_type, sf::Uint16 pos_x, sf::Uint16 pos_y]

	sf::Uint16 my_port_number = 50000, recipient_port_number = 50000;
};

template<typename T>
bool Game::receiveFromPacket(sf::Packet& pack, T& container)
{
	if (pack >> container)
		return true;
	return false;
}

#endif