#include "../pch.h"
#include "WarlocksLab.h"

namespace warlocksLab
{

WarlocksLabExtender::WarlocksLabExtender() : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.WarlocksLabExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::WARLOCKS_LAB;
    CreatePatches();
}

WarlocksLabExtender::~WarlocksLabExtender()
{
}

BOOL WarlocksLabExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
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

BOOL WarlocksLabExtender::VisitMapItem(H3Hero* hero, H3MapItem* mapItem, const H3Position pos,
    const BOOL isHuman) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        if (isHuman)
        {
            //// patch object name
            //H3String str = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem)).String();
            //LPCSTR objName = str.String();
            //const DWORD prevStr = DwordAt(0x5EC3CE + 2); // store object name
            //DwordAt(0x5EC3CE + 2) = (int)&objName; // patch object name

            //// jump button building in dlg build (check in debugger)
            //auto patch = _pi->WriteHexPatch(0x5DFE35, "E9 51010000"); // if jump is less than 128 byte, use EB
            //5DFF8B
            // Town_ArtMarket dlg after init
            P_MarketHero = hero;
            P_ArtifactMerchant = &P_Game->artifactMerchant;
            P_ActivePlayerMarkets = 10;  // markets amount owned by player
            P_MarketType = eMarketType::TRADE_RESOURCES;
            P_MarketBuilding = eMarketBuilding::WARLOCKS_LAB;
            
            // Run dlg
            CDECL_0(void, 0x5EA530);

            //// restore global vars
            ////IntAt(0x6AAB0C) = 0; // marker type
            //IntAt(0x6AAB00) = 0; // markets amount owned by player
            //DwordAt(0x5EC3CE + 2) = prevStr; // restore object name

            //// cancel patch
            //patch->Destroy();
        }
        //else
        //{
        //    AI_H3Hero_VisitJunkman(hero, 7);
        //}

        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

