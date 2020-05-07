#pragma once

#include "Component.h"

namespace Joker {
	class TestComponent : public Component {
		// Print to console on tick
	public:
		virtual void invoke();
	};
}