#include "DiscordManager.h"

#include <iostream>

namespace Joker {
	DiscordManager::DiscordManager(discord::ClientId appId) {
		auto result = discord::Core::Create(appId, DiscordCreateFlags_NoRequireDiscord, &core);
	}

	DiscordManager::~DiscordManager() {
		delete core; // Destroy reference to Discord
	}

	void DiscordManager::tick() {
		core->RunCallbacks(); // Discord won't run if we don't call this every frame
	}

	void DiscordManager::activity(const char* state, const char* asset) {
		discord::Activity activity{};
		activity.SetState(state);
		activity.GetAssets().SetLargeImage(asset);
		activity.GetParty().GetSize().SetCurrentSize(1);
		activity.GetParty().GetSize().SetMaxSize(10);
		activity.GetParty().SetId("a2131");
		activity.GetSecrets().SetJoin("aadsdadsad");
		activity.SetInstance(true);
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	}

	void DiscordManager::activity(const char* state, const char* details, const char* asset) {
		discord::Activity activity{};
		activity.SetState(state);
		activity.SetDetails(details);
		activity.GetAssets().SetLargeImage(asset);
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	}
}
