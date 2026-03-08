#include "../pch.h"
#include "HermitsShackExtender.h"

namespace hermitsShack
{
int H3MapItemHermitsShack::hermitsShackCounter = 0;

HermitsShackExtender::HermitsShackExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.HermitsShackExtender.daemon_n"))
{
    objectType = eHotaObject::ACTIVE;
    objectSubtype = eHotaObjectActiveType::HERMITS_SHACK;
    CreatePatches();
}

HermitsShackExtender::~HermitsShackExtender()
{
}

BOOL HermitsShackExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                              int &aiMapItemWeight, int *moveDistance,
                                              const H3Position pos) const noexcept
{
    if (auto hermitsShack = GetFromMapItem(mapItem))
    {
        const bool isVisitedByHero = H3MapItemHermitsShack::IsVisitedByHero(hermitsShack, hero);

        if (!isVisitedByHero)
        {
            // Pattern used: return AIHero_GetObjectPosWeight_WitchHut(hero, mapItem); 0052BDE0

            const int heroOwner = hero->owner;
            bool heroHasBasicOrAdvancedSkill = false;
            int i = 0;
            while (i < 28 && !heroHasBasicOrAdvancedSkill)
            {
                if (hero->secSkill[i] < 3 && hero->secSkill[i] > 0)
                {
                    heroHasBasicOrAdvancedSkill = true;
                    break;
                }
                i++;
            }
            if (!heroHasBasicOrAdvancedSkill)
            {
                return false;
            }

            aiMapItemWeight = static_cast<int>(
                static_cast<float>(hero->ExpNeedToNextLevel(hero->level)) * hero->AI_experienceEffectiveness);

            return true;
        }
    }

    return false;
}


BOOL H3MapItemHermitsShack::IsVisitedByHero(const H3MapItemHermitsShack *hermitsShack, const H3Hero *hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hermitsShack->id, hero->id);
    return Era::GetAssocVarIntValue(newMapItem::buffer);
}
void H3MapItemHermitsShack::SetAsVisited(const H3MapItemHermitsShack* hermitsShack, const H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, H3MapItemHermitsShack::ErmVariableFormat, hermitsShack->id, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 1);
}


void ShowMessage(const H3MapItem *mapItem, const int skillId, const int skillLvl, const bool isVisitedByHero)
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
    else if (skillId == -1)
    {
        objName.Append(EraJS::read(
            H3String::Format("RMG.objectGeneration.%d.%d.text.cannotVisit", mapItem->objectType, mapItem->objectSubtype)
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
        int skillPic = skillId * 3 + skillLvl + 3; // В оригинале +2, но нужен скилл на лвл выше
        H3PictureCategories pics(ePictureCategories::SECONDARY_SKILL, skillPic);

        if (skipMapMessage)
        {
            FASTCALL_12(void, 0x4F6C00, objName.String(),
                1, -1, -1,
                pics.type, pics.subtype,
                -1, 0,
                -1, 0,
                -1, -777);
        }
        else
        {
            H3Messagebox::Show(objName, pics);
        }
    }
}

BOOL HermitsShackExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                        const BOOL isHuman) const noexcept
{

    if (auto hermitsShack = GetFromMapItem(mapItem))
    {
        ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);

        const bool isVisitedByHero = H3MapItemHermitsShack::IsVisitedByHero(hermitsShack, hero);

        if (isVisitedByHero)
        {
            if (isHuman)
            {
                ShowMessage(mapItem, -1, -1, isVisitedByHero);
            }
            return true;
        }

        // Check basic or advanced secondary skills
        std::vector<int> skillIds;

        int i = 0;
        while (i < 28)
        {
            if (hero->secSkill[i] < 3 && hero->secSkill[i] > 0)
            {
                skillIds.push_back(i);
            }
            i++;
        }
        //// Ломается интеграция с вог-артами и прочими модами
        //if (hero->secSkillCount >= 8 && !heroHasBasicOrAdvancedSkill)
        //{

        //}

        int size = skillIds.size();

        // Нет скиллов для апа
        if (!size)
        {
            if (isHuman)
            {
                ShowMessage(mapItem, -1, -1, isVisitedByHero);
            }
        }
        // Есть скиллы
        else
        {
            int random = H3Random::Rand(1, size);
            int skillId = skillIds[random - 1];
            int skillLvl = int(hero->secSkill[skillId]);
            hero->LearnSecondarySkill(skillId, 1);
            H3MapItemHermitsShack::SetAsVisited(hermitsShack, hero);
            if (isHuman)
            {
                ShowMessage(mapItem, skillId, skillLvl, isVisitedByHero);
            }
        }

        return true;
    }

    return false;
}

BOOL HermitsShackExtender::InitNewGameMapItemSetup(H3MapItem *mapItem) const noexcept
{

    if (auto hermitsShack = GetFromMapItem(mapItem))
    {
        hermitsShack->id = H3MapItemHermitsShack::hermitsShackCounter++;
        return true;
    }

    return false;
}

_LHF_(Game__NewGameBeforeSetObjectsInitialParameters)
{
    H3MapItemHermitsShack::hermitsShackCounter = 0;

    return EXEC_DEFAULT;
}

BOOL HermitsShackExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int playerId, BOOL isRightClick) const noexcept
{

    if (auto hermitsShack = GetFromMapItem(mapItem))
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
            const bool isVisitedByHero = H3MapItemHermitsShack::IsVisitedByHero(hermitsShack, hero);
            AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

void HermitsShackExtender::CreatePatches()
{
    if (!m_isInited)
    {
        // Era::RegisterHandler
        // 004BFCBE
        _pi->WriteLoHook(0x04BFCBE, Game__NewGameBeforeSetObjectsInitialParameters);

        m_isInited = true;
    }
}

H3MapItemHermitsShack* HermitsShackExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemHermitsShack *>(reinterpret_cast<const H3MapItemHermitsShack *>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator *HermitsShackExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

HermitsShackExtender &HermitsShackExtender::Get()
{
    static HermitsShackExtender _instance;
    return _instance;
}
}