void H3WarlocksLabDlg_Ctor(H3BaseDlg* dlg)
{
    // H3BaseDlg::RemoveControlState
    THISCALL_3(void, 0x5FF520, dlg, 17, 6);
    THISCALL_3(void, 0x5FF520, dlg, 1001, 6);
    THISCALL_3(void, 0x5FF520, dlg, 21, 6);
    THISCALL_3(void, 0x5FF520, dlg, 23, 6);
    THISCALL_3(void, 0x5FF520, dlg, 27, 6);
    THISCALL_3(void, 0x5FF520, dlg, 35, 6);
    THISCALL_3(void, 0x5FF520, dlg, 37, 6);
    THISCALL_3(void, 0x5FF520, dlg, 41, 6);
    THISCALL_3(void, 0x5FF520, dlg, 28, 6);
    THISCALL_3(void, 0x5FF520, dlg, 30, 6);
    THISCALL_3(void, 0x5FF520, dlg, 34, 6);
    THISCALL_3(void, 0x5FF520, dlg, 42, 6);
    THISCALL_3(void, 0x5FF520, dlg, 44, 6);
    THISCALL_3(void, 0x5FF520, dlg, 48, 6);
    THISCALL_3(void, 0x5FF520, dlg, 77, 6);
    THISCALL_3(void, 0x5FF520, dlg, 79, 6);
    THISCALL_3(void, 0x5FF520, dlg, 83, 6);
    THISCALL_3(void, 0x5FF520, dlg, 63, 6);
    THISCALL_3(void, 0x5FF520, dlg, 65, 6);
    THISCALL_3(void, 0x5FF520, dlg, 69, 6);

    H3DlgItem* v1 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 2);   // H3BaseDlg::GetH3DlgItem
    if (v1)
    {
        THISCALL_3(void, 0x5FED80, v1, 52, 284);              // H3DlgItem::SendCommand
        THISCALL_3(void, 0x5FED80, v1, 53, 50);
        THISCALL_3(void, 0x5FED80, v1, 61, 225);
    }
    H3DlgItem* v3 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 14);
    if (v3)
    {
        THISCALL_3(void, 0x5FED80, v3, 52, 25);
        THISCALL_3(void, 0x5FED80, v3, 53, 139);
        THISCALL_3(void, 0x5FED80, v3, 61, 228);
    }
    H3DlgItem* v5 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 15);
    if (v5)
    {
        THISCALL_3(void, 0x5FED80, v5, 52, 280);
        THISCALL_3(void, 0x5FED80, v5, 53, 139);
        THISCALL_3(void, 0x5FED80, v5, 61, 228);
    }
    H3DlgItem* v7 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 7);
    if (v7)
    {
        THISCALL_3(void, 0x5FED80, v7, 52, 196);
        THISCALL_3(void, 0x5FED80, v7, 53, 448);
    }
    H3DlgItem* v9 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 1004);
    if (v9)
    {
        THISCALL_3(void, 0x5FED80, v9, 52, 195);
        THISCALL_3(void, 0x5FED80, v9, 53, 447);
    }
    H3DlgItem* v11 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 5);
    if (v11)
    {
        THISCALL_3(void, 0x5FED80, v11, 52, 270);
        THISCALL_3(void, 0x5FED80, v11, 53, 448);
    }

    THISCALL_5(void, 0x5FF400, dlg, 512, 9, 5, "itranmt.def");

    H3DlgItem* v13 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 1005);
    if (v13)
    {
        THISCALL_3(void, 0x5FED80, v13, 52, 269);
        THISCALL_3(void, 0x5FED80, v13, 53, 447);
    }

    THISCALL_5(void, 0x5FF400, dlg, 512, 9, 30722, "i_ok67.def");

    H3DlgItem* v15 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 30722);
    if (v15)
    {
        THISCALL_3(void, 0x5FED80, v15, 52, 414);
        THISCALL_3(void, 0x5FED80, v15, 53, 448);
        THISCALL_3(void, 0x5FED80, v15, 61, 67);
    }

    //THISCALL_5(void, 0x5FF400, dlg, 512, 11, 1003, "fr32_67.p32");

    H3DlgItem* v17 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 1003);
    if (v17)
    {
        THISCALL_3(void, 0x5FED80, v17, 52, 413);
        THISCALL_3(void, 0x5FED80, v17, 53, 447);
        THISCALL_3(void, 0x5FED80, v17, 61, 69);
    }
    H3DlgItem* v19 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 6);
    if (v19)
    {
        THISCALL_3(void, 0x5FED80, v19, 52, 198);
        THISCALL_3(void, 0x5FED80, v19, 53, 414);
    }
    H3DlgItem* v21 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 3);
    if (v21)
    {
        THISCALL_3(void, 0x5FED80, v21, 52, 72);
        THISCALL_3(void, 0x5FED80, v21, 53, 381);
    }
    H3DlgItem* v23 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 4);
    if (v23)
    {
        THISCALL_3(void, 0x5FED80, v23, 52, 55);
        THISCALL_3(void, 0x5FED80, v23, 53, 421);
    }
    H3DlgItem* v25 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 11);
    if (v25)
    {
        THISCALL_3(void, 0x5FED80, v25, 52, 432);
        THISCALL_3(void, 0x5FED80, v25, 53, 381);
    }
    H3DlgItem* v27 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 12);
    if (v27)
    {
        THISCALL_3(void, 0x5FED80, v27, 52, 415);
        THISCALL_3(void, 0x5FED80, v27, 53, 421);
    }
    H3DlgItem* v29 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 22);
    if (v29)
    {
        THISCALL_3(void, 0x5FED80, v29, 52, 72);
        THISCALL_3(void, 0x5FED80, v29, 53, 191);
    }
    H3DlgItem* v31 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 36);
    if (v31)
    {
        THISCALL_3(void, 0x5FED80, v31, 52, 55);
        THISCALL_3(void, 0x5FED80, v31, 53, 231);
    }
    H3DlgItem* v33 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 29);
    if (v33)
    {
        THISCALL_3(void, 0x5FED80, v33, 52, 51);
        THISCALL_3(void, 0x5FED80, v33, 53, 181);
    }
    H3DlgItem* v35 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 24);
    if (v35)
    {
        THISCALL_3(void, 0x5FED80, v35, 52, 177);
        THISCALL_3(void, 0x5FED80, v35, 53, 191);
    }
    H3DlgItem* v37 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 38);
    if (v37)
    {
        THISCALL_3(void, 0x5FED80, v37, 52, 160);
        THISCALL_3(void, 0x5FED80, v37, 53, 231);
    }
    H3DlgItem* v39 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 31);
    if (v39)
    {
        THISCALL_3(void, 0x5FED80, v39, 52, 156);
        THISCALL_3(void, 0x5FED80, v39, 53, 181);
    }
    H3DlgItem* v41 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 25);
    if (v41)
    {
        THISCALL_3(void, 0x5FED80, v41, 52, 72);
        THISCALL_3(void, 0x5FED80, v41, 53, 270);
    }
    H3DlgItem* v43 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 39);
    if (v43)
    {
        THISCALL_3(void, 0x5FED80, v43, 52, 55);
        THISCALL_3(void, 0x5FED80, v43, 53, 310);
    }
    H3DlgItem* v45 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 32);
    if (v45)
    {
        THISCALL_3(void, 0x5FED80, v45, 52, 51);
        THISCALL_3(void, 0x5FED80, v45, 53, 260);
    }
    H3DlgItem* v47 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 26);
    if (v47)
    {
        THISCALL_3(void, 0x5FED80, v47, 52, 177);
        THISCALL_3(void, 0x5FED80, v47, 53, 270);
    }
    H3DlgItem* v49 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 40);
    if (v49)
    {
        THISCALL_3(void, 0x5FED80, v49, 52, 160);
        THISCALL_3(void, 0x5FED80, v49, 53, 310);
    }
    H3DlgItem* v51 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 33);
    if (v51)
    {
        THISCALL_3(void, 0x5FED80, v51, 52, 156);
        THISCALL_3(void, 0x5FED80, v51, 53, 260);
    }
    H3DlgItem* v53 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 43);
    if (v53)
    {
        THISCALL_3(void, 0x5FED80, v53, 52, 327);
        THISCALL_3(void, 0x5FED80, v53, 53, 191);
    }
    H3DlgItem* v55 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 78);
    if (v55)
    {
        THISCALL_3(void, 0x5FED80, v55, 52, 310);
        THISCALL_3(void, 0x5FED80, v55, 53, 231);
    }
    H3DlgItem* v57 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 64);
    if (v57)
    {
        THISCALL_3(void, 0x5FED80, v57, 52, 306);
        THISCALL_3(void, 0x5FED80, v57, 53, 181);
    }
    H3DlgItem* v59 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 45);
    if (v59)
    {
        THISCALL_3(void, 0x5FED80, v59, 52, 432);
        THISCALL_3(void, 0x5FED80, v59, 53, 191);
    }
    H3DlgItem* v61 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 80);
    if (v61)
    {
        THISCALL_3(void, 0x5FED80, v61, 52, 415);
        THISCALL_3(void, 0x5FED80, v61, 53, 231);
    }
    H3DlgItem* v63 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 66);
    if (v63)
    {
        THISCALL_3(void, 0x5FED80, v63, 52, 411);
        THISCALL_3(void, 0x5FED80, v63, 53, 181);
    }
    H3DlgItem* v65 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 46);
    if (v65)
    {
        THISCALL_3(void, 0x5FED80, v65, 52, 327);
        THISCALL_3(void, 0x5FED80, v65, 53, 270);
    }
    H3DlgItem* v67 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 81);
    if (v67)
    {
        THISCALL_3(void, 0x5FED80, v67, 52, 310);
        THISCALL_3(void, 0x5FED80, v67, 53, 310);
    }
    H3DlgItem* v69 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 67);
    if (v69)
    {
        THISCALL_3(void, 0x5FED80, v69, 52, 306);
        THISCALL_3(void, 0x5FED80, v69, 53, 260);
    }
    H3DlgItem* v71 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 47);
    if (v71)
    {
        THISCALL_3(void, 0x5FED80, v71, 52, 432);
        THISCALL_3(void, 0x5FED80, v71, 53, 270);
    }
    H3DlgItem* v73 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 82);
    if (v73)
    {
        THISCALL_3(void, 0x5FED80, v73, 52, 415);
        THISCALL_3(void, 0x5FED80, v73, 53, 310);
    }
    H3DlgItem* v75 = THISCALL_2(H3DlgItem*, 0x5FF5B0, dlg, 68);
    if (v75)
    {
        THISCALL_3(void, 0x5FED80, v75, 52, 411);
        THISCALL_3(void, 0x5FED80, v75, 53, 260);
    }
}



