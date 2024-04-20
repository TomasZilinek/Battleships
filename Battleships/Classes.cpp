#include "Classes.h"
#include "Game.h"
#include "SFML\Graphics.hpp"
#include <iostream>

extern Game game;


BattleField::BattleField(float x, float y, float w): background(sf::Vector2f(w + 2 * w / 11, w + 2 * w / 11 )), cell_draw(), global_bounds(),
		grid_background()
{
	pos_x = int(x);
	pos_y = int(y);
	width = int(w);
	cell_width = float(width) / 11;
	grid_pos_x = pos_x + cell_width;
	grid_pos_y = pos_y + cell_width;

	background.setPosition(x - cell_width, y - cell_width);
	background.setFillColor(sf::Color(150, 150, 150, 210));
	global_bounds = background.getGlobalBounds();

	grid_background.setSize(sf::Vector2f(cell_width * 10, cell_width * 10));
	grid_background.setPosition(x + cell_width, y + cell_width);
	grid_background.setFillColor(sf::Color(63, 135, 252, 80));

	cross_lines[0][0].color = sf::Color(135, 0, 0);
	cross_lines[0][1].color = sf::Color(135, 0, 0);
	cross_lines[1][0].color = sf::Color(135, 0, 0);
	cross_lines[1][1].color = sf::Color(135, 0, 0);

	symbols_font = new sf::Font();
	if (!symbols_font->loadFromFile("resources\\Arial.ttf"))
		throw logic_error("Unable to load font arial.ttf from file (in battlefield object)");

	sf::Text* text_ptr;
	cell_draw.setFillColor(sf::Color::Green);
	cell_draw.setSize(sf::Vector2f(cell_width - 2, cell_width - 2));

	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++)
			taken_positions[x][y] = false;

	//setting numbers and letters
	for (int i = 0; i < 10; i++)
	{
		text_ptr = new sf::Text(chars[i], *symbols_font, int(width / 15));
		text_ptr->setPosition(pos_x + (i + 1.31)* cell_width, pos_y);
		coords_symbols_texts[0][i] = text_ptr;

		text_ptr = new sf::Text(to_string(i + 1), *symbols_font, int(width / 15));
		text_ptr->setPosition(pos_x + cell_width / (i > 8 ? 10:3), pos_y + (i + 1) * cell_width);
		coords_symbols_texts[1][i] = text_ptr;
	}

	//cout << "BattleField created" << endl;
}

