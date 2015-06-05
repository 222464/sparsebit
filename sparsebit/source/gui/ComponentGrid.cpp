#include "ComponentGrid.h"

void gui::ComponentGrid::addVerticalGridLine(float offsetX)
{
	_vertical.push_back(offsetX);
}

void gui::ComponentGrid::addHorizontalGridLine(float offsetY)
{
	_horizontal.push_back(offsetY);
}

void gui::ComponentGrid::removeVerticalGridLine(int index)
{
	_vertical.erase(_vertical.begin() + index, _vertical.begin() + index + 1);
}

void gui::ComponentGrid::removeHorizontalGridLine(int index)
{
	_horizontal.erase(_vertical.begin() + index, _vertical.begin() + index + 1);
}

void gui::ComponentGrid::emplaceComponent(std::shared_ptr<Component> component, int gridCol, int gridRow)
{
	if (getComponentAt(gridCol, gridRow) == nullptr)
	{
		component->_gridCol = gridCol;
		component->_gridRow = gridRow;

		if (!component->_ownStyle)
			component->_style = _parent->_style;

		component->_x = _vertical[gridCol] + _parent->_style._innerBorderLeft + component->_style._outerBorderLeft;
		component->_y = _horizontal[gridRow] + _parent->_style._innerBorderTop + component->_style._outerBorderTop;

		if (gridCol < _vertical.size() - 1)
			component->_width = _vertical[gridCol + 1] - component->_x - _parent->_style._innerBorderLeft - component->_style._outerBorderLeft;
		else
			component->_width = _parent->_x + _parent->_width - component->_x - _parent->_style._innerBorderLeft - component->_style._outerBorderLeft;

		if (gridRow < _horizontal.size() - 1)
			component->_height = _vertical[gridCol + 1] - component->_y - _parent->_style._innerBorderTop - component->_style._outerBorderTop;
		else
			component->_height = _parent->_y + _parent->_height - component->_y - _parent->_style._innerBorderTop - component->_style._outerBorderTop;

		_components.push_back(component);
	}
}

void gui::ComponentGrid::removeComponent(int gridCol, int gridRow)
{
	for (int i = 0; i < _components.size(); i++)
		if (_components[i]->_gridCol == gridCol && _components[i]->_gridRow == gridRow)
		{
			_components.erase(_components.begin() + i, _components.begin() + i + 1);
			return;
		}
}

std::shared_ptr<gui::Component> gui::ComponentGrid::getComponentAt(int gridCol, int gridRow)
{
	for (int i = 0; i < _components.size(); i++)
		if (_components[i]->_gridCol == gridCol && _components[i]->_gridRow == gridRow)
			return _components[i];

	return std::shared_ptr<Component>(nullptr);
}

int gui::ComponentGrid::getNumVerticalGridLines() const
{
	return _vertical.size();
}

int gui::ComponentGrid::getNumHorizontalGridLines() const
{
	return _horizontal.size();
}