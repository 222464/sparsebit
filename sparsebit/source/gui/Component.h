#pragma once

#include <memory>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Style.h"
#include "ComponentGrid.h"

namespace gui
{
	class Component
	{
	private:
		friend class ComponentGrid;

		float _x;
		float _y;
		float _width;
		float _height;

		int _gridRow;
		int _gridCol;

		bool _ownStyle;

		std::shared_ptr<Component> _parent;
		ComponentGrid _childrenGrid;

	public:
		bool _update;
		bool _render;

		Style _style;

		void create(float x, float y, float width, float height, bool update = true, bool render = true);
		void create(float x, float y, float width, float height, Style style, bool update = true, bool render = true);
		void create(std::shared_ptr<Component> parent, bool update = true, bool render = true);
		void create(std::shared_ptr<Component> parent, Style style, bool update = true, bool render = true);

		void sendMessageUp(const std::string &message);
		void sendMessageDown(const std::string &message);

		virtual void onAdd() {}

		virtual void update(float timeStep) {}

		virtual void render(sf::RenderWindow &window) {}

		virtual void onEvent(sf::Event &event) {}

		virtual bool onMessage(const std::string &message) {}
	};
}