void BattleField::draw(sf::RenderWindow &window)
{
	sf::Vertex line[2] = { sf::Vertex(), sf::Vertex() };
	line[0].color = sf::Color::Black;
	line[1].color = sf::Color::Black;
	unsigned short int lines_n = 11;

	window.draw(background);
	window.draw(grid_background);

	//draw  texts
	for (int i = 0; i < lines_n + 1; i++)
	{
		line[0].position = sf::Vector2f(pos_x, pos_y + i * cell_width);
		line[1].position = sf::Vector2f(pos_x + width, pos_y + i * cell_width);
		window.draw(line, 2, sf::Lines);
		if (i < 10)
		{
			window.draw(*coords_symbols_texts[0][i]);
			window.draw(*coords_symbols_texts[1][i]);
		}
	}
	// draw the grid
	for (int i = 0; i < lines_n + 1; i++)
	{
		line[0].position = sf::Vector2f(pos_x + i * cell_width, pos_y);
		line[1].position = sf::Vector2f(pos_x + i * cell_width, pos_y + width);
		window.draw(line, 2, sf::Lines);
	}
	//draw cells of placed ships
	cell_draw.setFillColor(sf::Color::Green);
	for (int y = 0; y < 10; y++)
		for (int x = 0; x < 10; x++)
			if (taken_positions[x][y])
			{
				cell_draw.setPosition(sf::Vector2f(grid_pos_x + x * cell_width + 2, grid_pos_y + y * cell_width + 2));
				window.draw(cell_draw);
			}

	bool something_selected = false;
	int** received_positions = nullptr;
	Ship* selected_ship = nullptr;
	for (int i = 0; i < ships_number; i++)
		if (ships_pointers[i]->this_ship_selected)
		{
			received_positions = ships_pointers[i]->givePositions();
			something_selected = true;
			selected_ship = ships_pointers[i];
			break;
		}
	
	if (something_selected)
	{
		sf::Vector2f draw_vect;

		// draw cells if mouse over
		if (pointed_cell[0] >= 0 && pointed_cell[1] >= 0 && !selected_ship->already_placed)
		{
			bool well_pointed = true; //the ship is whole in the battlefield
			bool taken_cell_nearby = false;
			float currant_draw_pos[2];
			
			for (int i = 0; i < 5; i++)
				if (received_positions[i][0] != 100 && (pointed_cell[0] + received_positions[i][0] >= 10 || pointed_cell[1] + received_positions[i][1] >= 10))
				{
					well_pointed = false;
					break;
				}
			
			if (well_pointed)
			{
				cell_draw.setFillColor(sf::Color::Green);
				uint8_t shifted_x, shifted_y;
				for (int i = 0; i < 5; i++)
				{
					taken_cell_nearby = false;
					if (received_positions[i][0] != 100 && pointed_cell[0] + received_positions[i][0] < 10 && pointed_cell[1] + received_positions[i][1] < 10)
					{
						currant_draw_pos[0] = grid_pos_x + (pointed_cell[0] + received_positions[i][0]) * cell_width + 2;
						currant_draw_pos[1] = grid_pos_y + (pointed_cell[1] + received_positions[i][1]) * cell_width + 2;
						draw_vect.x = currant_draw_pos[0];
						draw_vect.y = currant_draw_pos[1];
						cell_draw.setPosition(draw_vect);
						for (int shift_n = 0; shift_n < 8; shift_n++)
						{
							shifted_x = pointed_cell[0] + received_positions[i][0] + pos_check_shifts[shift_n][0];
							shifted_y = pointed_cell[1] + received_positions[i][1] + pos_check_shifts[shift_n][1];
							if (shifted_x >= 0 && shifted_y >= 0 && shifted_y < 10 && shifted_x < 10)
								if (taken_positions[shifted_x][shifted_y])
								{
									taken_cell_nearby = true;
									cell_draw.setFillColor(sf::Color::Red);
									break;
								}
						}
						window.draw(cell_draw);
						if (taken_cell_nearby)
						{
							cross_lines[0][0].position = sf::Vector2f(currant_draw_pos[0], currant_draw_pos[1]);
							cross_lines[0][1].position = sf::Vector2f(currant_draw_pos[0] + cell_width, currant_draw_pos[1] + cell_width);
							cross_lines[1][0].position = sf::Vector2f(currant_draw_pos[0] + cell_width, currant_draw_pos[1]);
							cross_lines[1][1].position = sf::Vector2f(currant_draw_pos[0], currant_draw_pos[1] + cell_width);
							window.draw(cross_lines[0], 2, sf::Lines);
							window.draw(cross_lines[1], 2, sf::Lines);
						}
						cell_draw.setFillColor(sf::Color::Green);
					}
				}
			}
			else
			{
				cell_draw.setFillColor(sf::Color::Red);
				for (int i = 0; i < 5; i++)
				{
					if (received_positions[i][0] != 100 && pointed_cell[0] + received_positions[i][0] < 10 && pointed_cell[1] + received_positions[i][1] < 10)
					{
						currant_draw_pos[0] = grid_pos_x + (pointed_cell[0] + received_positions[i][0]) * cell_width + 1;
						currant_draw_pos[1] = grid_pos_y + (pointed_cell[1] + received_positions[i][1]) * cell_width + 1;
						cell_draw.setPosition(sf::Vector2f(currant_draw_pos[0], currant_draw_pos[1]));
						window.draw(cell_draw);
					}
				}
			}
		}
		//draw selected ship cells
		if (selected_ship->already_placed)
		{
			cell_draw.setFillColor(sf::Color(191, 255, 30));
			for (int i = 0; i < 5; i++)
			{
				draw_vect.x = grid_pos_x + (selected_ship->battlefield_coords[0] + received_positions[i][0]) * cell_width + 2;
				draw_vect.y = grid_pos_y + (selected_ship->battlefield_coords[1] + received_positions[i][1]) * cell_width + 2;
				cell_draw.setPosition(draw_vect);
				window.draw(cell_draw);
				
			}
		}
		delete[] received_positions;
	}
}

