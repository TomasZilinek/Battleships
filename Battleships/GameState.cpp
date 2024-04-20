#include "GameState.h"
#include "iostream"


GameState::GameState(string name, float width, float height): bg_image_sprite(), gamestate_name(name)
{
	resolution[0] = width;
	resolution[1] = height;

	ships_number = 0;
	battlefields_number = 0;
	buttons_number = 0;
	labels_number = 0;
	textentries_number = 0;

	has_background_image = false;
	isactive_bool = false;

	cout << "Gamestate '" + name + "' created" << endl;
}

void GameState::draw(sf::RenderWindow &window)
{
	if (isactive_bool)
	{
		if (has_background_image)
			window.draw(bg_image_sprite);
		for (int i = 0; i < labels_number; i++)
			window.draw(*labels[i]);
		for (int i = 0; i < battlefields_number; i++)
			(*battlefields[i]).draw(window);
		for (int i = 0; i < ships_number; i++)
			(*ship_objects_pointers[i]).draw(window);
		for (int i = 0; i < buttons_number; i++)
			buttons[i]->draw(window);
		for (int i = 0; i < textentries_number; i++)
			text_entries[i]->draw(window);
	}
}

void GameState::setbgImage(sf::Texture& new_texture)
{
	bg_image_sprite.setTexture(new_texture);
	bg_image_sprite.setScale(float(resolution[0]) / float(new_texture.getSize().x), float(resolution[1]) / float(new_texture.getSize().y));
	has_background_image = true;
}

void GameState::processEvent(string event_type, sf::Vector2i mouse_position, int bind_flag)
{
	checkButtonsForEvent(event_type, bind_flag, mouse_position);
	checkBattlefieldsForEvent(event_type, bind_flag, mouse_position);
	checkShipsForEvent(event_type, bind_flag, mouse_position);
	if (event_type == "mouse_over")
		checkEntriesForEvent(mouse_position);
}

void GameState::processEvent(string event_type, int keycode, bool shift_pressed)
{
	if (event_type == "key_pressed")
		checkEntriesForEvent(keycode, shift_pressed);
}

void GameState::checkEntriesForEvent(sf::Vector2i mouse_position)
{
	for (int i = 0; i < text_entries.size(); i++)
		text_entries[i]->processEvent(mouse_position);
}

void GameState::checkEntriesForEvent(uint8_t keycode, bool shift_pressed)
{
	for (int i = 0; i < text_entries.size(); i++)
		text_entries[i]->processEvent(keycode, shift_pressed);
}

void GameState::checkButtonsForEvent(string event_type, int bind_flag, sf::Vector2i mouse_position)
{
	for (int i = 0; i < buttons.size() && isactive_bool; i++)
		buttons[i]->processMouseEvent(event_type, mouse_position);
}

void GameState::checkShipsForEvent(string event_type, int bind_flag, sf::Vector2i mouse_position)
{
	for (int i = 0; i < ships_number; i++)
		ship_objects_pointers[i]->processMouseEvent(event_type, mouse_position);
}

void GameState::checkBattlefieldsForEvent(string event_type, int bind_flag, sf::Vector2i mouse_position)
{
	for (int i = 0; i < battlefields.size(); i++)
		battlefields[i]->processMouseEvent(event_type, mouse_position);
}

void GameState::addWidget(MyButton* new_button)
{
	buttons.push_back(new_button);
	if (isactive_bool)
		new_button->setActivityState(true);
	buttons_number++;
}

void GameState::addWidget(sf::Text* new_label)
{
	labels.push_back(new_label);
	labels_number++;
}

void GameState::addWidget(BattleField* new_battlefield)
{
	battlefields.push_back(new_battlefield);
	battlefields_number++;
}

void GameState::addWidget(TextEntry* new_textentry)
{
	text_entries.push_back(new_textentry);
	textentries_number++;
}

void GameState::setActivityState(bool new_state)
{
	isactive_bool = new_state;
	for (int i = 0; i < buttons.size(); i++)
		buttons[i]->setActivityState(new_state);
}

void GameState::addShip(Ship* new_ship)
{
	if (ships_number < MAX_SHIPS_NUMBER)
	{
		ship_objects_pointers[ships_number] = new_ship;
		ships_number++;
	}
}

BattleField* GameState::getBattlefieldObjectPointer(uint8_t index)
{
	if (index < battlefields.size())
		return battlefields[index];
}

TextEntry* GameState::getTextentryObjectPointer(uint8_t index)
{
	if (index < textentries_number)
		return text_entries[index];
}

GameState::~GameState()
{
	for (int i = 0; i < labels.size(); i++)
		delete labels[i];

	for (int i = 0; i < battlefields.size(); i++)
		delete battlefields[i];

	for (int i = 0; i < buttons.size(); i++)
		delete buttons[i];

	for (int i = 0; i < text_entries.size(); i++)
		delete text_entries[i];

	cout << "Gamestate: " << gamestate_name << " deleted" << endl;
}
