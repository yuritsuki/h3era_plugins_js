#include "../pch.h"
#include "AncientLampExtender.h"

namespace ancientLamp
{

AncientLampExtender::AncientLampExtender() : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.AncientLampExtender.daemon_n"))
{
    objectType = eHotaObject::TYPE_145;
    objectSubtype = eHotaObjectType145::ANCIENT_LAMP;
    CreatePatches();
}

AncientLampExtender::~AncientLampExtender()
{
}

BOOL AncientLampExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                        int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    if (auto ancientLamp = GetFromMapItem(mapItem))
    {
        // AIHero_GetBuyingStacksValue_RefugeeCamp 0052ABF0
        // return AIHero_GetBuyingStacksValue_RefugeeCamp(hero, mapItem->SubType, mapItem->SetUp);
        aiMapItemWeight = FASTCALL_3(int, 0x52ABF0, hero, ancientLamp->creatureType, ancientLamp->creatureCount);

        return true;
    }

    return false;
}

BOOL AncientLampExtender::AskQuestion(H3MapItem *mapItem) const noexcept
{
    const bool skipMapMessage = globalPatcher->VarValue<int>("HD.UI.AdvMgr.SkipMapMsgs");

    if (skipMapMessage)
    {
        return true;
    }
    else
    {
        H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));

        objName.Append(EraJS::read(
            H3String::Format(visit_key, objectType, objectSubtype)
            .String()));

        return H3Messagebox::Choice(objName);
    }
}

BOOL AncientLampExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                  const BOOL isHuman) const noexcept
{

    if (auto ancientLamp = GetFromMapItem(mapItem))
    {
        // set object as visited
        THISCALL_2(void, 0x4FC620, mapItem, hero->owner);

        BOOL agreed; // = !isHuman;
        if (isHuman)
        {
            agreed = AskQuestion(mapItem);

            if (agreed)
            {
                INT16 creatureCount = ancientLamp->creatureCount;
                H3RecruitMgr mgr(hero->army, false, ancientLamp->creatureType, creatureCount);
                mgr.Run();
                ancientLamp->creatureCount = creatureCount;
            }
        } 
        else
        {
            // sub_005282F0(hero, SubType, &isAI);
            INT16 creatureCount = ancientLamp->creatureCount;
            // AI::H3Hero::BuyCreaturesInRefugeeCamp
            FASTCALL_3(void, 0x5282F0, hero, ancientLamp->creatureType, &creatureCount);
			ancientLamp->creatureCount = creatureCount;
        }
        // Delete the object if creatureCount == 0
        if (ancientLamp->creatureCount == 0)
        {
            // Delete the object
            THISCALL_4(void, 0x49DDE0, P_AdventureManager->Get(), mapItem, pos.Mixed(), isHuman);
        }

        return true;
    }

    return false;
}

BOOL AncientLampExtender::InitNewGameMapItemSetup(H3MapItem *mapItem) const noexcept
{

    if (const auto ancientLamp = GetFromMapItem(mapItem))
    {
        *ancientLamp = {};
        ancientLamp->creatureType = ANCIENT_LAMP_CREATURE_TYPE;
        ancientLamp->creatureCount = rand() % (ANCIENT_LAMP_MAX_CREATURE_COUNT - ANCIENT_LAMP_MIN_CREATURE_COUNT + 1) + ANCIENT_LAMP_MIN_CREATURE_COUNT; // from 4 to 6
        ancientLamp->Reset();

        return true;
    }

    return false;
}

BOOL AncientLampExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int playerId, BOOL isRightClick) const noexcept
{

    if (auto ancientLamp = GetFromMapItem(mapItem))
    {
        H3String objName = RMGObjectInfo::GetObjectName(mapItem);

        // Object scouting - use only for objects with "visited" property starting from 5 bit offset
        BOOL playerVisited = THISCALL_2(bool, 0x529B70, mapItem, H3HumanID::Get());

        if (playerVisited)
        {
            // Read string from json
            LPCSTR extraInfo = EraJS::read(H3String::Format(extraObjectInfo_key, objectType, objectSubtype).String());

            sprintf(h3_TextBuffer, "%s%s",
                isRightClick ? "\n\n" : " ",
                H3String::Format(extraInfo, P_CreatureInformation[ANCIENT_LAMP_CREATURE_TYPE].namePlural, ancientLamp->creatureCount).String());
            objName.Append(h3_TextBuffer);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());


        return true;
    }

    return false;
}

H3MapItemAncientLamp* AncientLampExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemAncientLamp*>(reinterpret_cast<const H3MapItemAncientLamp*>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator * AncientLampExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

void H3MapItemAncientLamp::Reset()
{
    visited = 0;
}

AncientLampExtender& AncientLampExtender::Get()
{
    static AncientLampExtender _instance;
    return _instance;
}
}