void BattleField::processMouseEvent(string event_type, sf::Vector2i mouse_position)
{
	if (global_bounds.contains(sf::Vector2f(mouse_position)))
	{
		if (event_type == "mouse_click" && pointed_cell[0] >= 0 && pointed_cell[1] >= 0)
		{
			bool something_selected = false;
			int** received_positions = nullptr;
			Ship* selected_ship = nullptr;
			for (int i = 0; i < ships_number; i++)
				if (ships_pointers[i]->this_ship_selected && !ships_pointers[i]->already_placed)
				{
					selected_ship = ships_pointers[i];
					received_positions = ships_pointers[i]->givePositions();
					something_selected = true;
					break;
				}

			if (something_selected)
			{
				bool well_pointed = true;

				for (int i = 0; i < 5; i++)
					if (received_positions[i][0] != 100 && (pointed_cell[0] + received_positions[i][0] >= 10 || pointed_cell[1] + received_positions[i][1] >= 10))
					{
						well_pointed = false;
						break;
					}
				if (well_pointed)
				{
					uint8_t shifted_x, shifted_y;
					for (int i = 0; i < 5; i++)
						if (received_positions[i][0] != 100 && pointed_cell[0] + received_positions[i][0] < 10 && pointed_cell[1] + received_positions[i][1] < 10)
							for (int shift_n = 0; shift_n < 8; shift_n++)
							{
								shifted_x = pointed_cell[0] + received_positions[i][0] + pos_check_shifts[shift_n][0];
								shifted_y = pointed_cell[1] + received_positions[i][1] + pos_check_shifts[shift_n][1];
								if (shifted_x >= 0 && shifted_y >= 0 && shifted_y < 10 && shifted_x < 10 && taken_positions[shifted_x][shifted_y])
									return;
							}

					for (int i = 0; i < 5; i++)
						if (received_positions[i][0] != 100)
							taken_positions[pointed_cell[0] + received_positions[i][0]][pointed_cell[1] + received_positions[i][1]] = true;

					selected_ship->beeingPlaced(true);
					selected_ship->battlefield_coords[0] = pointed_cell[0];
					selected_ship->battlefield_coords[1] = pointed_cell[1];
					delete[] received_positions;
				}
			}
			else if (taken_positions[pointed_cell[0]][pointed_cell[1]])
			{
				for (int i = 0; i < ships_number; i++)
				{
					if (ships_pointers[i]->has_position(pointed_cell[0], pointed_cell[1]))
					{
						ships_pointers[i]->select();
						return;
					}
				}
			}
				
		}
		else if (event_type == "mouse_over")
		{
			pointed_cell[0] = (mouse_position.x - grid_pos_x) / cell_width;
			pointed_cell[1] = (mouse_position.y - grid_pos_y) / cell_width;
		}
	}
}

void BattleField::removeShip(int** received_positions, int pos_x, int pos_y)
{
	for (int i = 0; i < 5; i++)
		if (received_positions[i][0] != 100)
			taken_positions[pos_x + received_positions[i][0]][pos_y + received_positions[i][1]] = false;
}

bool BattleField::attachShip(Ship* new_ship)
{
	if (ships_number < 7)
	{
		ships_pointers[ships_number] = new_ship;
		new_ship->my_battlefield = this;
		ships_number++;
		return true;
	}
	return false;
}

BattleField::~BattleField()
{
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 10; j++)
			delete coords_symbols_texts[i][j];
	delete symbols_font;
	cout << "BattleField deleted" << endl;
}


			//////////////////////////////////////
			///////////// Ship class /////////////
			//////////////////////////////////////


int Ship::all_ships_number = 0;

