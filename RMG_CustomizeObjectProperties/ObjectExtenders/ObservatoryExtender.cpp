#include "../pch.h"
#include "ObservatoryExtender.h"

namespace observatory
{
ObservatoryExtender::ObservatoryExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.ObservatoryExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectType146::OBSERVATORY;
    CreatePatches();
}

ObservatoryExtender::~ObservatoryExtender()
{
}

BOOL ObservatoryExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                             int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
     if (GetFromMapItem(mapItem))
     {
         const bool isVisitedByHero = H3MapItemObservatory::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            //const int currDay = P_Game->Get()->date.day;
            //const int weekProfit = 100 * (8 - currDay);
            //int a = 5;
            //if (*moveDistance < weekProfit)
            //{
            //    *moveDistance = 0;
            //    aiMapItemWeight = 10000;
            //}
            //else
            //{
            //    aiMapItemWeight = 50;
            //    *moveDistance -= weekProfit;
            //}

            aiMapItemWeight = 2 * (8 - P_Game->date.day) * (hero->maxMovement / 100) / 8;
        }

        return true;
     }

    return false;
}


BOOL H3MapItemObservatory::IsVisitedByHero(H3Hero *hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    return Era::GetAssocVarIntValue(newMapItem::buffer);
}
void H3MapItemObservatory::SetAsVisited(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 1);
}
void H3MapItemObservatory::SetAsNotVisited(H3Hero* hero) noexcept
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

BOOL ObservatoryExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                       const BOOL isHuman) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);

        const bool isVisitedByHero = H3MapItemObservatory::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            H3MapItemObservatory::SetAsVisited(hero);
            if (hero->owner <= 7 && hero->x >= 0)
            {
                int radius = hero->GetScoutingRadius();
                //TODO: check vTable visit for observatory
                //P_NetworkGame

                //H3Game::OpenArea
                THISCALL_7(void, 0x49CDD0, P_Game->Get(), hero->x, hero->y, hero->z, hero->owner, radius, 0);
            }
        }

        if (isHuman)
        {
            ShowMessage(mapItem, isVisitedByHero);
        }
    }

    return false;
}

BOOL ObservatoryExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int playerId, BOOL isRightClick) const noexcept
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
            const bool isVisitedByHero = H3MapItemObservatory::IsVisitedByHero(const_cast <H3Hero*> (hero));
            AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

INT32 __stdcall HiHook_004E42E0(HiHook* h, H3Hero* this_)
{
    // Get scouting radius
    INT32 result = THISCALL_1(INT32, h->GetDefaultFunc(), this_);

    if (H3MapItemObservatory::IsVisitedByHero(this_))
    {
        result += VISION_RADIUS_GIVEN;
    }
    return result;
}

void ObservatoryExtender::CreatePatches()
{
    if (!m_isInited)
    {
        _PI->WriteHiHook(0x4E42E0, SPLICE_, EXTENDED_, THISCALL_, HiHook_004E42E0);

        m_isInited = true;
    }
}

H3MapItemObservatory* ObservatoryExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemObservatory *>(reinterpret_cast<const H3MapItemObservatory *>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator* ObservatoryExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

ObservatoryExtender &ObservatoryExtender::Get()
{
    static ObservatoryExtender _instance;
    return _instance;
}
}
