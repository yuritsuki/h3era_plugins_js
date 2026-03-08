#include "../pch.h"
#include "MineralSpringExtender.h"

namespace mineralSpring
{
MineralSpringExtender::MineralSpringExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.WateringPlaceExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::MINERAL_SPRING;
    CreatePatches();
}

MineralSpringExtender::~MineralSpringExtender()
{
}

BOOL MineralSpringExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                               int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        const bool isVisitedByHero = H3MapItemMineralSpring::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            // Pattern used: return AIHero_GetObjectPosWeight_Oasis(hero, 64, 200, (int)moveCost); 0052A6C0
            // 64 - offset of temporary bonus, located in _H3Hero_ + 0x105
            // 200 - movePtsGiven/2

            const int checkDist = MOVE_POINTS_GIVEN / 2;

            if (*moveDistance < checkDist)
            {
                *moveDistance = 0;
                aiMapItemWeight = 10000;
            }
            else
            {
                *moveDistance -= checkDist;
                aiMapItemWeight = THISCALL_2(int, 0x528260, hero, LUCK_GIVEN);
            }
        }

        return true;
    }

    return false;
}


BOOL H3MapItemMineralSpring::IsVisitedByHero(const H3Hero *hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    return Era::GetAssocVarIntValue(newMapItem::buffer);
} 
void H3MapItemMineralSpring::SetAsVisited(const H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 1);
}

void H3MapItemMineralSpring::SetAsNotVisited(const H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 0);
}


void ShowMessage(const H3MapItem *mapItem, const bool isVisitedByHero)
{
    const bool skipMapMessage = globalPatcher->VarValue<int>("HD.UI.AdvMgr.SkipMapMsgs");

    H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));

    if (isVisitedByHero)
    {
        objName.Append(EraJS::read(
            H3String::Format("RMG.objectGeneration.%d.%d.text.visited", mapItem->objectType, mapItem->objectSubtype)
                .String()));

        if (skipMapMessage)
        {
            // AdvMgr_DisplayResourcesInfoPanel
            THISCALL_4(void, 0x415FC0, P_AdventureMgr->Get(), objName.String(), -1, -1);
        }
        else
        {
            H3Messagebox::Show(objName);
        }
    }
    else
    {
        objName.Append(EraJS::read(
            H3String::Format("RMG.objectGeneration.%d.%d.text.visit", mapItem->objectType, mapItem->objectSubtype)
                .String()));
        H3PictureCategories pics(ePictureCategories::POSITIVE_LUCK, -1);

        if (skipMapMessage)
        {
            FASTCALL_12(void, 0x4F6C00, objName.String(),
                1, -1, -1,
                pics.type, pics.subtype,
                -1, 0,
                -1, 0,
                -1, -777);
        }
        else
        {
            H3Messagebox::Show(objName, pics);
        }
    }
}

BOOL MineralSpringExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                         const BOOL isHuman) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);

        const bool isVisitedByHero = H3MapItemMineralSpring::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            H3MapItemMineralSpring::SetAsVisited(hero);
            hero->movement += MOVE_POINTS_GIVEN;
            hero->luckBonus += LUCK_GIVEN;

            if (isHuman)
            {
                THISCALL_4(void, 0x4032E0, P_AdventureManager->dlg, -1, 1, 1);
            }
        }

        if (isHuman)
        {
            ShowMessage(mapItem, isVisitedByHero);
        }
    }

    return false;
}

BOOL MineralSpringExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int playerId, BOOL isRightClick) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        H3String objName = RMGObjectInfo::GetObjectName(mapItem);
        int teamId = THISCALL_2(int, 0x4A55D0, P_Game->Get(), playerId);
        BOOL teamVisited = IsObjectVisitedByTeam(objectType, objectSubtype, teamId);

        // Add extra hint if object is visited at least once by the team
        if (teamVisited)
        {
            AddExtraInfoHint(&objName, isRightClick);
        }

        // Add "Visited/Not visited" hint
        if (hero)
        {
            const bool isVisitedByHero = H3MapItemMineralSpring::IsVisitedByHero(hero);
            AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
        }
        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

H3MapItemMineralSpring* MineralSpringExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemMineralSpring *>(reinterpret_cast<const H3MapItemMineralSpring *>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator *MineralSpringExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

MineralSpringExtender &MineralSpringExtender::Get()
{
    static MineralSpringExtender _instance;
    return _instance;
}
}
