#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Component.h"

namespace gui
{
	class ComponentGrid
	{
	private:
		std::vector<float> _vertical;
		std::vector<float> _horizontal;

	public:
		ComponentGrid(std::shared_ptr<Component> parent) : _parent(parent) {}

		std::shared_ptr<Component> _parent;
		std::vector<std::shared_ptr<Component>> _components;

		void addVerticalGridLine(float offsetX);
		void addHorizontalGridLine(float offsetY);

		void removeVerticalGridLine(int index);
		void removeHorizontalGridLine(int index);

		void emplaceComponent(std::shared_ptr<Component> component, int gridCol, int gridRow);
		void removeComponent(int gridCol, int gridRow);

		std::shared_ptr<Component> getComponentAt(int gridCol, int gridRow);

		int getNumVerticalGridLines() const;
		int getNumHorizontalGridLines() const;
	};
}