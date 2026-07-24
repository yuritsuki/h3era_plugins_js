#include "../pch.h"
#include "ColosseumOfTheMagiExtender.h"

namespace colosseumOfTheMagi
{
    int H3MapItemColosseumOfTheMagi::colosseumOfTheMagiCounter = 0;

    BOOL H3MapItemColosseumOfTheMagi::IsVisitedByHero(const H3MapItemColosseumOfTheMagi* colosseumOfTheMagi, const H3Hero* hero) noexcept
    {
        sprintf(h3_TextBuffer, ErmVariableFormat, colosseumOfTheMagi->id, hero->id);
        return Era::GetAssocVarIntValue(h3_TextBuffer);
    }
    void H3MapItemColosseumOfTheMagi::SetAsVisited(const H3MapItemColosseumOfTheMagi* colosseumOfTheMagi, const H3Hero* hero) noexcept
    {
        sprintf(h3_TextBuffer, ErmVariableFormat, colosseumOfTheMagi->id, hero->id);
        Era::SetAssocVarIntValue(h3_TextBuffer, 1);
    }

    _LHF_(Game__NewGameBeforeSetObjectsInitialParameters)
    {
        H3MapItemColosseumOfTheMagi::colosseumOfTheMagiCounter = 0;

        return EXEC_DEFAULT;
    }

    void ColosseumOfTheMagiExtender::CreatePatches()
    {
        static bool hookInstalled = false;
        if (hookInstalled) return;

        _PI->WriteLoHook(0x04BFCBE, Game__NewGameBeforeSetObjectsInitialParameters);
        hookInstalled = true;
    }

    ColosseumOfTheMagiExtender::ColosseumOfTheMagiExtender()
        : H3ActiveObject<H3MapItemColosseumOfTheMagi>(
            "EraPlugin.ColosseumOfTheMagiExtender.daemon_n",
            extender::HOTA_OBJECT_TYPE,
            COLOSSEUM_OF_THE_MAGI_OBJECT_SUBTYPE)
    {
        CreatePatches();
    }

    ColosseumOfTheMagiExtender* ColosseumOfTheMagiExtender::instance = nullptr;
    ColosseumOfTheMagiExtender& ColosseumOfTheMagiExtender::Get()
    {
        if (!instance) instance = new ColosseumOfTheMagiExtender();
        return *instance;
    }

    BOOL ColosseumOfTheMagiExtender::InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept
    {
        if (auto colosseumOfTheMagi = GetFromMapItem(mapItem))
        {
            colosseumOfTheMagi->id = H3MapItemColosseumOfTheMagi::colosseumOfTheMagiCounter++;
            return true;
        }

        return false;
    }

    BOOL ColosseumOfTheMagiExtender::SetHintInH3TextBuffer(
        H3MapItem* mapItem, const H3Hero* currentHero, 
        const int interactPlayerId, const BOOL isRightClick) const noexcept
    {
        if (auto colosseumOfTheMagi = GetFromMapItem(mapItem))
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
            if (currentHero)
            {
                const bool isVisitedByHero = H3MapItemColosseumOfTheMagi::IsVisitedByHero(colosseumOfTheMagi, currentHero);
                AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
            }

            sprintf(h3_TextBuffer, "%s", objName.String());

            return true;
        }

        return false;
    }

    BOOL ColosseumOfTheMagiExtender::VisitMapItem(
        H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
        const BOOL isHuman) const noexcept
    {
        if (auto colosseumOfTheMagi = GetFromMapItem(mapItem))
        {
            const bool isVisitedByHero = H3MapItemColosseumOfTheMagi::IsVisitedByHero(colosseumOfTheMagi, hero);
            if (isVisitedByHero)
            {
                if (isHuman)
                {
                    FASTCALL_12(void, 0x4F6C00,
                        this->GetVisitedMessage().String(),
                        1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -777);
                }
            }
            else
            {
                ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);
                INT8* selectedPrimarySkill = nullptr;

                if (isHuman)
                {
                    H3AdventureManager* advMgr = P_AdventureManager->Get();
                    // H3AdventureManager::SetTimer
                    THISCALL_3(void, 0x415CC0, advMgr, 0, -1);
                    // H3AdventureManager::UpdateInfoPanel
                    THISCALL_4(void, 0x415D40, advMgr, 0, 1, 1);

                    FASTCALL_12(void, 0x4F6C00,
                        this->GetVisitingMessage().String(),
                        10, -1, -1, 33, 2, 34, 2, -1, 0, -1, 0);

                    switch (P_WindowManager->resultItemID)
                    {
                    case eControlId::SELECT_RIGHT:
                        selectedPrimarySkill = &hero->primarySkill[2];
                        break;

                    case eControlId::SELECT_LEFT:
                        selectedPrimarySkill = &hero->primarySkill[3];
                        break;

                    default:
                        return true;
                    }
                }
                else
                {
                    const signed char spellPower =
                        THISCALL_2(signed char, 0x5BE240, hero, 2); // H3Hero::GetHeroPrimary

                    const signed char knowledge =
                        THISCALL_2(signed char, 0x5BE240, hero, 3); // H3Hero::GetHeroPrimary

                    selectedPrimarySkill =
                        spellPower <= knowledge
                        ? &hero->primarySkill[2]
                        : &hero->primarySkill[3];
                }

                // Увеличиваем значение скилла
                *selectedPrimarySkill += 2;

                // Prima patch
                ClampPrimarySkill(selectedPrimarySkill);
                
                H3MapItemColosseumOfTheMagi::SetAsVisited(colosseumOfTheMagi, hero);
            }

            return true;
        }

        return false;
    }

    BOOL ColosseumOfTheMagiExtender::SetAiMapItemWeight(
        H3MapItem* mapItem, H3Hero* hero, const H3Player* activePlayer,
        int& aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
    {
        if (auto colosseumOfTheMagi = GetFromMapItem(mapItem))
        {
            const bool isVisitedByHero = H3MapItemColosseumOfTheMagi::IsVisitedByHero(colosseumOfTheMagi, hero);

            if (!isVisitedByHero)
            {
                int needExpoToNextLvl = h3functions::NeedExpoToNextLevel(hero->level);
                float moveDist = (float)(2 * needExpoToNextLvl);
                aiMapItemWeight = static_cast<int>(moveDist * hero->AI_experienceEffectiveness);
            }

            return true;
        }

        return false;
    }

    int ColosseumOfTheMagiExtender::AI_OnScouting_Value() const noexcept
    {
        return 100;
    }
}
