#pragma once
#include "SFML\Graphics.hpp"
#include "Classes.h"
#include <vector>

#ifndef GAMESTATE_H_
#define GAMESTATE_H_

const uint8_t BUTTONS_NUMBER = 3;
const uint8_t MAX_SHIPS_NUMBER = 7;

class Ship;
class BattleField;

class GameState {
public:
	GameState(string name, float width, float height);
	~GameState();
	// add things
	void addWidget(MyButton* new_button);
	void addWidget(sf::Text* new_label);
	void addWidget(BattleField* new_battlefield);
	void addWidget(TextEntry* new_textentry);
	void addShip(Ship* new_ship);
	// events
	void processEvent(string event_type, sf::Vector2i mouse_position, int bind_flag);
	void processEvent(string event_type, int keycode, bool shift_pressed = false);
	void checkButtonsForEvent(string event_type, int bind_flag, sf::Vector2i mouse_position);
	void checkShipsForEvent(string event_type, int bind_flag, sf::Vector2i mouse_position);
	void checkBattlefieldsForEvent(string event_type, int bind_flag, sf::Vector2i mouse_position);
	void checkEntriesForEvent(sf::Vector2i mouse_position);
	void checkEntriesForEvent(uint8_t keycode, bool shift_pressed);
	void setActivityState(bool new_state);
	// others
	void draw(sf::RenderWindow& window);
	void setbgImage(sf::Texture& new_texture);
	BattleField* getBattlefieldObjectPointer(uint8_t index);
	TextEntry* getTextentryObjectPointer(uint8_t index);
	// inlines
	vector<MyButton*>* getButtonsVectorPointer() { return &buttons; }
	string getName() { return gamestate_name; }
	bool isActive() { return isactive_bool; }
	// variables
	uint8_t ships_number;
	uint8_t battlefields_number;
	uint8_t buttons_number;
	uint8_t labels_number;
	uint8_t textentries_number;
private:
	enum button_flags_names { mouse_click, mouse_over };
	string gamestate_name;
	bool has_background_image;
	bool isactive_bool;
	int resolution[2];
	vector<BattleField*> battlefields;
	vector<MyButton*> buttons;
	vector<TextEntry*> text_entries;
	vector<sf::Text*> labels;
	Ship* ship_objects_pointers[7];
	sf::Sprite bg_image_sprite;
};

#endif