#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "FunctionHolder.h"

#ifndef WIDGETS_H_
#define WIDGETS_H_

using namespace std;
const uint8_t mouse_int_flags_number = 2;

	
		//////////////////
		//MyButton Class//
		//////////////////


class MyButton {
public:
	MyButton(string inp_game_state, string button_type, sf::SoundBuffer& hover_sound, sf::SoundBuffer& click_sound);
	~MyButton();
	template<typename F> void setFunction(F function);
	template<typename F, typename A> void setFunction(F function, A argument);
	template<typename C> void setFunction(void(C::*function)(), C* object);
	template<typename C, typename A> void setFunction(void(C::*function)(A), C* object, A argument);
	void setImage(sf::Texture& new_texture);
	void setText(string inp_text);
	void draw(sf::RenderWindow &window);
	void setFontsize(float new_size);
	void setColor(sf::Color inp_color);
	void setPosition(float inp_pos_x, float inp_pos_y);
	void receiveNewGameState(string new_game_part);
	void setColorOnHover(sf::Color inp_color);
	void setImageOnHover(sf::Texture& new_hover_texture);
	void setActivityState(bool state);
	void setBackgroundColor(sf::Color new_bg_color);
	void horizontalCenter();
	void setImageResolution(int x, int y);
	void customize();
	bool processMouseEvent(string event_type, sf::Vector2i mouse_position);
	bool hasActiveBind(int flag);
		////inlines////
	string get_type() { return type; }
	int getWidth() { return global_bounds.width; };
	bool isActive() { return isactive_bool; }
	string getText() { return string(text.getString()); }
	bool contains(sf::Vector2f vect) { return global_bounds.contains(vect) ? true : false; }
	void setVisibility(bool new_vis_bool) { is_visible = new_vis_bool ? true : false; }
	void setOutlineThickness(uint8_t new_thickness) { background_rect.setOutlineThickness(new_thickness); };
	void setOutlineColor(sf::Color new_color) { background_rect.setOutlineColor(new_color); };
	void setFont(sf::Font& new_font) { if (type == "text") text.setFont(new_font); }
private:
	BaseFunction* function_holder;
	enum button_flags_names { mouse_button_press, mouse_over };
	enum colors_enum { normal_col, on_hover_col, rect_background_col };
	map <string, button_flags_names> flags_names_map;
	bool active_mouse_interaction_flags[mouse_int_flags_number] = { 0, 0 }; // { click, hover }
	bool applied_mouse_interaction_flags[mouse_int_flags_number] = { 0, 0 };
	bool isactive_bool;
	bool is_centered;
	bool is_visible;
	bool has_function;
	string type, game_state;
	sf::Sound sounds[2];
	sf::Sprite sprite;
	sf::Sprite sprite_on_hover;
	sf::Font font;
	sf::Text text;
	sf::Color colors[3] = { sf::Color::White, sf::Color::White, sf::Color::White }; // {normal, on hover, rect background}
	sf::RectangleShape background_rect;
	sf::FloatRect global_bounds;
	float pos_x, pos_y;
	float font_size;
	uint16_t resolution[2];
	int internal_padding;
};

template<typename F> void MyButton::setFunction(F function)
{
	if (has_function)
		delete function_holder;
	else
		has_function = true;

	function_holder = new Functor<F>(function);
}

template<typename F, typename A> void MyButton::setFunction(F function, A argument)
{
	if (has_function)
		delete function_holder;
	else
		has_function = true;

	function_holder = new FunctionWithArg<F, A>(function);
}

template<typename C> void MyButton::setFunction(void(C::*function)(), C* object)
{
	if (has_function)
		delete function_holder;
	else
		has_function = true;

	function_holder = new ClassFunction<C>(function, object);
}

template<typename C, typename A> void MyButton::setFunction(void(C::*function)(A), C* object, A argument)
{
	if (has_function)
		delete function_holder;
	else
		has_function = true;

	function_holder = new ClassFunctionWithArg<C, A>(function, object, argument);
}

						///////////////////
						//TextEntry Class//
						///////////////////


class TextEntry {
public:
	TextEntry(sf::String default_string = "");
	~TextEntry();
	void setPosition(float pos_x, float pos_y);
	void setWidth(float new_width);
	void setTextAndFont(string inp_text, sf::Font& new_font);
	void setFontsize(uint8_t new_size);
	void processEvent(sf::Vector2i mouse_position);
	void processEvent(int key_code, bool shift_pressed);
	void setOutlineColor(sf::Color new_color);
	void setBackgroundColor(sf::Color new_color);
	void draw(sf::RenderWindow& window);
	void blinkCursor();
	void alert();
		////inlines////
	float getWidth() { return width; };
	void setBackgroundColorOnHover(sf::Color new_color) { colors[on_hover_col] = new_color; }
	void setOutlineThickness(uint8_t new_thickness) { background_rect.setOutlineThickness(new_thickness); };
	void setTextColor(sf::Color new_color) { text.setFillColor(new_color); };
	sf::String getText() { return text_str; };
private:
	float getTextWidth(sf::String inp_text);
	void updateCursorPos(uint8_t new_pos);
	enum colors_enum { normal_col, on_hover_col, rect_background_col, outline_col };
	sf::RectangleShape background_rect;
	sf::RectangleShape cursor_rect;
	sf::Text text;
	sf::FloatRect global_bounds;
	sf::Color colors[4] = { sf::Color::White, sf::Color::White, sf::Color::White, sf::Color::White }; // {normal, on hover, rect background, outline}
	const sf::String lower_symbols;
	const sf::String upper_symbols;
	sf::String text_str;
	float cursor_pos_x = 0, cursor_pos_y = 0;
	uint8_t cursor_text_pos = 0;
	float width;
	float pos_x, pos_y;
	float internal_padding;
	float font_size = 20;
	bool on_hover_active;
	bool has_focus;
	bool cursor_visible;
};

#endif