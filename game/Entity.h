#pragma once

#include <memory>
#include <vector>

#include "component/Component.h"

namespace Joker {
	class Entity {
		// A container for components
	public:
		void tick();
		void addComponent(std::unique_ptr<Component> component);
	private:
		std::vector<std::unique_ptr<Component>> components;
	};
}