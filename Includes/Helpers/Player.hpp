#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <CTRPluginFramework.hpp>
#include "Helpers/GameStructs.hpp"

extern bool IsIndoorsBool;

namespace CTRPluginFramework {
	namespace Player {
		u32 			GetBulletin(int slot);
		void 			Load(int pIndex);
		void			ReloadDesign(int slot);
		u32				GetDesign(int slot, int pIndex = 4);
		void			StealDesign(u8 slot);
		void			UpdateStyle();
		void			UpdateTan();
		PlayerStatus 	GetPlayerStatus(u8 pPlayer);
		u32				GetSpecificSave(u8 pPlayer);
		u32				GetSaveOffset(u8 pIndex);
		void			WriteOutfit(u8 PlayerIndex, Item HeadWear, Item Glasses, Item Shirt, Item Pants, Item Socks, Item Shoes);
		void 			ClearMail(int slot);
		bool			Exists(u8 PlayerIndex);		
		float*		    GetCollisionSize(u8 PlayerIndex = 4);
		bool			IsIndoors();
		u8				GetRoom(u8 PlayerIndex);

		ACNL_Player		*GetData(u8 PlayerIndex = 4);

		bool 			SetUnlockableBitField(ACNL_Player *player, u8 ID, bool state);
	};

	namespace PlayerName {
		void 				UpdateReference(u8 pIndex, const std::string& pName, u8 pGender);
		void				Set(const std::string& name);
		void 				SetDesigns(const std::string& name);
		void				SetLetters(const std::string& name);
		void				SetVillagers(const std::string& name);
	}
}
#endif
