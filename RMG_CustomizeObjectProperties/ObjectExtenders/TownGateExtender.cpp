#include "../pch.h"
#include "TownGateExtender.h"

namespace townGate
{
TownGateExtender::TownGateExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.TownGateExtender.daemon_n"))
{
    objectType = eObject::PYRAMID;
    objectSubtype = eWogObject::TOWN_GATE;
    CreatePatches();
}

TownGateExtender::~TownGateExtender()
{
}

int townId = -1;


 BOOL TownGateExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                           int &aiMapItemWeight, int *moveDistance, const H3Position pos) const
                                           noexcept
{
     if (GetFromMapItem(mapItem))
     {
         H3Game* game = P_Game->Get();
         int playerTeamId = game->GetPlayerTeam(hero->owner);
         townId = -1;

         for (size_t i = 0; i < limits::TOWNS_ON_MAP; i++)
         {
             H3Town* town = &game->towns[i];

             if (town->owner > -1 && town->visitingHero == -1 && playerTeamId == game->GetPlayerTeam(town->owner))
             {
                 // AI::H3Hero::CalculateMapPosWeight
                 int newAiTownVal = FASTCALL_3(int, 0x528520, hero, H3Position(town->x, town->y, town->z), moveDistance);

                 if (newAiTownVal > aiMapItemWeight)
                 {
                     aiMapItemWeight = newAiTownVal;
                     townId = i;
                 }
             }
         }

         return true;
     }

     return false;
 }

void __stdcall H_ObjVisit(
    HiHook* h,
    H3AdventureManager* advMgr,
    H3Hero* hero,
    H3MapItem* cell,
    H3Position pos,
    bool isHuman)
{
    // Посещается town gate
    if (cell->objectType == eObject::PYRAMID && cell->objectSubtype == eWogObject::TOWN_GATE)
    {

        if (isHuman)
        {
            // movecost free teleport
            //0041D531 + 3 = C8
            auto patch = _PI->WriteHexPatch(0x41D531 + 3, "00");

            // mana free teleport
            //0041DA0B -> 0041DA2B
            auto patch2 = _PI->WriteJmp(0x41DA0B, 0x41DA2B);

            // patch spell name
            // get new name
            H3String str = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(cell)).String();
            LPCSTR objName = str.String();
            // store old name
            LPCSTR spellName = P_Spell[eSpell::TOWN_PORTAL].name;
            // patch
            P_Spell[eSpell::TOWN_PORTAL].name = objName;

            // patch spell def
            Era::RedirectFile("SpellScr.def:0_9.png", "data/pcx/SFzerk2.png");

            P_MouseManager->SetCursor(eAdvCursor::ARROWPOINTER, eCursor::ADVENTURE);
            P_MouseManager->TurnOn();
            // H3AdventureManager::CastTownPortal
            THISCALL_2(void, 0x41D4E0, P_AdventureManager->Get(), eSecSkillLevel::EXPERT);

            // restore old name
            P_Spell[eSpell::TOWN_PORTAL].name = spellName;
            // restore old def
            Era::RedirectFile("SpellScr.def:0_9.png", "");

            // undo patches
            patch->Destroy();
            patch2->Destroy();
        }
        else
        {
            if (townId > -1)
            {
                H3Town* targetTown = &P_Game->towns[townId];
                THISCALL_7(void, 0x41DAB0, P_AdventureManager->Get(), hero, H3Position(targetTown->x, targetTown->y, targetTown->z), 0, 0, 1, 0);
                townId = -1;
            }
        }
    }
    else
    {
        // Посещается не town gate - всё как обычно
        THISCALL_5(void, h->GetDefaultFunc(), advMgr, hero, cell, pos, isHuman);
    }
}

 void TownGateExtender::CreatePatches()
{
     if (!m_isInited)
     {
         // Посещение объекта
         _pi->WriteHiHook(0x705979, SPLICE_, EXTENDED_, THISCALL_, H_ObjVisit);

         m_isInited = true;
     }
 }

H3MapItemTownGate* TownGateExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemTownGate *>(reinterpret_cast<const H3MapItemTownGate *>(&mapItem->setup));
    }

    return nullptr;
}

BOOL TownGateExtender::InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        mapItem->setup = 0;
        return true;
    }
    return false;
}

H3RmgObjectGenerator* TownGateExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    // Due to wog object extender we need to return nullptr here in order to not create new extender
    return nullptr;
}

TownGateExtender &TownGateExtender::Get()
{
    static TownGateExtender _instance;
    return _instance;
}
}
