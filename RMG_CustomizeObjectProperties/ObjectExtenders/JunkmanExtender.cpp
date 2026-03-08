#include "../pch.h"
#include "JunkmanExtender.h"

namespace junkman
{

JunkmanExtender::JunkmanExtender() : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.JunkmanExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::JUNKMAN;
    CreatePatches();
}

JunkmanExtender::~JunkmanExtender()
{
}

BOOL JunkmanExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                        int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    //if (H3MapItemJunkman::GetFromMapItem(mapItem))
    //{
    //    // 005288DE blackMarket AIcalc
    //    // if ( Hero_Art_GetCount_InBackPack(hero, 1) == 64 )
    //    //     return 0;
    //    if (THISCALL_2(int, 0x4D9330, hero, 0) == 0) // count without war machines
    //    {
    //        aiMapItemWeight = 0;
    //        return true;
    //    }


    //    //if (P_ActivePlayer->playerResources.gold >= GOLD_REQUIRED)
    //    //{
    //    //    // адрес похожего псевдокода 0052BB89
    //    //    const float aiExperience = EXP_GIVEN * hero->AI_experienceEffectiveness;
    //    //    aiMapItemWeight =
    //    //        static_cast<int>(aiExperience - player->resourceImportance[eResource::GOLD] * GOLD_REQUIRED);

    //    //}

    //    return true;
    //}

    return false;
}

int H3Hero_CountArtifactsInBackpack(H3Hero* hero)
{
    int count = 0;
    
    for (H3Artifact& art : hero->backpackArtifacts)
    {
        if (art.id != eArtifact::NONE && art.id != eArtifact::CATAPULT && art.id != eArtifact::BALLISTA
            && art.id != eArtifact::AMMO_CART && art.id != eArtifact::FIRST_AID_TENT && art.id != eArtifact::SPELL_SCROLL
            && art.id != eArtifact::GRAIL && art.id < limits::ARTIFACTS)
        {
            count++;
        }
    }
    return count;
}

//void AI_H3Hero_VisitJunkman(H3Hero* hero, int marketCount)
//{
//    // Можно было бы использовать H3Hero::CountArtifactsInBackpack, но надо отсортировать и свитки
//    if (H3Hero_CountArtifactsInBackpack(hero) > 0)
//    {
//        while (1)
//        {
//            v4 = -1;
//            v20 = 0;
//            v22 = 0;
//            v19 = v3;
//            do
//            {
//                eArtifact artId = *v3;
//                if (artId != -1)
//                {
//                    playerResources = P_ActivePlayer->playerResources;
//                    HIDWORD(v16) = get_artifact_purchase_price(artId, marketCount, &v21);
//                    if (SHIDWORD(v16) > playerResources[v21]
//                        || (a1[0] = artId,
//                            a1[1] = -1,
//                            v17 = (double)AI_Player_CalcGettingArt_Value((int)a1, P_CurrentPlayerID),
//                            v16 = (double)SHIDWORD(v16),
//                            v7 = (__int64)(v17 - v16 * P_ActivePlayer->ai.resourceImportance[v21]),
//                            (int)v7 < 0))
//                    {
//                        LODWORD(v7) = 0;
//                    }
//                    if ((int)v7 > v20)
//                    {
//                        v4 = v22;
//                        v20 = v7;
//                    }
//                }
//                v3 = v19 + 1;
//                v8 = ++v22 < 7;
//                ++v19;
//            } while (v8);
//            if (v4 < 0)
//                break;
//            v3 = v15;
//            artifact_purchase_price = get_artifact_purchase_price(v15[v4], marketCount, &ArtNum);
//            v10 = P_ActivePlayer->playerResources[ArtNum];
//            P2.subtype = -1;
//            P_ActivePlayer->playerResources[ArtNum] = v10 - artifact_purchase_price;
//            v11 = hp;
//            P2.id = v3[v4];
//            H3Hero::AddArtifact(hp, &P2, 1, 1);
//            v3[v4] = -1;
//            if (H3Hero::CountArtifactsInBackpack(v11, 1) == 64)
//                return;
//        }
//        AI::H3Hero::EquipArtifacts(hp);
//    }
//}

BOOL JunkmanExtender::VisitMapItem(H3Hero* hero, H3MapItem* mapItem, const H3Position pos,
    const BOOL isHuman) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        if (isHuman)
        {
            // patch object name
            H3String str = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem)).String();
            LPCSTR objName = str.String();
            const DWORD prevStr = DwordAt(0x5EC3CE + 2); // store object name
            DwordAt(0x5EC3CE + 2) = (int)&objName; // patch object name

            // jump button building in dlg build (check in debugger)
            auto patch = _pi->WriteHexPatch(0x5E5F7C, "E9 FA000000"); // if jump is less than 128 byte, use EB

            // Town_ArtMarket dlg after init
            P_MarketHero = hero;
            P_ArtifactMerchant = &P_Game->artifactMerchant;
            P_ActivePlayerMarkets = 7;
            P_MarketType = eMarketType::SELL_ARTIFACTS;
            //P_MarketBuilding = eMarketBuilding::JUNKMAN;
            
            // Run dlg
            CDECL_0(void, 0x5EA530);

            // restore global vars
            //IntAt(0x6AAB0C) = 0; // marker type
            IntAt(0x6AAB00) = 0; // markets amount owned by player
            DwordAt(0x5EC3CE + 2) = prevStr; // restore object name

            // cancel patch
            patch->Destroy();
        }
        //else
        //{
        //    AI_H3Hero_VisitJunkman(hero, 7);
        //}

        return true;
    }

    return false;
}