H3BaseDlg* __stdcall HiHook_005df9f0(HiHook* h, H3BaseDlg* this_, int x, int y)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        IntAt(0x6AAB04) = x + 33;
        IntAt(0x6AAB08) = y + 34;
    }
    H3BaseDlg* result = THISCALL_3(H3BaseDlg*, h->GetDefaultFunc(), this_, x, y);

    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        H3WarlocksLabDlg_Ctor(this_);
    }
    return result;
}

_LHF_(LoHook_005dfa22)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        *(DWORD*)c->esp = 534;
        *(DWORD*)(c->esp + 4) = 526;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005dfaf7)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        c->esp -= 4;
        *(DWORD*)c->esp = (int)"TPLABTRN.pcx";
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_WarlocksLab_Smt)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        *(DWORD*)c->esp = 501;
        *(DWORD*)(c->esp + 4) = 518;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eab8b)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        LPCSTR dlgHint = EraJS::read(H3String::Format("RMG.objectGeneration.%d.%d.text.dlgHintDeal", eHotaObject::ACTIVE, eHotaObjectActiveType::WARLOCKS_LAB).String());
        c->eax = (int)dlgHint;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eabcb)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        LPCSTR dlgHint = EraJS::read(H3String::Format("RMG.objectGeneration.%d.%d.text.dlgHintDealDone", eHotaObject::ACTIVE, eHotaObjectActiveType::WARLOCKS_LAB).String());
        c->eax = (int)dlgHint;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eabd3)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        LPCSTR dlgHint = EraJS::read(H3String::Format("RMG.objectGeneration.%d.%d.text.dlgHint", eHotaObject::ACTIVE, eHotaObjectActiveType::WARLOCKS_LAB).String());
        c->eax = (int)dlgHint;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eac0e)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        LPCSTR dlgHint = EraJS::read(H3String::Format("RMG.objectGeneration.%d.%d.name", eHotaObject::ACTIVE, eHotaObjectActiveType::WARLOCKS_LAB).String());
        c->edi = (int)dlgHint;

        c->return_address = 0x5EAC45;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eacb2)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        LPCSTR dlgHint = EraJS::read(H3String::Format("RMG.objectGeneration.%d.%d.text.dlgHintAvalObjs", eHotaObject::ACTIVE, eHotaObjectActiveType::WARLOCKS_LAB).String());
        c->edi = (int)dlgHint;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eaec0)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        c->return_address = 0x5EAF23;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eaf38)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        c->return_address = 0x5EAF96;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005eb0c5)
{
    if (P_MarketBuilding != eMarketBuilding::WARLOCKS_LAB || c->esi != 35 && c->esi != 37 && c->esi != 41)
    {
        return EXEC_DEFAULT;
    }
    c->return_address = 0x5EB2F0;
    return NO_EXEC_DEFAULT;
}

