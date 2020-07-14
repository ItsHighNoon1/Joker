#pragma once

#include "sdk/discord.h"

namespace Joker {
	class DiscordManager {
		// Anything and everything that deals with Discord 
	public:
		DiscordManager(discord::ClientId appId);
		~DiscordManager();
		void tick();
		void activity(const char* state, const char* asset);
		void activity(const char* state, const char* details, const char* asset);
	private:
		discord::Core* core = nullptr; // Access point to Discord
	};
}