_LHF_(LoHook_005EE880)
{
    if (IntAt(0x6AAB2C) == 4)
    {
        //H3Hero* marketHero = *reinterpret_cast<H3Hero**>(0x6AAAE0);
        int slotId = c->eax;
        int a = 5;
        //eArtifact artId1 = static_cast<eArtifact>(marketHero->bodyArtifacts[slotId].id);

        eArtifact artId = eArtifact::NONE;
        // Артефакт на теле
        if (slotId < 19)
        {
            artId = static_cast<eArtifact>(P_MarketHero->bodyArtifacts[slotId].id);
        }
        // В рюкзаке
        else
        {
            artId = static_cast<eArtifact>(P_MarketHero->backpackArtifacts[slotId - 19].id);
        }
        
        if (artId == eArtifact::SPELL_SCROLL)
        {
            LPCSTR mes = EraJS::read(
                H3String::Format("RMG.objectGeneration.%d.%d.text.scrollsNotAllowed", eHotaObject::ACTIVE, eHotaObjectActiveType::JUNKMAN)
                .String());

            FASTCALL_12(void, 0x4F6C00, mes, 1, -1, -1, -1, 0, -1, 0, -1, 0, -1, 0);
            
            c->return_address = 0x5EE9A9;
            return NO_EXEC_DEFAULT;
        }
    }
    return EXEC_DEFAULT;
}


//_LHF_(LoHook_005EC3A7)
//{
//    H3BaseDlg* dlg = (H3BaseDlg*)c->ebx;
//    if (IntAt(0x6AAB2C) == 4)
//    {
//        // H3BaseDlg::RemoveControlState
//        THISCALL_3(void, 0x5FF520, dlg, 16, 4102);
//        THISCALL_3(void, 0x5FF520, dlg, 17, 4102);
//        THISCALL_3(void, 0x5FF520, dlg, 18, 4102);
//        THISCALL_3(void, 0x5FF520, dlg, 1000, 6);
//        THISCALL_3(void, 0x5FF520, dlg, 1001, 6);
//        THISCALL_3(void, 0x5FF520, dlg, 1002, 6);
//    }
//    else
//    {
//        THISCALL_3(void, 0x5FF490, dlg, 16, 6);               // H3BaseDlg::AddControlState
//        THISCALL_3(void, 0x5FF520, dlg, 16, 4096);
//        THISCALL_3(void, 0x5FF490, dlg, 17, 6);
//        THISCALL_3(void, 0x5FF520, dlg, 17, 4096);
//        THISCALL_3(void, 0x5FF490, dlg, 18, 6);
//        THISCALL_3(void, 0x5FF520, dlg, 18, 4096);
//    }
//    return 1;
//}

int __stdcall HiHook_H3SellArtifactDlg_WindowHandler(HiHook* h, H3BaseDlg* dlg, H3Msg* msg)
{
    //int result = THISCALL_2(int, h->GetDefaultFunc(), dlg, msg);

    // Клик по стрелке рюкзака
    if (msg->subtype == eMsgSubtype::LBUTTON_CLICK && (msg->itemId == 130 || msg->itemId == 131))
    {
        int a = P_MarketSelectedSlotIndex;

        //// Нет выбранного артефакта.
        //P_MarketSelectedSlotIndex = -1;

        ////H3SellArtifactDlg::Proc(dlg, 1);
        //THISCALL_2(void, 0x5EC280, dlg, 1);
        //int v28 = 0;
        //if (v28)
        //{
        //    msg->itemId = 10;
        //    msg->subtype = eMsgSubtype::END_DIALOG;
        //    return 2;
        //}
        //return 1;
        return THISCALL_2(int, h->GetDefaultFunc(), dlg, msg);
    }
    else {
        return THISCALL_2(int, h->GetDefaultFunc(), dlg, msg);
    }


    //return result;
}



void JunkmanExtender::CreatePatches()
{
    if (!m_isInited)
    {
        // Запрет продавать у старьевщика свитки кликом мыши
        //_pi->WriteLoHook(0x5EE880, LoHook_005EE880);

        //// Убираем кнопки переключения на торговлю ресурсами/покупаку артефактов
        //_pi->WriteLoHook(0x5EC3A7, LoHook_005EC3A7);

        // Делаем слот со свитком невалидным при переключении (запрет продавать свитки)
        //_PI->WriteHiHook(0x5EE360, SPLICE_, EXTENDED_, THISCALL_, HiHook_H3SellArtifactDlg_WindowHandler);
        //LoHook_ArtSell_Add5Slot_ArtSellSelect
        //LoHook_ArtSell_Backpack_ResetEmptySlotSelection
        

        m_isInited = true;
    }
}

H3MapItemJunkman* JunkmanExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemJunkman*>(reinterpret_cast<const H3MapItemJunkman*>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator* JunkmanExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

JunkmanExtender& JunkmanExtender::Get()
{
    static JunkmanExtender _instance;
    return _instance;
}
}
