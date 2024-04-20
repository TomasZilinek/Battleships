#include "SFML\Network.hpp"
#include "SFML\Graphics.hpp"
#include "SFML\System\String.hpp"
#include "Game.h"
#include "Classes.h"
#include "Widgets.h"
#include <locale>
#include <iostream>
#include <fstream>


Game::Game() : Window(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Battleships",
	sf::Style::Fullscreen), time_manager()
{
	Window.setVerticalSyncEnabled(true);
	resolution[0] = sf::VideoMode::getDesktopMode().width;
	resolution[1] = sf::VideoMode::getDesktopMode().height;

	sound_holder.load(Sounds::button_click_sound, "resources\\button_click_sound.wav");
	sound_holder.load(Sounds::button_hover_sound, "resources\\button_hover_sound.wav");
	font_holder.load(Fonts::Arial, "resources\\Arial.ttf");
	texture_holder.load(Textures::setname_bg, "resources\\setname_bg.jpg");

	sf::Thread* thread = new sf::Thread( // start thread to load gamestates
		[this]() {
			this->load_resources();
			for (int i = 0; i < GAMESTATES_TO_CREATE_NUMBER; i++)
				if(Window.isOpen())
					createAndSetupGamestate(gamestates_names_to_load[i]);
		}
	);

	thread->launch();
	threads_to_delete.push_back(thread);

	createAndSetupGamestate("setname_gamestate");
	changeGameState("setname_gamestate");
	
	in_match = false;
	has_server = false;
	shift_pressed = false;
	me_shooting = false;

	while (udp_socket.bind(my_port_number) != sf::Socket::Done)
		my_port_number++;

	udp_socket.setBlocking(false);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const sf::Time TimePerFrame = sf::seconds(0.0333f);
	sf::Time last_lapsed;

	while (Window.isOpen())
	{
		processEvents();
		last_lapsed = clock.restart();
		timeSinceLastUpdate += last_lapsed;
		time_manager.setTimeChange(last_lapsed);
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
		}
		render();
	}
}

void Game::processEvents()
{
	sf::IpAddress sender_address;
	sf::Uint16 sender_port = 0;

	while (Window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			Window.close();
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				Window.close();
			else
			{
				if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift)
					shift_pressed = true;
				active_gamestate->processEvent("key_pressed", event.key.code, shift_pressed);
			}
		}
		else if (event.type == sf::Event::KeyReleased)
			if (event.key.code == event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift)
				shift_pressed = false;
		if (event.type == sf::Event::MouseButtonPressed)
			active_gamestate->processEvent("mouse_click", mouse.getPosition(), mouse_click);
		if (event.type == sf::Event::MouseMoved)
			active_gamestate->processEvent("mouse_over", mouse.getPosition(), mouse_over);
	}

	while (udp_socket.receive(packet, sender_address, my_port_number) == sf::Socket::Done)
		processNetworkingEvent(sender_address, sender_port);
}

void Game::render()
{
	Window.clear(sf::Color::White);
	active_gamestate->draw(Window);
	Window.display();
}

void Game::changeGameState(string gamestate_name)
{
	cout << "Changing gamestate to " << gamestate_name << endl;
	if (gamestateExists(gamestate_name))
	{
		active_gamestate = all_gamestates[GameType_to_index_map[gamestate_name]];

		active_buttons_vector.clear();
		vector<MyButton*>* buttons_vect_pointer = active_gamestate->getButtonsVectorPointer();
		for(int i = 0; i < buttons_vect_pointer->size(); i++)
		{
			(*buttons_vect_pointer)[i]->receiveNewGameState(gamestate_name);
			active_buttons_vector.push_back((*buttons_vect_pointer)[i]);
		}

		active_gamestate->setActivityState(true);
	}
}

