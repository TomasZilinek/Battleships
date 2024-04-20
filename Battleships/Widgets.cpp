#include "Widgets.h"
#include "Game.h"
#include "iostream"


extern Game game;


MyButton::MyButton(string inp_game_state, string button_type, sf::SoundBuffer& hover_sound, sf::SoundBuffer& click_sound)
		: isactive_bool(false), type(button_type), sprite(), font(), text(), game_state(inp_game_state)
{
	if (type != "image" && type != "text")
		throw invalid_argument("Invalid button type (possible button types: 'text', 'image')");

	sounds[0].setBuffer(hover_sound);
	sounds[1].setBuffer(click_sound);

	flags_names_map["mouse_click"] = mouse_button_press;
	flags_names_map["mouse_over"] = mouse_over;

	resolution[0] = sf::VideoMode::getDesktopMode().width;
	resolution[1] = sf::VideoMode::getDesktopMode().height;
	font_size = resolution[1] / 20;

	is_visible = true;
	has_function = false;
	is_centered = false;
	
	pos_x = 0;
	pos_y = 0;
	internal_padding = 15;

	background_rect.setOutlineColor(sf::Color::Black);

	//cout << "Button created -> my constructor: " << button_type << endl;
}

void MyButton::setFontsize(float new_size)
{
	if (type == "text")
	{
		font_size = new_size;
		text.setCharacterSize(new_size);
		internal_padding = new_size * 0.25;
		text.setPosition(pos_x + internal_padding - font_size * 0.05,
			pos_y + internal_padding - font_size * 0.275);
		background_rect.setSize(
			sf::Vector2f(
				text.getGlobalBounds().width + 
				2 * internal_padding,
			text.getGlobalBounds().height + 2 *
				internal_padding));
		global_bounds = background_rect.getGlobalBounds();
		if (is_centered)
			horizontalCenter();
	}
}

void MyButton::setImage(sf::Texture& new_texture)
{
	if (type == "image")
	{
		sprite.setTexture(new_texture);
		global_bounds = sprite.getGlobalBounds();
	}
}

void MyButton::setText(string inp_text)
{
	if (type == "text")
	{
		text.setString(inp_text);
		
		float text_width = text.getGlobalBounds().width;
		float text_height = text.getGlobalBounds().height;

		background_rect.setSize(sf::Vector2f(text_width + 2 * internal_padding,
											 text_height + 2 * internal_padding));
		text.setPosition(pos_x + internal_padding - font_size * 0.05,
						 pos_y + internal_padding - font_size * 0.275);	
		global_bounds = background_rect.getGlobalBounds();
		if (is_centered)
			horizontalCenter();
		//cout << "text: " << inp_text << " is set for button" << endl;
	}
	else
		throw logic_error("Unable to set text color for image button");
}

void MyButton::draw(sf::RenderWindow &window)
{
	if (isactive_bool && is_visible)
	{
		if (type == "text")
		{
			window.draw(background_rect);
			window.draw(text);
		}
		else if (type == "image")
		{
			if (applied_mouse_interaction_flags[mouse_over])
				window.draw(sprite_on_hover);
			else
				window.draw(sprite);
		}
	}
}

void MyButton::setColor(sf::Color inp_color)
{
	if (type == "text")
	{
		text.setFillColor(inp_color);
		colors[normal_col] = inp_color;
	}
	else
		throw logic_error("Unable to set text color for image button");
}

void MyButton::setPosition(float inp_pos_x, float inp_pos_y)
{
	pos_x = inp_pos_x;
	pos_y = inp_pos_y;
	if (type == "image")
	{
		sprite.setPosition(sf::Vector2f(pos_x, pos_y));
		sprite_on_hover.setPosition(sprite.getPosition());
		global_bounds = sprite.getGlobalBounds();
	}
	else if (type == "text")
	{
		text.setPosition(pos_x + internal_padding - font_size * 0.05,
						 pos_y + internal_padding - font_size * 0.275);
		background_rect.setPosition(pos_x, pos_y);
		global_bounds = text.getGlobalBounds();
		is_centered = false;
	}
}

void MyButton::setBackgroundColor(sf::Color new_bg_color)
{
	colors[rect_background_col] = new_bg_color;
	background_rect.setFillColor(new_bg_color);
}

void MyButton::receiveNewGameState(string new_game_state)
{
	if (new_game_state == game_state)
		isactive_bool = true;
	else
		isactive_bool = false;

}

bool MyButton::hasActiveBind(int flag)
{
	if (active_mouse_interaction_flags[flag])
		return true;
	else
		return false;
}

