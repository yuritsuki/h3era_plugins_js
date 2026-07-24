#include "../pch.h"
#include "GazeboExtender.h"

namespace gazebo
{
    int H3MapItemGazebo::gazeboCounter = 0;

    BOOL H3MapItemGazebo::IsVisitedByHero(const H3MapItemGazebo* gazebo, const H3Hero* hero) noexcept
    {
        sprintf(h3_TextBuffer, ErmVariableFormat, gazebo->id, hero->id);
        return Era::GetAssocVarIntValue(h3_TextBuffer);
    }
    void H3MapItemGazebo::SetAsVisited(const H3MapItemGazebo* gazebo, const H3Hero* hero) noexcept
    {
        sprintf(h3_TextBuffer, ErmVariableFormat, gazebo->id, hero->id);
        Era::SetAssocVarIntValue(h3_TextBuffer, 1);
    }

    _LHF_(Game__NewGameBeforeSetObjectsInitialParameters)
    {
        H3MapItemGazebo::gazeboCounter = 0;

        return EXEC_DEFAULT;
    }

    void GazeboExtender::CreatePatches()
    {
        static bool hookInstalled = false;
        if (hookInstalled) return;

        _PI->WriteLoHook(0x04BFCBE, Game__NewGameBeforeSetObjectsInitialParameters);
        hookInstalled = true;
    }

    GazeboExtender::GazeboExtender()
        : H3ActiveObject<H3MapItemGazebo>(
            "EraPlugin.GazeboExtender.daemon_n",
            extender::HOTA_OBJECT_TYPE,
            GAZEBO_OBJECT_SUBTYPE)
    {
        CreatePatches();
    }

    GazeboExtender* GazeboExtender::instance = nullptr;
    GazeboExtender& GazeboExtender::Get()
    {
        if (!instance) instance = new GazeboExtender();
        return *instance;
    }

    BOOL GazeboExtender::InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept
    {
        if (auto gazebo = GetFromMapItem(mapItem))
        {
            gazebo->id = H3MapItemGazebo::gazeboCounter++;
            return true;
        }

        return false;
    }

    BOOL GazeboExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int interactPlayerId,
        const BOOL isRightClick) const noexcept
    {
        if (const auto gazebo = GetFromMapItem(mapItem))
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
                const bool isVisitedByHero = H3MapItemGazebo::IsVisitedByHero(gazebo, hero);
                AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
            }

            sprintf(h3_TextBuffer, "%s", objName.String());

            return true;
        }

        return false;
    }

    BOOL GazeboExtender::VisitMapItem(
        H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
        const BOOL isHuman) const noexcept
    {
        if (auto gazebo = GetFromMapItem(mapItem))
        {
            const bool isVisitedByHero = H3MapItemGazebo::IsVisitedByHero(gazebo, hero);
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
                const int playerGoldBeforeVisit = P_ActivePlayer->playerResources.gold;
                
                if (playerGoldBeforeVisit < GOLD_REQUIRED)
                {
                    if (isHuman)
                    {
                        FASTCALL_12(void, 0x4F6C00,
                            this->GetCannotVisitMessage().String(),
                            1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -777);
                    }
                }
                else if (isHuman)
                {
                    // Calculate exp given considering the learning power
                    float heroLearningPower = h3functions::GetHeroLearningPower(hero);
                    const int expGiven = static_cast<int>(EXP_GIVEN * heroLearningPower);

                    // Задаем вопрос
                    FASTCALL_12(void, 0x4F6C00,
                        this->GetVisitingMessage().String(),
                        2, -1, -1, ePictureCategories::EXPERIENCE, expGiven, ePictureCategories::GOLD, -GOLD_REQUIRED - 100000, -1, 0, -1, 0);

                    if (P_WindowManager->resultItemID != eControlId::CANCEL)
                    {
                        THISCALL_3(void, 0x04E3870, hero, eResource::GOLD, -GOLD_REQUIRED);
                        THISCALL_4(void, 0x04E3620, hero, expGiven, 1, 1);
                        H3MapItemGazebo::SetAsVisited(gazebo, hero);
                        // H3AdventureMgrDlg::RedrawHeroesSlots
                        THISCALL_4(void, 0x4032E0, P_AdventureManager->dlg, -1, 1, 1);
                    }
                }
                // AI::H3Hero::CalculateMapPosWeight_
                else if (THISCALL_2(int, 0x52C1F0, hero, pos) > 0)
                {
                    // Calculate exp given considering the learning power
                    float heroLearningPower = h3functions::GetHeroLearningPower(hero);
                    const int expGiven = static_cast<int>(EXP_GIVEN * heroLearningPower);
                    THISCALL_3(void, 0x04E3870, hero, eResource::GOLD, -GOLD_REQUIRED);
                    THISCALL_4(void, 0x04E3620, hero, expGiven, 1, 1);
                    H3MapItemGazebo::SetAsVisited(gazebo, hero);
                }
            }

            return true;
        }

        return false;
    }

    BOOL GazeboExtender::SetAiMapItemWeight(
        H3MapItem* mapItem, H3Hero* hero, const H3Player* player, int& aiMapItemWeight,
        int* moveDistance, const H3Position pos) const noexcept
    {

        if (auto gazebo = GetFromMapItem(mapItem))
        {
            const bool isVisitedByHero = H3MapItemGazebo::IsVisitedByHero(gazebo, hero);

            if (!isVisitedByHero)
            {
                if (P_ActivePlayer->playerResources.gold >= GOLD_REQUIRED)
                {
                    // pseudocode addr 0052BB89 (TreeofKnowledge) and 00529833 (LearningStone)
                    //*(_QWORD*)&v8 = (__int64)(hp->turnExperienceToRVRatio * 1000.0);
                    const float aiExperience = EXP_GIVEN * hero->AI_experienceEffectiveness;

                    // return (__int64)((double)v6 - HeroOwner->ai.resource_value[6] * 2000.0);
                    aiMapItemWeight =
                        static_cast<int>(aiExperience - player->aIPlayer.resourceImportance[eResource::GOLD] * GOLD_REQUIRED);
                }
            }

            return true;
        }

        return false;
    }

    int GazeboExtender::AI_OnScouting_Value() const noexcept
    {
        return 50;
    }
}
