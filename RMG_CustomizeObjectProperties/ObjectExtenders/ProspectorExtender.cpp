#include "../pch.h"
#include "ProspectorExtender.h"

namespace prospector
{
    ProspectorExtender::ProspectorExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.GraveExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::PROSPECTOR;
    CreatePatches();
}

    ProspectorExtender::~ProspectorExtender()
{
}

BOOL ProspectorExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                               int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    if (auto objSetup = GetFromMapItem(mapItem))
    {
        const bool isVistedByPlayer = THISCALL_2(bool, 0x529B70, mapItem, H3CurrentPlayerID::Get());
        UINT16 resNum = objSetup->resourceAmount;

        // if AI player didnt' visit or there is resources
        if (!isVistedByPlayer || resNum)
        {
            UINT16 resType = objSetup->resourceType;

            aiMapItemWeight =
                static_cast<int>(player->resourceImportance[resType] * resNum); // static_cast<double*>()
        }
        return true;
    }

    return false;
}

BOOL ProspectorExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                         const BOOL isHuman) const noexcept
{
    if (auto objSetup = GetFromMapItem(mapItem))
    {
        const bool skipMapMessage = globalPatcher->VarValue<int>("HD.UI.AdvMgr.SkipMapMsgs");

        // set object as visited
        THISCALL_2(void, 0x4FC620, mapItem, hero->owner);

        const INT16 resType = objSetup->resourceType;
        const INT16 resNum = objSetup->resourceAmount;

        if (resNum > 0)
        {
            // remove resources from the object
            objSetup->resourceAmount = 0;

            // add resourcse
            THISCALL_3(void, 0x4E3870, hero, resType, resNum);

            if (isHuman)
            {
                H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));
                objName.Append(EraJS::read(
                    H3String::Format("RMG.objectGeneration.%d.%d.text.visit", mapItem->objectType, mapItem->objectSubtype)
                    .String()));

                FASTCALL_12(void, 0x4F6C00, objName.String(),
                    1, -1, -1,
                    resType, resNum,
                    -1, 0,
                    -1, 0,
                    -1, skipMapMessage ? -777 : 0);
            }
        }
        else if (isHuman)
        {
            H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));
            objName.Append(EraJS::read(
                H3String::Format("RMG.objectGeneration.%d.%d.text.visited", mapItem->objectType, mapItem->objectSubtype)
                .String()));

            FASTCALL_12(void, 0x4F6C00, objName.String(),
                1, -1, -1,
                -1, 0,
                -1, 0,
                -1, 0,
                -1, skipMapMessage ? -777 : 0);
        }
        return true;
    }

    return false;
}

BOOL ProspectorExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int playerId, BOOL isRightClick) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        H3String objName = RMGObjectInfo::GetObjectName(mapItem);

        const bool isVistedByPlayer = THISCALL_2(bool, 0x529B70, mapItem, H3HumanID::Get());
        if (isVistedByPlayer)
        {
            sprintf(h3_TextBuffer, "%s%s", isRightClick ? "\n\n" : " ", P_GeneralText->GetText(354));
            objName.Append(h3_TextBuffer);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

H3MapItemProspector* ProspectorExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemProspector*>(reinterpret_cast<const H3MapItemProspector*>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator * ProspectorExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

void H3MapItemProspector::Reset()
{
    resourceAmount = 0;
    visited = 0;
    resourceType = FASTCALL_2(INT32, 0x50C7C0, 1, 2) * 3;
    if (resourceType == eResource::SULFUR) resourceAmount = 5;
    else resourceAmount = 500;
}

BOOL ProspectorExtender::InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept
{
    if (auto prospector = GetFromMapItem(mapItem))
    {
        prospector->Reset();
        return true;
    }
    return false;
}
BOOL ProspectorExtender::InitNewWeekMapItemSetup(H3MapItem* mapItem) const noexcept
{
    if (auto prospector = GetFromMapItem(mapItem))
    {
        prospector->Reset();
        return true;
    }
    return false;
}

ProspectorExtender& ProspectorExtender::Get()
{
    static ProspectorExtender _instance;
    return _instance;
}
}