bool MyButton::processMouseEvent(string event_type, sf::Vector2i mouse_position)
{
	if (isactive_bool)
	{
		if (event_type == "mouse_click" && global_bounds.contains(sf::Vector2f(mouse_position)) && has_function)
		{
			sounds[1].play();
			function_holder->run();
			return true;
		}
		else if (event_type == "mouse_over")
		{
			if (global_bounds.contains(sf::Vector2f(mouse_position)))
			{
				if (!applied_mouse_interaction_flags[mouse_over])
				{
					if (type == "text")
						text.setFillColor(colors[on_hover_col]);
					applied_mouse_interaction_flags[mouse_over] = true;
					sounds[0].play();
				}
				return true;
			}
			else if (applied_mouse_interaction_flags[mouse_over])
			{
				if (type == "text")
					text.setFillColor(colors[normal_col]);
				applied_mouse_interaction_flags[mouse_over] = false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

void MyButton::setColorOnHover(sf::Color inp_color)
{
	colors[on_hover_col] = inp_color;
	active_mouse_interaction_flags[mouse_over] = true;
}

void MyButton::setImageOnHover(sf::Texture& new_hover_texture)
{
	sprite_on_hover.setTexture(new_hover_texture);
	active_mouse_interaction_flags[1] = true;
}

void MyButton::setActivityState(bool state)
{
	isactive_bool = state;
	/*
	if (state)
		cout << "button with an image is now active" << endl;
	else
		cout << "button with an image is now inactive" << endl;*/
}

void MyButton::horizontalCenter()
{
	if (type == "text")
	{
		pos_x = (resolution[0] - background_rect.getSize().x) / 2;
		background_rect.setPosition(pos_x, pos_y);
		text.setPosition(pos_x + internal_padding - font_size * 0.05, 
						 pos_y + internal_padding - font_size * 0.275);
		global_bounds = background_rect.getGlobalBounds();
		is_centered = true;
	}
}

void MyButton::setImageResolution(int x, int y)
{
	if (type == "image")
	{
		sf::Texture t = *sprite.getTexture();
		sprite.setScale(sf::Vector2f(float(x) / float(t.getSize().x), float(y) / float(t.getSize().y)));
		sprite_on_hover.setScale(sf::Vector2f(float(x) / float(t.getSize().x), float(y) / float(t.getSize().y)));
		global_bounds = sprite.getGlobalBounds();
		//cout << "new res " << texture.getSize().x << ", " << texture.getSize().y << ", x = " << x << ", y = " << y << endl;
	}
}

void MyButton::customize()
{
	setColor(sf::Color::Black);
	setFontsize(resolution[1] / 20);
	setColorOnHover(sf::Color::Red);
	setBackgroundColor(sf::Color(255, 255, 255, 190));
	setOutlineThickness(3);
	horizontalCenter();
}

MyButton::~MyButton()
{
	if (type == "text")
		cout << "button destroyed, text = " << string(text.getString()) << endl;
	else
		cout << "image button destroyed" << endl;
}


						///////////////////
						//TextEntry Class//
						///////////////////


TextEntry::TextEntry(sf::String default_string) : text_str(default_string),
		lower_symbols(L"abcdefghijklmnopqrstuvwxyzé+¾šèžýáíé%%%%%%%%%úäô,.§-ò%=% %%%%%%%%%+-*/%%%%0123456789%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"),
		upper_symbols(L"ABCDEFGHIJKLMNOPQRSTUVQXYZÉ+¼ŠÈŽÝÁÍÉ%%%%%%%%%ÚÄÔ,.§-Ò%=% %%%%%%%%%+-*/%%%%0123456789%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
{
	pos_x = 0;
	pos_y = 0;
	internal_padding = 0;
	width = 0;

	on_hover_active = false;
	has_focus = false;
	cursor_visible = false;

	background_rect.setOutlineColor(sf::Color::Black);
	cursor_rect.setFillColor(sf::Color::White);
	cursor_rect.setSize(sf::Vector2f(font_size * 0.1, font_size));
	cursor_text_pos = default_string.getSize();
	updateCursorPos(cursor_text_pos);
	blinkCursor();
}

void TextEntry::draw(sf::RenderWindow& window)
{
	window.draw(background_rect);
	window.draw(text);
	if(cursor_visible)
		window.draw(cursor_rect);
}

float TextEntry::getTextWidth(sf::String inp_text)
{
	text.setString(inp_text);
	float result = text.getGlobalBounds().width;
	text.setString(text_str);
	return result;
}

void TextEntry::processEvent(sf::Vector2i mouse_position)
{
	if (global_bounds.contains(sf::Vector2f(mouse_position)))
	{
		if (!on_hover_active)
		{
			background_rect.setFillColor(colors[on_hover_col]);
			on_hover_active = true;
		}
	}
	else if (on_hover_active)
	{
		background_rect.setFillColor(colors[normal_col]);
		on_hover_active = false;
	}
}

void TextEntry::processEvent(int key_code, bool shift_pressed)
{
	if (key_code < lower_symbols.getSize())
	{
		//cout << key_code << endl;
		if (key_code == sf::Keyboard::BackSpace)
		{
			if (cursor_text_pos != 0)
			{
				text_str = text_str.substring(0, cursor_text_pos - 1) + text_str.substring(cursor_text_pos, text_str.getSize() - 1);
				cursor_text_pos--;
			}
		}
		else if (key_code == sf::Keyboard::Left)
		{
			if (cursor_text_pos != 0)
				cursor_text_pos--;
			cursor_visible = true;
		}
		else if (key_code == sf::Keyboard::Right)
		{
			if (cursor_text_pos != text_str.getSize())
				cursor_text_pos++;
			cursor_visible = true;
		}
		else
		{
			if (!(lower_symbols[key_code] == '%'))
			{
				sf::String temp;
				temp = text_str.substring(0, cursor_text_pos == 0 ? 0 : cursor_text_pos) +
					(shift_pressed ? upper_symbols : lower_symbols)[key_code] + text_str.substring(cursor_text_pos, text_str.getSize() - 1);
				if (getTextWidth(temp) < global_bounds.width - 2 * internal_padding)
				{
					text_str = temp;
					cursor_text_pos++;
				}
				else
				{
					alert();
				}
			}
		}

		updateCursorPos(cursor_text_pos);
		text.setString(text_str);
	}
}

void TextEntry::alert()
{
	background_rect.setOutlineColor(sf::Color::Red);
	game.time_manager.setTimeout([this]() { background_rect.setOutlineColor(colors[outline_col]); }, 250);
}

void TextEntry::updateCursorPos(uint8_t new_pos)
{
	cursor_text_pos = new_pos;
	cursor_rect.setPosition(pos_x + internal_padding + getTextWidth(text_str.substring(0, cursor_text_pos)) + 5, cursor_pos_y);
}

void TextEntry::blinkCursor()
{
	cursor_visible = cursor_visible ? false : true;
	game.time_manager.setTimeout(&TextEntry::blinkCursor, this, 500);
}

void TextEntry::setBackgroundColor(sf::Color new_color)
{
	background_rect.setFillColor(new_color);
	colors[normal_col] = new_color;
}

void TextEntry::setPosition(float new_pos_x, float new_pos_y)
{
	pos_x = new_pos_x;
	pos_y = new_pos_y;
	background_rect.setPosition(pos_x, pos_y);
	text.setPosition(pos_x + internal_padding, pos_y + internal_padding);
	global_bounds = background_rect.getGlobalBounds();
	cursor_pos_x = pos_x;
	cursor_pos_y = pos_y + internal_padding;
	updateCursorPos(cursor_text_pos);
}

void TextEntry::setWidth(float new_width)
{
	width = new_width;
	background_rect.setSize(sf::Vector2f(width, background_rect.getSize().y));
	global_bounds = background_rect.getGlobalBounds();
}

void TextEntry::setTextAndFont(string inp_text, sf::Font& new_font)
{
	text.setString(inp_text);
	text.setFont(new_font);
	updateCursorPos(inp_text.size());
}

void TextEntry::setFontsize(uint8_t new_size)
{
	font_size = new_size;
	text.setCharacterSize(new_size);
	internal_padding = 0.15 * new_size;
	internal_padding = 0.15 * new_size;
	background_rect.setSize(sf::Vector2f(background_rect.getSize().x, new_size + 4 * internal_padding));
	text.setPosition(pos_x + internal_padding, pos_y + internal_padding - text.getCharacterSize() * 0.33);
	global_bounds = background_rect.getGlobalBounds();
	cursor_rect.setSize(sf::Vector2f(font_size * 0.035, font_size + 2 * internal_padding));
	cursor_pos_x = pos_x + internal_padding;
	cursor_pos_y = pos_y + internal_padding;
	cursor_rect.setPosition(cursor_pos_x + internal_padding, cursor_pos_y);
}

void TextEntry::setOutlineColor(sf::Color new_color)
{
	background_rect.setOutlineThickness(4);
	background_rect.setOutlineColor(new_color);
	colors[outline_col] = new_color;
}

TextEntry::~TextEntry()
{
	cout << "deleting entry" << endl;
}