void Game::load_resources()
{
	texture_holder.load(Textures::credits_bg, "resources\\credits_bg.jpg");
	texture_holder.load(Textures::menu_bg, "resources\\menu_bg.jpg");
	texture_holder.load(Textures::placing_bg, "resources\\placing_bg.jpg");
	texture_holder.load(Textures::rotate_image, "resources\\rotate_image.png");
	texture_holder.load(Textures::rotate_image_hover, "resources\\rotate_image_hover.png");
	texture_holder.load(Textures::red_x, "resources\\red_x.png");
	texture_holder.load(Textures::red_x_hover, "resources\\red_x_hover.png");

	font_holder.load(Fonts::DrivingAround, "resources\\DrivingAround.ttf");

	cout << "resources loaded" << endl;
}

bool Game::gamestateExists(string gamestate_name)
{
	if (GameType_to_index_map.find(gamestate_name) == GameType_to_index_map.end())
		return false;
	return true;
}

void Game::getTextFromEntryAndGoToMenu(TextEntry* entry)
{
	
	player_name = entry->getText();

	if (player_name != "")
	{
		changeGameState("menu_gamestate");

		ofstream write_to_file;
		locale loc("sk-SK");
		write_to_file.open("resources\\player_info.txt");
		write_to_file << player_name.toAnsiString(loc);
		write_to_file.close();
	}
	else
		entry->alert();
}

