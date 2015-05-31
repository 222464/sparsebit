#pragma once

#include <memory>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace gui
{
	enum FloatStyle
	{
		TOP_LEFT,
		TOP_RIGHT,
		TOP_CENTER,
		BOTTOM_LEFT,
		BOTOM_RIGHT,
		BOTTOM_CENTER,
		CENTER
	};

	enum BorderStyle
	{
		NONE,
		SOLID
	};

	class Style
	{
	public:
		Style(const Style &parent)
		{
			_floatStyle = parent._floatStyle;

			_borderStyle = parent._borderStyle;

			_innerBorderLeft = parent._innerBorderLeft;
			_innerBorderRight = parent._innerBorderRight;
			_innerBorderTop = parent._innerBorderTop;
			_innerBorderBottom = parent._outerBorderBottom;

			_outerBorderLeft = parent._outerBorderLeft;
			_outerBorderRight = parent._outerBorderRight;
			_outerBorderTop = parent._outerBorderTop;
			_outerBorderBottom = parent._outerBorderBottom;
		}

		Style()
		{
			_floatStyle = FloatStyle::TOP_LEFT;

			_borderStyle = BorderStyle::NONE;

			_innerBorderLeft = 0.0f;
			_innerBorderRight = 0.0f;
			_innerBorderTop = 0.0f;
			_innerBorderBottom = 0.0f;

			_outerBorderLeft = 0.0f;
			_outerBorderRight = 0.0f;
			_outerBorderTop = 0.0f;
			_outerBorderBottom = 0.0f;
		}

		void operator=(const Style& parent)
		{
			_floatStyle = parent._floatStyle;

			_borderStyle = parent._borderStyle;

			_innerBorderLeft = parent._innerBorderLeft;
			_innerBorderRight = parent._innerBorderRight;
			_innerBorderTop = parent._innerBorderTop;
			_innerBorderBottom = parent._outerBorderBottom;

			_outerBorderLeft = parent._outerBorderLeft;
			_outerBorderRight = parent._outerBorderRight;
			_outerBorderTop = parent._outerBorderTop;
			_outerBorderBottom = parent._outerBorderBottom;
		}

		int _floatStyle;
		int _borderStyle;

		float _innerBorderLeft;
		float _innerBorderRight;
		float _innerBorderTop;
		float _innerBorderBottom;

		float _outerBorderLeft;
		float _outerBorderRight;
		float _outerBorderTop;
		float _outerBorderBottom;
	};

	class Component
	{
	private:
		void calcAbsolutePosition();
		void applyBorders();

	public:
		void create(std::shared_ptr<Component> parent, Style style, bool visible = false, bool update = false, bool render = false);
		void create(std::shared_ptr<Component> parent, bool visible = false, bool update = false, bool render = false);
		void create(bool visible = false, bool update = false, bool render = false);

		bool _visible;
		bool _update;
		bool _render;

		sf::Vector2f _absolutePosition;
		sf::Vector2f _position;

		sf::Vector2f _size;

		std::shared_ptr<Component> _parent;
		std::vector<std::shared_ptr<Component>> _children;

		Style _style;

		void setPosition(sf::Vector2f position, bool percentage = false);

		void sendMessageUp(const std::string &message);
		void sendMessageDown(const std::string &message);

		virtual void onAdd() {}

		virtual void update(float timeStep) {}

		virtual void render(sf::RenderWindow &window) {}

		virtual void onEvent(sf::Event &event) {}

		virtual bool onMessage(const std::string &message) {}
	};
}