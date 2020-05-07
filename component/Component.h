#pragma once

namespace Joker {
	class Component {
		// Base class for other components
	public:
		virtual void invoke() = 0;
	};
}