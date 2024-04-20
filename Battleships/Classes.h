#pragma once
#include "SFML\Graphics.hpp"
#include "Widgets.h"
#include "GameState.h"


#ifndef CLASSES_H_
#define CLASSES_H_


class Ship;
class GameState;

class BattleField {
public:
	BattleField(float x, float y, float w);
	~BattleField();
	void draw(sf::RenderWindow &window);
	void processMouseEvent(string event_type, sf::Vector2i mouse_position);
	void removeShip(int** received_positions, int pos_x, int pos_y);
	bool attachShip(Ship* new_ship);
private:
	sf::Font* symbols_font;
	sf::Text* coords_symbols_texts[2][10];
	sf::RectangleShape background;
	sf::RectangleShape grid_background;
	sf::RectangleShape cell_draw;
	sf::Vertex cross_lines[2][2] = { {sf::Vertex(sf::Vector2f(0, 0)), sf::Vertex(sf::Vector2f(0, 0))},
								    {sf::Vertex(sf::Vector2f(0, 0)), sf::Vertex(sf::Vector2f(0, 0))} };
	sf::FloatRect global_bounds;
	int pos_check_shifts[8][2] = { { 0, 1 },{ 1, 1 },{ 1, 0 },{ 1, -1 },{ 0, -1 },{ -1, -1 },{ -1, 0 },{ -1, 1 } };
	bool taken_positions[10][10];
	Ship* ships_pointers[7];
	char chars[11] = "abcdefghij";
	uint16_t pos_x, pos_y, grid_pos_x, grid_pos_y;
	float width, cell_width = 0;
	int pointed_cell[2] = {100, 100};
	int ships_number = 0;
};


class Ship {
public:
	Ship(string inp_ship_type, GameState* inp_my_gamestate);
	~Ship();

	void draw(sf::RenderWindow& window);
	void setPosition(int x, int y);
	void setWidth(int new_width);
	void rotate();
	void removeFromBattlefield();
	void processMouseEvent(string event_type, sf::Vector2i mouse_position);
	void beeingPlaced(bool inp_bool);
	void select();
	bool has_position(int x, int y);
	int** givePositions();
		////variables////
	BattleField* my_battlefield;
	int width = 0;
	int battlefield_coords[2] = {100, 100};
	bool this_ship_selected;
	bool already_placed;
private:
	void createRealCellsPositions();
	void setReprColor(int event_type);
	////variables////
	GameState* my_gamestate;
	enum ship_name_enum {T, U, long_ship, middle_ship, short_ship = 5};
	static enum color_states {normal_col, hover_col, selected_col, placed_col};
	map<string, int> ship_name_to_int;
	MyButton* rotate_button;
	MyButton* delete_button;
	string ship_type;
	uint8_t opacity_level = 150;
	sf::RectangleShape repr_border;
	sf::RectangleShape cell_draw;
	sf::FloatRect global_bounds;
	float screen_coords[2];
	sf::Color color_selection[4][2] = { {sf::Color(255, 255, 255, opacity_level), sf::Color::Black},
										{sf::Color(255, 128, 17, opacity_level + 30), sf::Color::Black},
										{sf::Color(28, 115, 255, opacity_level + 50), sf::Color::Blue},
										{sf::Color(114, 98, 127, opacity_level + 65), sf::Color::Black } };
	int cells_positions[5][2];
	int real_cells_positions[5][2];
	int repr_cells_resolution[2];
	float cell_size;
	static int all_ships_number;
	int rotation_state = 0;
	bool just_button_clicked;
	bool repr_selected;
	bool on_hover_active;
	bool giving_positions;
	bool keep_selected;
};


const uint8_t ships_models[7][6][2] = {
	{ { 0, 0 },{ 0, 1 },{ 0, 2 },{ 1, 1 },{ 100, 100 },{ 4, 5 } },
	{ { 0, 0 },{ 0, 1 },{ 0, 2 },{ 1, 0 },{ 1, 2 },{ 4, 5 } },
	{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 1, 3 },{ 100, 100 },{ 5, 6 } },
	{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 100, 100 },{ 100, 100 },{ 3, 5 } },
	{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 100, 100 },{ 100, 100 },{ 3, 5 } },
	{ { 1, 0 },{ 1, 1 },{ 100, 100 },{ 100, 100 },{ 100, 100 },{ 3, 4 } },
	{ { 1, 0 },{ 1, 1 },{ 100, 100 },{ 100, 100 },{ 100, 100 },{ 3, 4 } }
};

const uint8_t ship_rotations[7][4][5][2] = {
	{
		{ { 0, 0 },{ 0, 1 },{ 0, 2 },{ 1, 1 },{ 100, 100 } },
		{ { 0, 0 },{ 1, 0 },{ 2, 0 },{ 1, 1 },{ 100, 100 } },
		{ { 2, 0 },{ 2, 1 },{ 2, 2 },{ 1, 1 },{ 100, 100 } },
		{ { 0, 2 },{ 1, 2 },{ 2, 2 },{ 1, 1 },{ 100, 100 } }
	},
	{
		{ { 0, 0 },{ 0, 1 },{ 0, 2 },{ 1, 0 },{ 1, 2 } },
		{ { 0, 0 },{ 1, 0 },{ 2, 0 },{ 0, 1 },{ 2, 1 } },
		{ { 1, 0 },{ 2, 0 },{ 2, 1 },{ 2, 2 },{ 1, 2 } },
		{ { 0, 1 },{ 0, 2 },{ 1, 2 },{ 2, 2 },{ 2, 1 } }
	},
	{
		{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 1, 3 },{ 100, 100 } },
		{ { 0, 2 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 100, 100 } },
		{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 1, 3 },{ 100, 100 } },
		{ { 0, 2 },{ 1, 2 },{ 2, 2 },{ 3, 2 },{ 100, 100 } }
	},
	{
		{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 1 },{ 1, 1 },{ 2, 1 },{ 100, 100 },{ 100, 100 } },
		{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 1 },{ 1, 1 },{ 2, 1 },{ 100, 100 },{ 100, 100 } }
	},
	{
		{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 1 },{ 1, 1 },{ 2, 1 },{ 100, 100 },{ 100, 100 } },
		{ { 1, 0 },{ 1, 1 },{ 1, 2 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 1 },{ 1, 1 },{ 2, 1 },{ 100, 100 },{ 100, 100 } }
	},
	{
		{ { 1, 0 },{ 1, 1 },{ 100, 100 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 0 },{ 1, 0 },{ 100, 100 },{ 100, 100 },{ 100, 100 } },
		{ { 1, 0 },{ 1, 1 },{ 100, 100 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 0 },{ 1, 0 },{ 100, 100 },{ 100, 100 },{ 100, 100 } }
	},
	{
		{ { 1, 0 },{ 1, 1 },{ 100, 100 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 0 },{ 1, 0 },{ 100, 100 },{ 100, 100 },{ 100, 100 } },
		{ { 1, 0 },{ 1, 1 },{ 100, 100 },{ 100, 100 },{ 100, 100 } },
		{ { 0, 0 },{ 1, 0 },{ 100, 100 },{ 100, 100 },{ 100, 100 } }
	},
};

#endif