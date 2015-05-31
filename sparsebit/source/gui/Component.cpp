#include "Component.h"

void gui::Component::create(std::shared_ptr<Component> parent, Style style, bool visible = false, bool update = false, bool render = false)
{
	_parent = parent;
	_style = style;
	_visible = visible;
	_update = update;
	_render = render;

	_absolutePosition = parent->_absolutePosition;
}

void gui::Component::create(std::shared_ptr<Component> parent, bool visible = false, bool update = false, bool render = false)
{
	_parent = parent;
	_visible = visible;
	_update = update;
	_render = render;

	_absolutePosition = parent->_absolutePosition;
}

void gui::Component::create(bool visible = false, bool update = false, bool render = false)
{
	_visible = visible;
	_update = update;
	_render = render;

	_absolutePosition = sf::Vector2f(0.0f, 0.0f);
}

void gui::Component::calcAbsolutePosition()
{
	std::shared_ptr<Component> _current = _parent;

	if (_parent == nullptr)
		_absolutePosition = _position;
	else
	{
		while (_current->_parent != nullptr)
			_current = _current->_parent;

		_absolutePosition = _current->_position;
	}
}

void gui::Component::applyBorders()
{
	if (_parent != nullptr)
	{
		_position.x += _parent->_style._innerBorderLeft;
		_size.x -= _parent->_style._innerBorderRight;
		_position.y += _parent->_style._innerBorderTop;
		_size.y -= _parent->_style._innerBorderBottom;
	}

	_position.x += _style._outerBorderLeft;
	_size.x -= _style._outerBorderRight;
	_position.y += _style._outerBorderTop;
	_size.y -= _style._outerBorderBottom;
}

void gui::Component::setPosition(sf::Vector2f position, bool percentage = false)
{
	if (percentage)
	{
		_position.x = position.x / 100.0f * _size.x;
		_position.y = position.y / 100.0f * _size.y;
		calcAbsolutePosition();
		applyBorders();
		return;
	}

	_position = position;
	calcAbsolutePosition();
	applyBorders();
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
	for (int i = 0; i < _children.size(); i++)
	{
		_children
	}
}