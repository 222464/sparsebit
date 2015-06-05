#include "Component.h"

void gui::Component::create(float x, float y, float width, float height, bool update = true, bool render = true)
{
	_x = x;
	_y = y;
	_width = width;
	_height = height;
	_update = update;
	_render = render;
	_ownStyle = false;
}

void gui::Component::create(float x, float y, float width, float height, Style style, bool update = true, bool render = true)
{
	_x = x;
	_y = y;
	_width = width;
	_height = height;
	_style = style;
	_update = update;
	_render = render;
	_ownStyle = true;
}

void gui::Component::create(std::shared_ptr<Component> parent, bool update = true, bool render = true)
{
	_parent = parent;
	_update = update;
	_render = render;
	_ownStyle = false;
}

void gui::Component::create(std::shared_ptr<Component> parent, Style style, bool update = true, bool render = true)
{
	_parent = parent;
	_style = style;
	_update = update;
	_render = render;
	_ownStyle = true;
}

void gui::Component::sendMessageUp(const std::string &message)
{
	std::shared_ptr<Component> _current = _parent;

	while (_current != nullptr)
	{
		if (!_current->onMessage(message))
			return;
		_current = _current->_parent;
	}
}

void gui::Component::sendMessageDown(const std::string &message)
{
	for (int i = 0; i < _childrenGrid._components.size(); i++)
	{
		if (_childrenGrid._components[i]->onMessage(message))
			_childrenGrid._components[i]->sendMessageDown(message);
	}
}