#include "../pch.h"
#include "BogHermitExtender.h"

namespace bogHermit
{
BogHermitExtender::BogHermitExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.BogHermitExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = 14;
    CreatePatches();
}

BogHermitExtender::~BogHermitExtender()
{
}

BOOL BogHermitExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                             int &aiMapItemWeight, int *moveDistance,
                                             const H3Position pos) const noexcept
{
    if (auto bogHermit = GetFromMapItem(mapItem))
    {
        const bool isVisitedByHero = H3MapItemBogHermit::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            const int currDay = P_Game->Get()->date.day;
            const int weekProfit = 250 * (8 - currDay);
            if (*moveDistance < weekProfit)
            {
                *moveDistance = 0;
                aiMapItemWeight = 10000;
            }
            else
            {
                aiMapItemWeight = 50;
                *moveDistance -= weekProfit;
            }
        }

        return true;
    }

    return false;
}


BOOL H3MapItemBogHermit::IsVisitedByHero(H3Hero *hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    return Era::GetAssocVarIntValue(newMapItem::buffer);
}
void H3MapItemBogHermit::SetAsVisited(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 1);
}
void H3MapItemBogHermit::SetAsNotVisited(H3Hero* hero) noexcept
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

        if (skipMapMessage)
        {
            THISCALL_4(void, 0x415FC0, P_AdventureMgr->Get(), objName.String(), -1, -1);
        }
        else
        {
            H3Messagebox::Show(objName);
        }
    }
}

BOOL BogHermitExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                       const BOOL isHuman) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);

        const bool isVisitedByHero = H3MapItemBogHermit::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            H3MapItemBogHermit::SetAsVisited(hero);
        }

        if (isHuman)
        {
            ShowMessage(mapItem, isVisitedByHero);
        }
    }

    return false;
}

BOOL BogHermitExtender::SetHintInH3TextBuffer(H3MapItem *mapItem, const H3Hero *hero, int playerId, BOOL isRightClick) const noexcept
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
            const bool isVisitedByHero = H3MapItemBogHermit::IsVisitedByHero((H3Hero*)hero);
            AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

int __stdcall GetStepCost(HiHook *h, H3MapItem *mapItem, char direction, signed int movementLeft, int pathfindSkill,
                            int road, int flyPower, int waterWalkPower, int nativeLand, int nomadsCount)
{
    INT8 targetLandType = mapItem->land;

    if (H3Hero *hero = P_ActivePlayer->GetActiveHero())
    {
        if (targetLandType == BOG_HERMIT_TERRAIN_TYPE && !road)
        {
            const bool isVisitedByHero = H3MapItemBogHermit::IsVisitedByHero(hero);
            if (isVisitedByHero)
            {
                int stepTrailCost = 75;
                if ((direction & 1) != 0)
                {
                    stepTrailCost = static_cast<int>(static_cast<float>(stepTrailCost) * 1.4142135);
                }

                return stepTrailCost;
            }
        }
    }

    return FASTCALL_9(int, h->GetDefaultFunc(), mapItem, direction, movementLeft, pathfindSkill, road, flyPower,
                      waterWalkPower, nativeLand, nomadsCount);
}

void BogHermitExtender::CreatePatches()
{
    if (!m_isInited)
    {
        _pi->WriteHiHook(0x4B14A0, FASTCALL_, GetStepCost);

        m_isInited = true;
    }
}

H3MapItemBogHermit * BogHermitExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemBogHermit *>(reinterpret_cast<const H3MapItemBogHermit *>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator *BogHermitExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

BogHermitExtender &BogHermitExtender::Get()
{
    static BogHermitExtender _instance;
    return _instance;
}
}