GameState* Game::createAndSetupGamestate(string gamestate_name)
{
	GameState* new_gamestate = new GameState(gamestate_name, resolution[0], resolution[1]);

	sf::Font& Arial = font_holder.get(Fonts::Arial);
	sf::Font& DrivingAround = font_holder.get(Fonts::DrivingAround);

	sf::Texture& credits_bg = texture_holder.get(Textures::credits_bg);
	sf::Texture& menu_bg = texture_holder.get(Textures::menu_bg);
	sf::Texture& placing_bg = texture_holder.get(Textures::placing_bg);
	sf::Texture& setname_bg = texture_holder.get(Textures::setname_bg);
	sf::Texture& rotate_image = texture_holder.get(Textures::rotate_image);
	sf::Texture& rotate_image_hover = texture_holder.get(Textures::rotate_image_hover);

	sf::SoundBuffer& hover_sound = sound_holder.get(Sounds::button_hover_sound);
	sf::SoundBuffer& click_sound = sound_holder.get(Sounds::button_click_sound);

	/*
	if (gamestate_name == "loading_gamestate")
	{
		new_gamestate->setbgImage(loading_bg);

		sf::Text* loading_label = new sf::Text("Loading", Arial, resolution[1] / 15);
		loading_label->setFillColor(sf::Color::White);
		loading_label->setPosition(resolution[0] / 2 - loading_label->getGlobalBounds().width / 2, resolution[1] * 0.4);
		loading_label->setStyle(sf::Text::Bold);

		new_gamestate->addWidget(loading_label);
	}
	else*/ if (gamestate_name == "setname_gamestate")
	{
		new_gamestate->setbgImage(setname_bg);
		uint8_t fontsize = resolution[1] / 15;

		locale loc("sk-SK");
		wifstream read_from_file;
		read_from_file.imbue(loc);
		read_from_file.open("resources\\player_info.txt");
		
		wstring player_name_r;
		getline(read_from_file, player_name_r);
		
		sf::String pn(player_name_r);

		// widgets:

		sf::Text* nickname_label = new sf::Text("Player name:", Arial, fontsize);
		nickname_label->setFillColor(sf::Color::White);
		nickname_label->setPosition(resolution[0] / 2 - nickname_label->getGlobalBounds().width / 2, resolution[1] / 2 - 3.5 * fontsize);
		nickname_label->setStyle(sf::Text::Bold);

		new_gamestate->addWidget(nickname_label);

		TextEntry* entry = new TextEntry(pn);
		entry->setBackgroundColor(sf::Color(59, 62, 66, 200));
		entry->setBackgroundColorOnHover(sf::Color(107, 111, 117, 200));
		entry->setWidth(resolution[0] * 0.25);
		entry->setTextAndFont(pn, Arial);
		entry->setFontsize(fontsize);
		entry->setTextColor(sf::Color::White);
		entry->setPosition(resolution[0] * 0.75 / 2, resolution[1] / 2 - 2 * fontsize);
		entry->setOutlineColor(sf::Color::Black);
		new_gamestate->addWidget(entry);

		MyButton* submit_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		submit_button->setText("Submit");
		submit_button->setFont(Arial);
		submit_button->setPosition(0, resolution[1] / 2);
		submit_button->setFunction(&Game::getTextFromEntryAndGoToMenu, this, entry);
		submit_button->customize();
		submit_button->setFontsize(fontsize);
		new_gamestate->addWidget(submit_button);

		MyButton* quit_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		quit_button->setText("Quit");
		quit_button->setFont(Arial);
		quit_button->setPosition(0, resolution[1] / 2 + 1.5 * fontsize);
		quit_button->setFunction(&Game::end, this);
		quit_button->customize();
		quit_button->setFontsize(fontsize);
		new_gamestate->addWidget(quit_button);
	}
	else if (gamestate_name == "menu_gamestate")
	{
		new_gamestate->setbgImage(menu_bg);

		for (int i = 0; i < BUTTONS_NUMBER; i++)
		{
			MyButton* new_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
			new_button->setPosition(0, resolution[1] * (0.35 + float(i) / float(BUTTONS_NUMBER + 5)));
			new_gamestate->addWidget(new_button);
		}

		string button_texts[BUTTONS_NUMBER] = { "Play", "Credits", "Quit" };
		vector<MyButton*>* menu_buttons = new_gamestate->getButtonsVectorPointer();

		(*menu_buttons)[0]->setFunction<Game, string>(&Game::changeGameState, this, "ship_placing_gamestate"); // menu
		//(*menu_buttons)[0]->setFunction<Game, string>(&Game::changeGameState, this, "multiplayer_menu_gamestate"); // multiplayer
		(*menu_buttons)[1]->setFunction<Game, string>(&Game::changeGameState, this, "credits_gamestate"); // credits
		(*menu_buttons)[2]->setFunction(&Game::end, this); // quit

		for (int i = 0; i < menu_buttons->size(); i++)
		{
			MyButton* button = (*menu_buttons)[i];
			button->setText(button_texts[i]);
			button->setFont(Arial);
			button->customize();
		}

		sf::Text* title_label = new sf::Text("BATTLESHIPS", DrivingAround, resolution[1] / 8);
		title_label->setFillColor(sf::Color::White);
		title_label->setPosition(resolution[0] / 2 - title_label->getGlobalBounds().width / 2, resolution[1] * 0.15);
		title_label->setStyle(sf::Text::Bold);

		new_gamestate->addWidget(title_label);
	}
	else if (gamestate_name == "ship_placing_gamestate")
	{
		int battlefield_height = resolution[1] * 0.65;
		BattleField* placing_battlefield = new BattleField((resolution[0] - resolution[1] * 0.65) / 2, resolution[1] * 0.1, battlefield_height);
		new_gamestate->addWidget(placing_battlefield);
		new_gamestate->setbgImage(placing_bg);
		uint16_t shift = int(resolution[0] * 0.015);

		sf::Text* title_label = new sf::Text("Place your ships!", Arial, resolution[1] / 22);
		title_label->setPosition(resolution[0] * 0.025, resolution[1] * 0.025);
		title_label->setStyle(sf::Text::Bold);
		title_label->setFillColor(sf::Color::White);
		new_gamestate->addWidget(title_label);

		Ship* new_ship;
		for (int i = 0; i < 7; i++)
		{
			new_ship = new Ship(ship_names[i == 5 || i == 6 ? i == 5 ? 3 : 4 : i], new_gamestate);
			new_ship->setPosition(resolution[0] / 7 * i + shift, battlefield_height * 1.21);
			new_ship->setWidth(resolution[0] / 7 * 0.8);
			new_gamestate->addShip(new_ship);
			placing_battlefield->attachShip(new_ship);
		}

		MyButton* start_battle_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		/*
		start_battle_button->seFillColor(sf::Color(255, 255, 255, 190));
		start_battle_button->setFillColor(sf::Color::Black);
		start_battle_button->setFillColorOnHover(sf::Color::White);
		start_battle_button->setFontsize(40);
		*/
		start_battle_button->setText("Start battle!");
		start_battle_button->setFont(Arial);
		start_battle_button->customize();
		start_battle_button->setPosition(resolution[0] - start_battle_button->getWidth() - 20, resolution[1] * 0.035);
		new_gamestate->addWidget(start_battle_button);

		MyButton* back_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		back_button->setText("Back");
		back_button->setFont(Arial);
		back_button->customize();
		back_button->setPosition(resolution[0] - back_button->getWidth() - 20, resolution[1] * 0.11);
		back_button->setFunction<Game, string>(&Game::changeGameState, this, "menu_gamestate");
		new_gamestate->addWidget(back_button);

	}
	else if (gamestate_name == "multiplayer_menu_gamestate")
	{
		new_gamestate->setbgImage(menu_bg);
		float font_size = resolution[1] * 0.05;

		MyButton* back_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		back_button->setFont(Arial);
		back_button->setText("back");
		back_button->customize();
		back_button->setPosition(resolution[0] * 0.035, resolution[1] * 0.03);
		back_button->setFunction<Game, string>(&Game::changeGameState, this, "menu_gamestate");
		new_gamestate->addWidget(back_button);

		MyButton* create_game_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		create_game_button->setPosition(0, resolution[1] / 2 - font_size * 1.5);
		//create_game_button->setFunction(create_game_server);
		create_game_button->setText("Create game");
		create_game_button->setFont(Arial);
		create_game_button->customize();
		new_gamestate->addWidget(create_game_button);

		MyButton* search_games_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		search_games_button->setPosition(0, resolution[1] / 2 + font_size * 0.5);
		//search_games_button->setFunction(search_for_games);
		search_games_button->setText("Search games");
		search_games_button->setFont(Arial);
		search_games_button->customize();
		new_gamestate->addWidget(search_games_button);
	}
	else if (gamestate_name == "credits_gamestate")
	{
		new_gamestate->setbgImage(credits_bg);
		//back button
		MyButton* back_button = new MyButton(gamestate_name, "text", hover_sound, click_sound);
		back_button->setFont(Arial);
		back_button->setText("back");
		back_button->customize();
		back_button->setPosition(resolution[0] * 0.035, resolution[1] * 0.03);
		back_button->setFunction<Game, string>(&Game::changeGameState, this, "menu_gamestate");
		new_gamestate->addWidget(back_button);

		//title label
		sf::Text* title_label = new sf::Text("Credits", Arial, resolution[1] / 15);
		title_label->setStyle(sf::Text::Bold);
		title_label->setFillColor(sf::Color::White);
		title_label->setPosition((resolution[0] - title_label->getGlobalBounds().width) / 2, resolution[1] * 0.2);

		new_gamestate->addWidget(title_label);
	}
	else if (gamestate_name == "search_gamestate")
	{

	}
	else
	{
		cout << endl << "Cannot create GameState with name '" + gamestate_name + "', no such name exists." << endl;
		throw logic_error("");
	}
		

	GameType_to_index_map[gamestate_name] = GameType_to_index_map.size();
	all_gamestates.push_back(new_gamestate);

	return new_gamestate;
}

void Game::processNetworkingEvent(sf::IpAddress addr, sf::Uint16 port)
{
	uint8_t event_type;
	if (receiveFromPacket(packet, event_type))
	{
		if (event_type == Net::connection_approved) // connected to server
		{
			packet >> me_shooting;
			in_match = true;
			cout << "connection approved, starting: " << me_shooting << endl;
		}
		else if (event_type == Net::broadcast_server_presence && active_gamestate->getName() == "search_gamestate")
		{

		}
	}
}

void Game::end()
{
	active_gamestate->setActivityState(false);
	Window.close();
}

Game::~Game()
{
	for (int i = 0; i < all_gamestates.size(); i++)
		delete all_gamestates[i];
	for (int i = 0; i < threads_to_delete.size(); i++)
		delete threads_to_delete[i];
	all_gamestates.clear();
}