Ship::Ship(string inp_ship_type, GameState* inp_my_gamestate) : ship_type(inp_ship_type)
{
	just_button_clicked = false;
	repr_selected = false;
	on_hover_active = false;
	giving_positions = false;
	keep_selected = false;
	this_ship_selected = false;
	already_placed = false;
	my_gamestate = inp_my_gamestate;

	all_ships_number++;
	ship_name_to_int["T_ship"] = T;
	ship_name_to_int["U_ship"] = U;
	ship_name_to_int["long_ship"] = long_ship;
	ship_name_to_int["middle_ship"] = middle_ship;
	ship_name_to_int["short_ship"] = short_ship;
	repr_border.setOutlineColor(sf::Color::Black);
	repr_border.setOutlineThickness(2);
	repr_border.setFillColor(sf::Color::White);

	sf::SoundBuffer& hover_sound = game.sound_holder.get(Sounds::button_hover_sound);
	sf::SoundBuffer& click_sound = game.sound_holder.get(Sounds::button_click_sound);

	rotate_button = new MyButton("ship_placing_gamestate", "image", hover_sound, click_sound);
	rotate_button->setImage(game.texture_holder.get(Textures::rotate_image));
	rotate_button->setImageOnHover(game.texture_holder.get(Textures::rotate_image_hover));
	rotate_button->setFunction(&Ship::rotate, this);
	my_gamestate->addWidget(rotate_button);

	delete_button = new MyButton("ship_placing_gamestate", "image", hover_sound, click_sound);
	delete_button->setImage(game.texture_holder.get(Textures::red_x));
	delete_button->setImageOnHover(game.texture_holder.get(Textures::red_x_hover));
	delete_button->setFunction(&Ship::removeFromBattlefield, this);
	my_gamestate->addWidget(delete_button);

	for (int i = 0; i < 5; i++)
		for (int pos = 0; pos < 2; pos++)
			cells_positions[i][pos] = ships_models[ship_name_to_int[inp_ship_type]][i][pos];

	createRealCellsPositions();
	
	repr_cells_resolution[0] = ships_models[ship_name_to_int[inp_ship_type]][5][0];
	repr_cells_resolution[1] = ships_models[ship_name_to_int[inp_ship_type]][5][1];

	cell_draw.setOutlineThickness(2);
	cell_draw.setOutlineColor(sf::Color::Black);
	cell_draw.setFillColor(sf::Color(0, 178, 14));
	cout << "Ship created" << ", type: " << inp_ship_type << endl;
}

void Ship::draw(sf::RenderWindow& window)
{
	window.draw(repr_border);

	int i = 0;
	float x_shift = 1;

	while (i < 5 && real_cells_positions[i][0] != 100)
	{
		cell_draw.setPosition(screen_coords[0] + (real_cells_positions[i][0] + x_shift) * cell_size + 2 * real_cells_positions[i][0],
							  screen_coords[1] + (real_cells_positions[i][1] + 1) * cell_size + 2 * real_cells_positions[i][1]);
		window.draw(cell_draw);
		i++;
	}
}

void Ship::setPosition(int x, int y)
{
	screen_coords[0] = x;
	screen_coords[1] = y;
	repr_border.setPosition(x, y);
	//cout << "ship " << ship_type << " coords: " << x << " " << y << endl;
}

void Ship::setWidth(int new_width)
{
	width = new_width;
	repr_border.setSize(sf::Vector2f(new_width, new_width));
	global_bounds = repr_border.getGlobalBounds();

	cell_size = float(repr_border.getSize().x) / float(max(repr_cells_resolution[0], repr_cells_resolution[1]));
	cell_draw.setSize(sf::Vector2f(cell_size, cell_size));

	float button_width = repr_border.getSize().x / 5;
	rotate_button->setImageResolution(button_width, button_width);
	rotate_button->setPosition(repr_border.getPosition().x + repr_border.getSize().x - button_width + 1, repr_border.getPosition().y);

	delete_button->setImageResolution(button_width, button_width);
	delete_button->setPosition(repr_border.getPosition().x + repr_border.getSize().x - button_width + 1, repr_border.getPosition().y + button_width * 1.1);
}

