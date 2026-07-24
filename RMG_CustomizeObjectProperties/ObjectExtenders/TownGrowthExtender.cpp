#include "../pch.h"
#include "TownGrowthExtender.h"
#include "H3MapItemNew.h"

namespace townGrowth
{
    TownGrowthExtender::TownGrowthExtender()
        : H3ActiveObject<H3MapItemTownGrowth>(
            "EraPlugin.TownGrowthExtender",
            extender::HOTA_OBJECT_TYPE,
            TOWN_GROWTH_OBJECT_SUBTYPE)
    {
    }

    TownGrowthExtender* TownGrowthExtender::instance = nullptr;
    TownGrowthExtender& TownGrowthExtender::Get()
    {
        if (!instance) instance = new TownGrowthExtender();
        return *instance;
    }

    BOOL TownGrowthExtender::VisitMapItem(
        H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
        const BOOL isHuman) const noexcept
    {
        if (auto townGrowth = GetFromMapItem(mapItem))
        {
            const int playerId = hero->owner;
            sprintf(newMapItem::buffer, H3MapItemTownGrowth::PlayerVisitedVariableFormat, playerId);
            
            if (Era::GetAssocVarIntValue(newMapItem::buffer))
            {
                if (isHuman)
                {
                    // Сообщение о том, что бонус уже получен
                    FASTCALL_12(void, 0x4F6C00,
                        this->GetVisitedMessage().String(),
                        1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -777);
                }
                return true;
            }

            // Начисление бонуса
            H3Player* player = &P_Game->players[playerId];
            for (int i = 0; i < player->townsCount; ++i)
            {
                int townId = player->towns[i];
                H3Town* town = &P_Game->towns[townId];
                
                for (int slot = 0; slot < 7; ++slot)
                {
                    // 0x5BF1D0 - H3Town::GetGrowth(int slot)
                    int growth = THISCALL_2(int, 0x5BF1D0, town, slot);
                    if (growth > 0)
                    {
                        // 37 - DWELL1U
                        int upgraded = town->IsBuildingBuilt(37 + slot) ? 1 : 0;
                        town->recruits[upgraded][slot] += (INT16)growth;
                    }
                }
            }

            // Помечаем, что игрок получил бонус
            Era::SetAssocVarIntValue(newMapItem::buffer, 1);

            if (isHuman)
            {
                // Сообщение о посещении
                FASTCALL_12(void, 0x4F6C00,
                    this->GetVisitingMessage().String(),
                    1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -777);
            }

            return true;
        }

        return false;
    }
}