_LHF_(LoHook_005eb14e)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        c->return_address = 0x5EB168;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005ed7d1)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        LPCSTR dlgHint = EraJS::read(H3String::Format("RMG.objectGeneration.%d.%d.text.dlgHintButtonDeal", eHotaObject::ACTIVE, eHotaObjectActiveType::WARLOCKS_LAB).String());
        c->edi = (int)dlgHint;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_005ed909)
{
    if (P_MarketBuilding == eMarketBuilding::WARLOCKS_LAB)
    {
        c->ecx -= 67;
        *(DWORD*)c->esp = 518;
    }
    return EXEC_DEFAULT;
}

_LHF_(LoHook_WarlocksLab_PatchResExchangeRate)
{
    return P_MarketBuilding != eMarketBuilding::WARLOCKS_LAB;
}






void WarlocksLabExtender::CreatePatches()
{
    if (!m_isInited)
    {
        // H3TradeResourcesDlg::Ctor - 005DF9F0

        // конструктор
        _pi->WriteHiHook(0x5df9f0, SPLICE_, EXTENDED_, THISCALL_, HiHook_005df9f0);
        // ??? патч ширины диалога
        _pi->WriteLoHook(0x5dfa22, LoHook_005dfa22);
        // патч задника диалога
        _pi->WriteLoHook(0x5dfaf7, LoHook_005dfaf7);
        // хз что это
        _pi->WriteLoHook(0x5e13f6, LoHook_WarlocksLab_Smt);
        _pi->WriteLoHook(0x5e1580, LoHook_WarlocksLab_Smt);



        // H3TradeResourcesDlg::Proc - 005EAAE0

        // патч хинта сделки (когда выбраны ресурсы)
        _pi->WriteLoHook(0x5eab8b, LoHook_005eab8b);
        // патч хинта совершенной сделки
        _pi->WriteLoHook(0x5eabcb, LoHook_005eabcb);
        // патч хинта внутри диалога
        _pi->WriteLoHook(0x5eabd3, LoHook_005eabd3);
        // патч имени объекта
        _pi->WriteLoHook(0x5eac0e, LoHook_005eac0e);
        // патч "доступно для торговли"
        _pi->WriteLoHook(0x5eacb2, LoHook_005eacb2);
        // ??? прыжок через кнопки
        _pi->WriteLoHook(0x5eaec0, LoHook_005eaec0);
        _pi->WriteLoHook(0x5eaf38, LoHook_005eaf38);
        // прыжок через флаги
        _pi->WriteLoHook(0x5eb0c5, LoHook_005eb0c5);
        _pi->WriteLoHook(0x5eb14e, LoHook_005eb14e);
        //
        _pi->WriteLoHook(0x5eb228, LoHook_WarlocksLab_PatchResExchangeRate);


        // H3TradeResourcesDlg::ShowHint - 005ED7A0

        // патч хинта кнопки "заключить сделку"
        _pi->WriteLoHook(0x5ed7d1, LoHook_005ed7d1);
        // патч хинта
        _pi->WriteLoHook(0x5ed909, LoHook_005ed909);


        // H3TradeResourcesDlg::WindowHandler - 005ED3E0

        //
        _pi->WriteLoHook(0x5ed59d, LoHook_WarlocksLab_PatchResExchangeRate);
        _pi->WriteLoHook(0x5ed63a, LoHook_WarlocksLab_PatchResExchangeRate);

        m_isInited = true;
    }
}

H3MapItemWarlocksLab* WarlocksLabExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemWarlocksLab*>(reinterpret_cast<const H3MapItemWarlocksLab*>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator* WarlocksLabExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

WarlocksLabExtender& WarlocksLabExtender::Get()
{
    static WarlocksLabExtender _instance;
    return _instance;
}
}
