#include "cheats.hpp"
#include "Files.h"
#include "MenuPointers.hpp"
#include "Helpers/Game.hpp"
#include "Color.h"

namespace CTRPluginFramework {
	bool turbo = false;
	bool fovbool = false;
	bool save = false;

	const std::string g_Empty = "-Empty-";

	const Color pColor[4] = {
		Color(pBlue), Color(pRed),		
		Color(pGreen), Color(pYellow),	
	};

/*
Returns a randomized RGBA color
*/
	u32 Rainbow() {
		u8 ShiftR = Utils::Random(0, 255);
        u8 ShiftG = Utils::Random(0, 255);
        u8 ShiftB = Utils::Random(0, 255);	
		u8 ShiftA = Utils::Random(0, 255);
			
		return Set::ToRGBA_U32(ShiftR, ShiftG, ShiftB, ShiftA);
    }

	bool RainBowON = false;

/*
Randomizes colors of Menu Folders
*/
	void RainbowEntrys(Time time) {
		static Clock timer(time);
		if(timer.HasTimePassed(Milliseconds(500)) && RainBowON) {   
			SaveCodesUpdate(Rainbow());
			MovementCodesUpdate(Rainbow());
			InventoryCodesUpdate(Rainbow());
			PlayerCodesUpdate(Rainbow());
			AnimationCodesUpdate(Rainbow());
			SeedingCodesUpdate(Rainbow());
			MoneyCodesUpdate(Rainbow());
			IslandCodesUpdate(Rainbow());
			NPCCodesUpdate(Rainbow());
			FunCodesUpdate(Rainbow());
			ExtraCodesUpdate(Rainbow());
			MiscCodesUpdate(Rainbow());
			timer.Restart();
		}
	}

	bool FCLoaded = false;

	void StoreFC(std::string &FC) {
		frdInit();

		FriendKey key;
		FRD_GetMyFriendKey(&key);

		u64 localFriendCode = 0;
		FRD_PrincipalIdToFriendCode(key.principalId, &localFriendCode);

		char buffer[13];
		sprintf(buffer, "%012lld", localFriendCode);
		std::string str = (std::string(buffer));
		FC = Utils::Format("FC: %s - %s - %s", str.substr(0, 4).c_str(), str.substr(4, 4).c_str(), str.substr(8, 4).c_str());

		frdExit();
	}

	void StoreBatteryPercentage(float &percentage) {
		u8 data[4];
		mcuHwcInit();
		MCUHWC_ReadRegister(0xA, data, 4);

		percentage = data[1] + data[2] / 256.0f;
        percentage = (u32)((percentage + 0.05f) * 10.0f) / 10.0f;

		mcuHwcExit();
	}

	void OnNewFrameCallback(Time ttime) {
		RainbowEntrys(ttime);

		static std::string FC = "";

		if(!FCLoaded) {
			StoreFC(FC);
			FCLoaded = true;
		}

		const FwkSettings &settings = FwkSettings::Get();
		const Screen& TopScreen = OSD::GetTopScreen();
		const Screen& BottomScreen = OSD::GetBottomScreen();
		time_t rawtime;
		struct tm * timeinfo;
		char timestamp[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timestamp, 80, "%r | %F", timeinfo);

		float percentage = 0;
		StoreBatteryPercentage(percentage);

		int coordx = 30, coordy1 = 0, coordy2 = 218;

 		TopScreen.DrawRect(coordx, coordy1, 340, 20, settings.BackgroundMainColor, true);
		TopScreen.DrawRect(coordx + 2, coordy1 + 2, 340 - 4, 20 - 2, settings.BackgroundBorderColor, false);

		TopScreen.DrawRect(coordx, coordy2, 340, 22, settings.BackgroundMainColor, true);
		TopScreen.DrawRect(coordx + 2, coordy2 + 2, 340 - 4, 20 - 2, settings.BackgroundBorderColor, false);

		TopScreen.DrawSysfont(timestamp, coordx + 5, coordy1 + 3);
		TopScreen.DrawSysfont(Utils::Format("%d%%", (u32)percentage), coordx + 280, coordy1 + 3);

		TopScreen.DrawSysfont(FC, coordx + 5, coordy2 + 3);
		//TopScreen.DrawSysfont("BETA 2", coordx + 280, coordy2 + 3);

		coordx = 20, coordy1 = 0, coordy2 = 218;

		BottomScreen.DrawRect(coordx, coordy1, 280, 20, settings.BackgroundMainColor, true);
		BottomScreen.DrawRect(coordx + 2, coordy1 + 2, 280 - 4, 20 - 2, settings.BackgroundBorderColor, false);
	
		BottomScreen.DrawRect(coordx, coordy2, 280, 22, settings.BackgroundMainColor, true);
		BottomScreen.DrawRect(coordx + 2, coordy2 + 2, 280 - 4, 20 - 2, settings.BackgroundBorderColor, false);
		
		BottomScreen.DrawSysfont("Discord: " + std::string(DISCORDINV), coordx + 5, coordy1 + 3);

		BottomScreen.DrawSysfont("Plugin made by Lukas#4444", coordx + 5, coordy2 + 3);
	}

/*
Toggles the random color entry
*/
	void rainbow(MenuEntry *entry) {
		if(entry->WasJustActivated())
			RainBowON = true;
		else if(!entry->IsActivated())
			RainBowON = false;
	}
}