#include "../pch.h"

#include "WateringPlaceExtender.h"
namespace wateringPlace
{
WateringPlaceExtender::WateringPlaceExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.WateringPlaceExtender.daemon_n"))
{

    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::WATERING_PLACE;

    CreatePatches();
}

WateringPlaceExtender::~WateringPlaceExtender()
{
}

BOOL WateringPlaceExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                               int &aiMapItemWeight, int *moveDistance,
                                               const H3Position pos) const noexcept
{
    if (H3MapItemWateringPlace::GetFromMapItem(mapItem))
    {
        const bool isVisitedByHero = H3MapItemWateringPlace::IsVisitedByHero(hero);

        if (isVisitedByHero
            //|| THISCALL_2(BOOL8, 0x529B70, mapItem, player->ownerID) && (mapItem->setup & 1) != 0
            || *moveDistance >= 0 && *moveDistance > hero->movement
            || hero->movement >= 500
            )
        {
            aiMapItemWeight = 0;
            return true;
        }
        *moveDistance = 0;
        aiMapItemWeight = 10000;
        return true;
    }

    return false;
}

BOOL H3MapItemWateringPlace::IsVisitedByHero(const H3Hero *hero) noexcept
{
    sprintf(h3_TextBuffer, ErmVariableFormat, hero->id);

    return Era::GetAssocVarIntValue(h3_TextBuffer);
}


void H3MapItemWateringPlace::SetAsVisited(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 1);
}

void H3MapItemWateringPlace::SetAsNotVisited(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 0);
}

void ShowMessage(const H3MapItem *mapItem)
{
    const bool skipMapMessage = globalPatcher->VarValue<int>("HD.UI.AdvMgr.SkipMapMsgs");

    H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));

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

BOOL AskQuestion(const H3MapItem *mapItem)
{
    H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));

    objName.Append(EraJS::read(
        H3String::Format("RMG.objectGeneration.%d.%d.text.visit", mapItem->objectType, mapItem->objectSubtype)
            .String()));

    return H3Messagebox::Choice(objName);
}

BOOL WateringPlaceExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                         const BOOL isHuman) const noexcept
{
    if (H3MapItemWateringPlace::GetFromMapItem(mapItem))
    {
        ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);

        const bool isVisitedByHero = H3MapItemWateringPlace::IsVisitedByHero(hero);

        if (!isVisitedByHero)
        {
            BOOL agreed = !isHuman;
            if (isHuman)
            {
                agreed = AskQuestion(mapItem);
            }
            if (agreed)
            {
                H3MapItemWateringPlace::SetAsVisited(hero);
                hero->movement = 0;
                hero->maxMovement += MOVE_POINTS_GIVEN;
                hero->RecalculateMovement();
                sprintf(h3_TextBuffer, H3MapItemWateringPlace::ErmVariableFormat,
                        hero->id);                          // ��������� ����� ����������
                Era::SetAssocVarIntValue(h3_TextBuffer, 1); // �������� ����������, ��� ������ �������
            }

            return true;
        }

        if (isHuman)
        {
            ShowMessage(mapItem);
        }
    }

    return false;
}

BOOL WateringPlaceExtender::SetHintInH3TextBuffer(H3MapItem *mapItem, const H3Hero *hero, const int interactPlayerId,
                                                  const BOOL isRightClick) const noexcept
{

    if (H3MapItemWateringPlace::GetFromMapItem(mapItem))
    {
        H3String objName = RMGObjectInfo::GetObjectName(mapItem);
        int teamId = THISCALL_2(int, 0x4A55D0, P_Game->Get(), interactPlayerId);
        BOOL teamVisited = IsObjectVisitedByTeam(objectType, objectSubtype, teamId);

        // Add extra hint if object is visited at least once by the team
        if (teamVisited)
        {
            AddExtraInfoHint(&objName, isRightClick);
        }

        // Add "Visited/Not visited" hint
        if (hero)
        {
            const bool isVisitedByHero = H3MapItemWateringPlace::IsVisitedByHero(hero);
            AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

_ERH_(OnEveryDay)
{
    for (const auto heroId : P_Game->players[P_CurrentPlayerID].heroIDs)
    {
        auto hero = &P_Game->heroes[heroId];

        if (H3MapItemWateringPlace::IsVisitedByHero(hero))
        {
            H3MapItemWateringPlace::SetAsNotVisited(hero);
            hero->movement += MOVE_POINTS_GIVEN;
        }
    }
}


_LHF_(WateringPlace_HeroReset)
{
    int heroId = c->edx; // !!UN:C(hook)/(STRUCT_HOOK_CONTEXT_EDX)/4/?(heroId:y);
    sprintf(h3_TextBuffer, H3MapItemWateringPlace::ErmVariableFormat, heroId); // ��������� ����� ����������
    Era::SetAssocVarIntValue(h3_TextBuffer, 0);                                // �������� ����������

    return EXEC_DEFAULT;
}

void WateringPlaceExtender::CreatePatches()
{
    if (!m_isInited)
    {
        Era::RegisterHandler(OnEveryDay, "OnEveryDay");      // ������ ������ ��������� � ��������� �������� � ERA
        //_pi->WriteLoHook(0x4D89B8, WateringPlace_HeroReset); // 5081528 = 4D89B8 - hero reset // �������� ���������

        m_isInited = true;
    }
}

H3MapItemWateringPlace *H3MapItemWateringPlace::GetFromMapItem(const H3MapItem *mapItem) noexcept
{
    if (mapItem && mapItem->objectType == extender::HOTA_OBJECT_TYPE &&
        mapItem->objectSubtype == WATERING_PLACE_OBJECT_SUBTYPE)
    {
        // return reinterpret_cast<H3MapItemWateringPlace *>(&mapItem->setup);
        return const_cast<H3MapItemWateringPlace *>(reinterpret_cast<const H3MapItemWateringPlace *>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator *WateringPlaceExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == extender::HOTA_OBJECT_TYPE && objectInfo.subtype == WATERING_PLACE_OBJECT_SUBTYPE)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

WateringPlaceExtender *WateringPlaceExtender::instance = nullptr;

WateringPlaceExtender &WateringPlaceExtender::Get()
{
    if (!instance)
        instance = new WateringPlaceExtender();
    return *instance;
}
} // namespace wateringPlace