void Ship::rotate()
{
	removeFromBattlefield();
	if (++rotation_state == 4)
		rotation_state = 0;

	for (int i = 0; i < 5; i++)
		for (int pos = 0; pos < 2; pos++)
			cells_positions[i][pos] = ship_rotations[ship_name_to_int[ship_type]][rotation_state][i][pos];

	createRealCellsPositions();

	//cout << "ship " << ship_type << " rotated" << endl;
}

void Ship::removeFromBattlefield()
{
	if (already_placed)
	{
		my_battlefield->removeShip(givePositions(), battlefield_coords[0], battlefield_coords[1]);
		setReprColor(normal_col);
		already_placed = false;
		//cout << "ship " << ship_type << " removed from battlefield" << endl;
	}
}

void Ship::processMouseEvent(string event_type, sf::Vector2i mouse_position)
{
	if (event_type == "mouse_click" && !rotate_button->contains(sf::Vector2f(mouse_position)) &&
		!delete_button->contains(sf::Vector2f(mouse_position)))
	{
		if (global_bounds.contains(sf::Vector2f(mouse_position)))
		{
			setReprColor(selected_col);
			this_ship_selected = true;
		}
		else
		{
			if (!keep_selected)
			{
				if (already_placed)
					setReprColor(placed_col);
				else
					setReprColor(normal_col);
			}

			this_ship_selected = false;
			if (keep_selected)
				this_ship_selected = true;

			keep_selected = false;
		}
	}
	else if (event_type == "mouse_over")
	{
		if (!this_ship_selected && !already_placed)
		{
			if (global_bounds.contains(sf::Vector2f(mouse_position)))
			{
				if (!on_hover_active)
				{
					repr_border.setOutlineThickness(4);
					setReprColor(hover_col);
				}
			}
			else
			{
				repr_border.setOutlineThickness(2);
				setReprColor(normal_col);
			}
		}
	}
}

int** Ship::givePositions()
{
	int ** output_arr = new int*[5];

	for (int h = 0; h < 5; h++)
	{
		output_arr[h] = new int[2];
		for (int w = 0; w < 2; w++)
			output_arr[h][w] = real_cells_positions[h][w];
	}

	return output_arr;
}

void Ship::createRealCellsPositions()
{
	int min_x_pos = 50, min_y_pos = 50;
	for (int i = 0; i < 5; i++)
		for (int pos = 0; pos < 2; pos++)
		{
			if (cells_positions[i][0] < min_x_pos)
				min_x_pos = cells_positions[i][0];
			if (cells_positions[i][1] < min_y_pos)
				min_y_pos = cells_positions[i][1];
		}

	for (int i = 0; i < 5; i++)
	{
		if (cells_positions[i][0] != 100)
		{
			real_cells_positions[i][0] = cells_positions[i][0] - min_x_pos;
			real_cells_positions[i][1] = cells_positions[i][1] - min_y_pos;
		}
		else
		{
			real_cells_positions[i][0] = 100;
			real_cells_positions[i][1] = 100;
		}
	}
}

void Ship::beeingPlaced(bool inp_bool)
{
	if (inp_bool)
	{
		already_placed = true;
		setReprColor(placed_col);
	}
	else
	{
		already_placed = false;
		setReprColor(normal_col);
	}
}

void Ship::select()
{
	this_ship_selected = true;
	setReprColor(selected_col);
	keep_selected = true;
}

bool Ship::has_position(int x, int y)
{
	if(battlefield_coords[0] != 100)
		for (int i = 0; i < 5; i++)
			if (battlefield_coords[0] + real_cells_positions[i][0] == x && battlefield_coords[1] + real_cells_positions[i][1] == y)
				return true;
	return false;
}

void Ship::setReprColor(int event_type)
{
	repr_border.setFillColor(color_selection[event_type][0]);
	repr_border.setOutlineColor(color_selection[event_type][1]);
}

Ship::~Ship()
{
	cout << "Ship deleted" << endl;
}
