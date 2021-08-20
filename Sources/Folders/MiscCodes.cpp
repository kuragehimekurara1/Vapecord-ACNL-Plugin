#include <CTRPluginFramework.hpp>
#include "cheats.hpp"
#include "RegionCodes.hpp"
#include "TextFileParser.hpp"

extern "C" void MoveFurn(void);
extern "C" void PATCH_MoveFurnButton(void);
extern "C" void PATCH_ToolAnim(void);

u8 toolTypeAnimID = 6;

extern "C" bool __IsAnimID(u8 toolAnimID) {
	static const u8 toolAnimIDArr[18] = { 0xB0, 0x49, 0x55, 0x6C, 0xA0, 0x98, 0x8F, 0x91, 0xC3, 0xCE, 0xCF, 0x8D, 0x8E, 0x91, 0xB1, 0xB1, 0x70, 0x9A };
	return std::find(std::begin(toolAnimIDArr), std::end(toolAnimIDArr), toolAnimID) != std::end(toolAnimIDArr);
}

namespace CTRPluginFramework {
	static bool AllOFF = false;
//disable commands
	void disablecommands(MenuEntry *entry) {
		std::vector<std::string> noncommands = {
			Language->Get("VECTOR_ANIM_COMM"),
			Language->Get("VECTOR_EMOT_COMM"),
			Language->Get("VECTOR_SNAK_COMM"),
			Language->Get("VECTOR_MUSI_COMM"),
			Language->Get("VECTOR_ITEM_COMM"),
			Language->Get("VECTOR_ALL_COMM")
		};

		for(int i = 0; i < 5; ++i) 
			noncommands[i] = (NonHacker::Accessible[i] ? Color(pGreen) : Color(pRed)) << noncommands[i];

		noncommands[5] = (AllOFF ? Color(pGreen) : Color(pRed)) << noncommands[5];

		Keyboard keyboard(Language->Get("COMM_CHOOSE"));
		keyboard.Populate(noncommands);
		Sleep(Milliseconds(100));
        s8 choice = keyboard.Open();
        if(choice < 0)
			return;

		if(choice == 5) {
			PluginMenu *menu = PluginMenu::GetRunningInstance();
			if(AllOFF) {
				*menu += NonHackerCallBack;
				AllOFF = false;
			}
			else {
				*menu -= NonHackerCallBack;
				AllOFF = true;
			}
			goto update;
		}

		NonHacker::Accessible[choice] = !NonHacker::Accessible[choice];

		update:
		disablecommands(entry);
	}

//Change Tool Animation
	void tooltype(MenuEntry *entry) {
		static Hook hook;
		if(Wrap::KB<u8>(Language->Get("TOOL_ANIM_ENTER_ANIM"), true, 2, toolTypeAnimID, toolTypeAnimID)) {
			if(toolTypeAnimID == 0) { //if switched OFF
				hook.Disable();
				return;
			}

			if(!IDList::AnimationValid(toolTypeAnimID))
				toolTypeAnimID = 6;

			hook.Initialize(Code::AnimFunction + 0x10, (u32)PATCH_ToolAnim);
			hook.SetFlags(USE_LR_TO_RETURN);
			hook.Enable();
		}
	}
//Change Gametype
	void mgtype(MenuEntry *entry) {
		std::vector<std::string> gametype = {
			Language->Get("VECTOR_GAMETYPE_OFFLINE"),
			Language->Get("VECTOR_GAMETYPE_ONLINE1"),
			Language->Get("VECTOR_GAMETYPE_ONLINE2"),
			Language->Get("VECTOR_GAMETYPE_DREAM"),
		};

		bool IsON;
		
		for(int i = 0; i < 4; ++i) { 
			IsON = GameHelper::GetGameType() == i;
			gametype[i] = (IsON ? Color(pGreen) : Color(pRed)) << gametype[i];
		}
		
        Keyboard keyboard(Language->Get("GAME_TYPE_CHOOSE"));
		keyboard.Populate(gametype);
		Sleep(Milliseconds(100));
        s8 gametchoice = keyboard.Open();
        if(gametchoice < 0)	
			return;
	
		GameHelper::ChangeGameType(gametchoice);
		mgtype(entry);
    }
//Unbreakable Flowers	
	void unbreakableflower(MenuEntry *entry) { 
		std::vector<std::string> cmnOpt =  { "" };
		
		bool IsON = *(u32 *)Code::UnbreakableFlower == 0xE3A0001D;

		cmnOpt[0] = IsON ? (Color(pGreen) << Language->Get("VECTOR_ENABLED")) : (Color(pRed) << Language->Get("VECTOR_DISABLED"));
		
		Keyboard optKb(Language->Get("KEY_CHOOSE_OPTION"));
		optKb.Populate(cmnOpt);
		Sleep(Milliseconds(100));
		s8 op = optKb.Open();
		
		if(op < 0)
			return;
			
		Process::Patch(Code::UnbreakableFlower, IsON ? Code::v_UnbreakableFlower : 0xE3A0001D);
		unbreakableflower(entry);
	}
//Weather Mod	
	void Weathermod(MenuEntry *entry) { 
		static const u32 weather = Region::AutoRegion(0x62FC30, 0x62F158, 0x62EC68, 0x62EC68, 0x62E728, 0x62E728, 0x62E2D0, 0x62E2D0);
		
		std::vector<std::string> weatheropt = {
			Language->Get("VECTOR_WEATHER_SUNNY"),
			Language->Get("VECTOR_WEATHER_CLOUDY"),
			Language->Get("VECTOR_WEATHER_RAINY"),
			Language->Get("VECTOR_WEATHER_STORMY"),
			Language->Get("VECTOR_WEATHER_SNOWY"),
			Language->Get("VECTOR_DISABLE")
		};
		
		static constexpr u32 Weathers[5] = {
			0xE3A00000, 0xE3A00002, 0xE3A00003, 0xE3A00004, 0xE3A00005
		};

		bool IsON;
		
		for(int i = 0; i < 5; ++i) { 
			IsON = *(u32 *)weather == Weathers[i];
			weatheropt[i] = (IsON ? Color(pGreen) : Color(pRed)) << weatheropt[i];
		}
		
		Keyboard optKb(Language->Get("KEY_CHOOSE_OPTION"));
		optKb.Populate(weatheropt);
		Sleep(Milliseconds(100));
		s8 op = optKb.Open();
		
		if(op < 0)
			return;
			
		if(op == 5) {
			Process::Patch(weather, 0xE1A00004);
			return;
		}
			
		Process::Patch(weather, Weathers[op]);
		Weathermod(entry);
	}
//Music Speed Modifier	
	void Musicchange(MenuEntry *entry) { 
		static const u32 musicch = Region::AutoRegion(0x4C4B4C, 0x4C44C4, 0x4C3B94, 0x4C3B94, 0x4C382C, 0x4C382C, 0x4C36EC, 0x4C36EC);
		
		std::vector<std::string> musicopt = {
			Language->Get("VECTOR_MUSIC_HYPER"),
			Language->Get("VECTOR_MUSIC_FAST"),
			Language->Get("VECTOR_MUSIC_SLOW"),
			Language->Get("VECTOR_MUSIC_SLOWER"),
			Language->Get("VECTOR_DISABLE")
		};
		
		static constexpr u32 Sounds[4] = {
			0x3F950000, 0x3FA0C000, 0x3F680000, 0x3F4C8000
		};

		bool IsON;
		
		for(int i = 0; i < 4; ++i) { 
			IsON = *(u32 *)musicch == Sounds[i];
			musicopt[i] = (IsON ? Color(pGreen) : Color(pRed)) << musicopt[i];
		}
		
		Keyboard musickb(Language->Get("KEY_CHANGE_SOUND"));
		musickb.Populate(musicopt);
		Sleep(Milliseconds(100));
		s8 op = musickb.Open();
		
		if(op < 0)
			return;
		
		if(op == 4) {
			Process::Patch(musicch, 0x3F800000);
			return;
		}
		
		Process::Patch(musicch, Sounds[op]);
		Musicchange(entry);
	}
	
//InputChangeEvent For Quick Menu
	void onBuildingChange(Keyboard &k, KeyboardEvent &e) {
		if(e.type == KeyboardEvent::CharacterRemoved || e.type == KeyboardEvent::CharacterAdded) {
			std::string s = k.GetInput();
			k.GetMessage() = "ID:\n\n" << IDList::GetBuildingName(s != "" ? std::stoi(s, nullptr, 16) : 0);
		}
	}

