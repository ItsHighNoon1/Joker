#include "Entity.h"

#include "Joker.h"

namespace Joker {
	void Entity::tick() {
		// Invoke all member components
		for (uint32_t i = 0; i < components.size(); i++) {
			components[i]->invoke();
		}
	}

	void Entity::addComponent(std::unique_ptr<Component> component) {
		components.push_back(std::move(component));
	}
}