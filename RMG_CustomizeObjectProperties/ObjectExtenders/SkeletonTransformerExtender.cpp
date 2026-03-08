#include "../pch.h"
#include "SkeletonTransformerExtender.h"

namespace skeletonTransformer
{

SkeletonTransformerExtender::SkeletonTransformerExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.SkeletonTransformerExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::SKELETON_TRANSFORMER;
    CreatePatches();
}

SkeletonTransformerExtender::~SkeletonTransformerExtender()
{
}

BOOL SkeletonTransformerExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                                     int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    // TODO: look at: AI town manager (or visiting) - transformer
    // if (auto skeletonTransformer = H3MapItemSkeletonTransformer::GetFromMapItem(mapItem))
    //{

    //    if (P_ActivePlayer->playerResources.gold >= GOLD_REQUIRED)
    //    {
    //        // адрес похожего псевдокода 0052BB89
    //        const float aiExperience = EXP_GIVEN * hero->AI_experienceEffectiveness;
    //        aiMapItemWeight =
    //            static_cast<int>(aiExperience - player->resourceImportance[eResource::GOLD] * GOLD_REQUIRED);

    //    }

    //    return true;
    //}

    return false;
}

BOOL SkeletonTransformerExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                               const BOOL isHuman) const noexcept
{

    if (GetFromMapItem(mapItem))
    {
        if (isHuman)
        {
            H3Army* army = &hero->army;
            char dlgBuffer[1024]; // Memory for dlg
            H3Dlg* dlg = (H3Dlg*)dlgBuffer;
            THISCALL_2(void, 0x0565770, dlg, army);         // Dlg_SkeletTransformer_Create
            P_MouseManager->SetCursor(0, eCursor::ADVENTURE);// Change cursor to default
            THISCALL_2(void, 0x05FFA20, dlg, 0);            // H3BaseDlg::ShowAndRun
            THISCALL_1(void, 0x05661E0, dlg);               // Dlg_SkeletTransformer_Destroy
            memset(Era::z[1], 0, 512);                      // Clear memory
        }

        return true;
    }

    return false;
}

H3MapItemSkeletonTransformer* SkeletonTransformerExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemSkeletonTransformer *>(reinterpret_cast<const H3MapItemSkeletonTransformer *>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator *SkeletonTransformerExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

SkeletonTransformerExtender &SkeletonTransformerExtender::Get()
{
    static SkeletonTransformerExtender _instance;
    return _instance;
}
}