	void BuildingMod(MenuEntry *entry) {
		if(Player::GetSaveOffset(4) == 0) {
			Sleep(Milliseconds(100));
			MessageBox(Language->Get("SAVE_PLAYER_NO")).SetClear(ClearScreen::Top)();
			return;
		}

		static const std::vector<std::string> buildingOpt = {
			Language->Get("QUICK_MENU_PLACE_AT_LOCATION"),
			Language->Get("QUICK_MENU_MOVE_TO_LOCATION"),
			Language->Get("QUICK_MENU_REMOVE_BUILDING"),
		};

		Keyboard optKb(Language->Get("KEY_CHOOSE_OPTION"));	
		Sleep(Milliseconds(100));
		optKb.Populate(buildingOpt);
		switch(optKb.Open()) {
			case 0:
				u8 id;
				if(Wrap::KB<u8>(Language->Get("ENTER_ID"), 1, 2, id, 0, onBuildingChange)) {
					GameHelper::PlaceBuilding(id);
				}
				break;
			case 1:
				GameHelper::MoveBuilding();
				break;
			case 2:
				GameHelper::RemoveBuilding();
				break;
			default: break;
		}
	}

	void SearchReplace(MenuEntry *entry) {
		if(!PlayerClass::GetInstance()->IsLoaded()) {
			OSD::Notify("Your player needs to be loaded!", Color::Red);
			return;
		}

		u32 x = 0, y = 0;
		u32 count = 0;
		u32 ItemToSearch = 0;
		u32 ItemToReplace = 0;
		
		if(!Wrap::KB<u32>(Language->Get("QUICK_MENU_SEARCH_REPLACE_SEARCH"), true, 8, ItemToSearch, 0x7FFE)) 
			return;
		
		if(!Wrap::KB<u32>(Language->Get("QUICK_MENU_SEARCH_REPLACE_REPLACE"), true, 8, ItemToReplace, ItemToReplace)) 
			return;
		
		if(!IDList::ItemValid(ItemToReplace)) {
			OSD::Notify("Item Is Invalid!", Color::Red);
			return;
		}
			
		//int res = Dropper::Search_Replace(size, { 0x7C, 0x7D, 0x7E, 0x7F, 0xCC, 0xF8 }, Code::Pointer7FFE, 0x3D, false, "Weed Removed!", true);
			
		int res = Dropper::Search_Replace(300, { ItemToSearch }, ItemToReplace, 0x3D, true, "items replaced!", true);
		if(res == -1) {
			OSD::Notify("Your player needs to be loaded!", Color::Red);
			return;
		}
		else if(res == -2) {
			OSD::Notify("Only works outside!", Color::Red);
			return;
		}
	}

//Quick Menu
	void miscFunctions(MenuEntry *entry) {	
		static const std::vector<std::string> funcOpt = {
			Language->Get("VECTOR_QUICK_BUILDING"),
			Language->Get("VECTOR_QUICK_REMOVE"),
			Language->Get("VECTOR_COMMANDS_CLEAR_INV"),
			Language->Get("VECTOR_QUICK_S_A_R"),
			Language->Get("VECTOR_QUICK_RELOAD"),
			Language->Get("VECTOR_QUICK_LOCK_SPOT"),
			Language->Get("VECTOR_QUICK_UNLOCK_SPOT"),
			Language->Get("VECTOR_QUICK_LOCK_MAP"),
			Language->Get("VECTOR_QUICK_UNLOCK_MAP"),
		};
		
		if(entry->Hotkeys[0].IsDown()) {
			u32 x = 0, y = 0;
			
			Keyboard optKb(Language->Get("KEY_CHOOSE_OPTION"));
			optKb.Populate(funcOpt);
			switch(optKb.Open()) {	
				
			//Building Modifier
				case 0: {
					BuildingMod(entry);
				} break;

			//Remove Map Items
				case 1: {
					Sleep(Milliseconds(100));
					if((MessageBox(Language->Get("REMOVE_ITEM_WARNING"), DialogType::DialogYesNo)).SetClear(ClearScreen::Top)()) {
						GameHelper::RemoveItems(true, 0, 0, 0xFF, 0xFF, true, true);
					}
				} break;
				
			//Clear Inventory
				case 2: {
					if(Player::GetSaveOffset(4) == 0) {
						Sleep(Milliseconds(100));
						MessageBox(Language->Get("SAVE_PLAYER_NO")).SetClear(ClearScreen::Top)();
						return;
					}
					Sleep(Milliseconds(100));
					if((MessageBox(Language->Get("REMOVE_INV_WARNING"), DialogType::DialogYesNo)).SetClear(ClearScreen::Top)()) {
						for(int i = 0; i <= 0xF; ++i)
							Inventory::WriteSlot(i, 0x7FFE);
					}
				} break;

			//Search and replace
				case 3: {
					SearchReplace(entry);
				} break;
				
			//Reload Room
				case 4:
					GameHelper::ReloadRoom();	
				break;
			
			//Lock Spot
				case 5: {
					PlayerClass::GetInstance()->GetWorldCoords(&x, &y);
					
					if(bypassing) 
						GameHelper::DropItemLock(false);
					
					Sleep(Milliseconds(5));
					
					if(GameHelper::CreateLockedSpot(0x12, x, y, GameHelper::RoomCheck(), true) == 0xFFFFFFFF) 
						OSD::Notify("Error: Too many locked spots are already existing!");			
					else 
						OSD::Notify("Locked Spot");
					
					if(bypassing) 
						GameHelper::DropItemLock(true);
				} break;
			
			//Unlock Spot
				case 6: {
					if(bypassing) 
						GameHelper::DropItemLock(false);
					
					Sleep(Milliseconds(5));
					
					PlayerClass::GetInstance()->GetWorldCoords(&x, &y);
					GameHelper::ClearLockedSpot(x, y, GameHelper::RoomCheck(), 4);
					
					OSD::Notify("Unlocked Spot");
					
					if(bypassing) 
						GameHelper::DropItemLock(true);
				} break;
			
			//create lock spots on whole map
				case 7: {
					if(bypassing) 
						GameHelper::DropItemLock(false);

					u32 countY = 0;
					u32 countX = 0;
					
					Sleep(Milliseconds(5));
					while(GameHelper::CreateLockedSpot(0x12, 0x10 + countX, 0x10 + countY, GameHelper::RoomCheck(), true) != 0xFFFFFFFF) {
						countX++;
						if(countX % 6 == 2) { 
							countY++; 
							countX = 0; 
						}
						
						Sleep(Milliseconds(40));
					}
					
					OSD::Notify("Locked Map");
					
					if(bypassing) 
						GameHelper::DropItemLock(true);
				} break;
			
			//unlock lock spots on whole map
				case 8: {
					if(bypassing) 
						GameHelper::DropItemLock(false);

					Sleep(Milliseconds(5));
					x = 0x10;
					y = 0x10;
					bool res = true;
					while(res) {
						while(res) {
							if((u32)GameHelper::GetItemAtWorldCoords(x, y) != 0) {
								if(GameHelper::GetLockedSpotIndex(x, y, GameHelper::RoomCheck()) != 0xFFFFFFFF) {
									GameHelper::ClearLockedSpot(x, y, GameHelper::RoomCheck(), 4);
									Sleep(Milliseconds(40));
								}
							}
							else 
								res = false;

							y++;
						}
						res = true;
						y = 0x10;
						x++;
						if((u32)GameHelper::GetItemAtWorldCoords(x, y) == 0) 
							res = false;
					}
					
					OSD::Notify("Unlocked Map");
					
					Sleep(Milliseconds(5));
					if(bypassing) 
						GameHelper::DropItemLock(true);
				} break;
				
				default:
					break;
			}
		}	
	}
//More Than 3 Numbers On Island
	void morenumberisland(MenuEntry *entry) {
		static const u32 numbers = Region::AutoRegion(0xAD7158, 0xAD6158, 0xAD6158, 0xAD6158, 0xAD0158, 0xACF158, 0xACF158, 0xACF158);
		Process::Write8(numbers, 2);
		
		if(!entry->IsActivated()) 
			Process::Write8(numbers, 0);
	}
//Large FOV	
	void fovlarge(MenuEntry *entry) {
		static float OnOff = 1.0;
		
		Process::WriteFloat(Code::fov, OnOff); 
		
		if(GameHelper::RoomCheck() == 1 || fovbool) 
			OnOff = 1.0; 
		else 
			OnOff = 0.75; 
		
		if(!entry->IsActivated()) 
			Process::WriteFloat(Code::fov, 1.0);
	}
//Move Furniture
	void roomSeeder(MenuEntry *entry) {
		static const u32 movefurn = Region::AutoRegion(0x5B531C, 0x5B4834, 0x5B4364, 0x5B4364, 0x5B3C54, 0x5B3C54, 0x5B3928, 0x5B3928);
		static const u32 lightswitch = Region::AutoRegion(0x5B7558, 0x5B6A70, 0x5B65A0, 0x5B65A0, 0x5B5E90, 0x5B5E90, 0x5B5B64, 0x5B5B64);
		static const u32 MoveFurnPatch = Region::AutoRegion(0x326B98, 0x3265AC, 0x325EEC, 0x325EEC, 0x325B78, 0x325B78, 0x325A30, 0x325A30);

		static Hook hook1, hook2, hook3;

		if(entry->WasJustActivated()) {
			hook1.Initialize(movefurn, (u32)MoveFurn);
		  	hook1.SetFlags(USE_LR_TO_RETURN);
			hook1.Enable();	

			hook2.Initialize(lightswitch, (u32)MoveFurn);
		  	hook2.SetFlags(USE_LR_TO_RETURN);
			hook2.Enable();	

			hook3.Initialize(MoveFurnPatch, (u32)PATCH_MoveFurnButton);
			hook3.SetFlags(USE_LR_TO_RETURN);
			hook3.Enable();	
		}
		
		else if(!entry->IsActivated()) {
			hook1.Disable();
			hook2.Disable();
			hook3.Disable();
		}
    }
//Can Walk When Talk /*Made by Jay*/
	void walktalkentry(MenuEntry *entry) { 
		static const u32 walktalk = Region::AutoRegion(0x655390, 0x6548B8, 0x6543C8, 0x6543C8, 0x653E88, 0x653E88, 0x653A30, 0x653A30);
		if(entry->WasJustActivated()) 
			Process::Patch(walktalk, 0x1A000000); 
		else if(!entry->IsActivated()) 
			Process::Patch(walktalk, 0x0A000000); 
	}
//Keyboard Extender
	void key_limit(MenuEntry* entry) {
		if(!GameKeyboard::IsOpen())
			return;
		
		if(Inventory::GetCurrent() == 4) 
			return;
		
		u32 KeyData = *(u32 *)(GameHelper::BaseInvPointer() + 0xC) + 0x1328;
		static const u32 KeyEnter = Region::AutoRegion(0xAD7253, 0xAD6253, 0xAD6253, 0xAD6253, 0xAD0253, 0xACF253, 0xACF253, 0xACF253);
		static const u32 KeyAt = Region::AutoRegion(0xAD75C0, 0xAD65C0, 0xAD65C0, 0xAD65C0, 0xAD05C0, 0xACF5C0, 0xACF5C0, 0xACF5C0);
		
		Process::Write8(KeyData + 0xC, 0x41);
		Process::Write8(KeyData + 0x12B, 0x44);
		Process::Write8(KeyEnter, 1);
		Process::Write8(KeyAt, 1);
    }
//Custom Keyboard Symbols
	void CustomKeyboard(MenuEntry* entry) {   	
		static constexpr u16 patch[110] = {
			0xE000, 0xE001, 0xE002, 0xE003, 0xE004, 0xE005, 0xE006, 0xE008, 0xE009,
			0xE070, 0xE06F, 0xE06C, 0xE00C, 0xE00D, 0xE00E, 0xE00F, 0xE010, 0xE011,
			0xE012, 0xE013, 0xE014, 0xE03C, 0xE03B, 0xE03D, 0xE072, 0xE019, 0xE01A,
			0xE01B, 0xE01C, 0xE01D, 0xE01E, 0xE01F, 0xE020, 0xE021, 0xE022, 0xE023,
			0xE024, 0xE025, 0xE026, 0xE027, 0xE028, 0xE029, 0xE02A, 0xE02B, 0xE02C,
			0xE02D, 0xE02E, 0xE02F, 0xE030, 0xE031, 0xE032, 0xE033, 0xE034, 0xE035,
			0xE036, 0xE037, 0xE038, 0xE039, 0xE079, 0xE07A, 0xE07B, 0xE07C, 0xE03E,
			0xE03F, 0xE040, 0xE041, 0xE042, 0xE043, 0xE044, 0xE045, 0xE046, 0xE047,
			0xE048, 0xE049, 0xE04A, 0xE04B, 0xE04C, 0xE04D, 0xE04E, 0xE04F, 0xE050,
			0xE051, 0xE052, 0xE053, 0xE054, 0xE055, 0xE056, 0xE057, 0xE058, 0xE059,
			0xE05A, 0xE05B, 0xE05C, 0xE05D, 0xE05E, 0xE05F, 0xE060, 0xE061, 0xE062,
			0xE063, 0xE064, 0xE065, 0xE069, 0xE06A, 0xE073, 0xE067, 0xE074, 0xE075,
			0xE076, 0xE077
        };	
	
		static const u32 IsOpen = Region::AutoRegion(0xAD7050, 0xAD6050, 0xAD6050, 0xAD6050, 0xAD0050, 0xACF050, 0xACF050, 0xACF050);		
        if(*(u16 *)IsOpen == 0x0103) {
			u32 offset = 0;
			static const u32 customKey = Region::AutoRegion(0xAD7630, 0xAD6630, 0xAD6630, 0xAD6630, 0xAD0630, 0xACF630, 0xACF630, 0xACF630);	
            offset = *(u32 *)customKey;
            if(offset != 0) {	
                Process::Read32(offset + 0x224, offset);

				u16 value = 0;
			//US/EU French, US/EU English, US/EU Spanish, EU Italian, EU German
				if(Process::Read16(offset + 0x26, value) && value == 0x2E) 
					Process::CopyMemory((void *)(offset + 0x26), (void *)patch, 0x6E * 2); 
			//Japanese Keyboard
				else if(Process::Read16(offset + 0x1DC, value) && value == 0x3001) 
					Process::CopyMemory((void *)(offset + 0x1DC), (void *)patch, 0x6E * 2); 
			}
		}
    }
			
//Beans Particle Changer	
	void Beans(MenuEntry *entry) {
		static const u32 beans = Region::AutoRegion(0x673E0C, 0x673334, 0x672E44, 0x672E44, 0x672904, 0x672904, 0x6724AC, 0x6724AC);
        static u16 input = 0; 
		
        if(entry->Hotkeys[0].IsDown()) {
			if(Wrap::KB<u16>(Language->Get("BEANS_PARTICLE_ENTER_ID"), true, 3, input, 0)) 
				Process::Patch(beans, input);
		}
		
		if(!entry->IsActivated()) 
			Process::Patch(beans, 0x205);
	}

//Always Daytime /*Made by Jay*/
	void Daytime(MenuEntry *entry) {
		static const u32 day1 = Region::AutoRegion(0x4B10A4, 0x4B0A1C, 0x4B00EC, 0x4B00EC, 0x4AFD84, 0x4AFD84, 0x4AFC44, 0x4AFC44);
		static const u32 day2 = Region::AutoRegion(0x1E6D58, 0x1E679C, 0x1E6D78, 0x1E6D78, 0x1E6CB4, 0x1E6CB4, 0x1E6C80, 0x1E6C80);
		static const u32 day3 = Region::AutoRegion(0x4B10AC, 0x4B0A24, 0x4B00F4, 0x4B00F4, 0x4AFD8C, 0x4AFD8C, 0x4AFC4C, 0x4AFC4C);
		static const u32 day4 = Region::AutoRegion(0x4B10C8, 0x4B0A40, 0x4B0110, 0x4B0110, 0x4AFDA8, 0x4AFDA8, 0x4AFC68, 0x4AFC68);
		
		static const u32 DayTime[4] = { day1, day2, day3, day4 };
		
		static constexpr u32 DayTimePatch[2][4] = {
            { 0xE3A01788, 0x3FA00000, 0xE3A00000, 0xE8871015 },
			{ 0xE1A01521, 0x3F800000, 0xE3A06000, 0xE8871004 }
		};
		
		if(entry->WasJustActivated()) {
			for(int i = 0; i < 4; ++i)
                Process::Patch(DayTime[i], DayTimePatch[0][i]);
		}
		else if(!entry->IsActivated()) {
			for(int i = 0; i < 4; ++i)
                Process::Patch(DayTime[i], DayTimePatch[1][i]);
		}
	}
//Fast Mode	
	void fast(MenuEntry *entry) {
		if(entry->Hotkeys[0].IsPressed()) { //Key::R + Key::DPadDown
			turbo = !turbo;
			OSD::Notify("Fast mode " << (turbo ? Color::Green << "ON" : Color::Red << "OFF"));
		}
	}
//Fast Text Speed
	void fasttalk(MenuEntry *entry) { 
		static const u32 fastt = Region::AutoRegion(0x5FC6B0, 0x5FBBE0, 0x5FB6E8, 0x5FB6E8, 0x5FAF68, 0x5FAF68, 0x5FABF0, 0x5FABF0);
		if(entry->WasJustActivated()) 
			Process::Patch(fastt, 0xE3A00001);
		else if(!entry->IsActivated()) 
			Process::Patch(fastt, 0xEBFFF4B2);
	}
//Fast Game Speed	
	void speedentry(MenuEntry *entry) {
		static const u32 speed = Region::AutoRegion(0x54DDB4, 0x54D2CC, 0x54CDFC, 0x54CDFC, 0x54C6E8, 0x54C6E8, 0x54C40C, 0x54C40C);
		Process::Patch(speed, GameHelper::GameSaving() ? 0xE59400A0 : 0xE3E004FF); 
		
		if(!entry->IsActivated()) 
			Process::Patch(speed, 0xE59400A0); 
	}
}
