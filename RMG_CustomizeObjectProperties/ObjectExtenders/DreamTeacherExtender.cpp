#include "../pch.h"
#include "DreamTeacherExtender.h"

namespace dreamTeacher
{
int H3MapItemDreamTeacher::dreamTeacherCounter = 0;

DreamTeacherExtender::DreamTeacherExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.DreamTeacherExtender.daemon_n"))
{
    objectType = extender::ERA_OBJECT_TYPE;
    objectSubtype = DREAM_TEACHER_OBJECT_SUBTYPE;
    CreatePatches();
}

DreamTeacherExtender::~DreamTeacherExtender()
{
}

BOOL DreamTeacherExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                              int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    if (auto dreamTeacher = GetFromMapItem(mapItem))
    {
        const bool isVisitedByHero = H3MapItemDreamTeacher::IsVisitedByHero(dreamTeacher, hero);

        if (!isVisitedByHero)
        {
            // get visited obj counter
            int visitedObjCount = H3MapItemDreamTeacher::GetVisitedObjectsCountByHero(hero);

            // pseudocode addr 0052BB89 (TreeofKnowledge) and 00529833 (LearningStone)
            
            // calculate exp given considering the learning power
            float heroLearningPower = h3functions::GetHeroLearningPower(hero);
            int expGiven = static_cast<int>((EXP_GIVEN + (ADDITIONAL_EXP_GIVEN_FOR_EACH_VISIT * visitedObjCount)) * heroLearningPower);
            
            // pseudocode addr 0052BB89
            //*(_QWORD *)&v8 = (__int64)(hero->turnExperienceToRVRatio * 1000.0);
            aiMapItemWeight = static_cast<int>(expGiven * hero->AI_experienceEffectiveness);
        }

        return true;
    }

    return false;
}

BOOL H3MapItemDreamTeacher::IsVisitedByHero(const H3MapItemDreamTeacher * dreamTeacher, const H3Hero *hero) noexcept
{
    sprintf(h3_TextBuffer, ErmVariableFormat, dreamTeacher->id, hero->id);
    return Era::GetAssocVarIntValue(h3_TextBuffer);
}

int H3MapItemDreamTeacher::GetVisitedObjectsCountByHero(const H3Hero* hero) noexcept
{
    sprintf(h3_TextBuffer, VisitedObjectsCount, hero->id);
    return Era::GetAssocVarIntValue(h3_TextBuffer);
}

void ShowMessage(const H3MapItem *mapItem, int expGiven, const bool isVisitedByHero)
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
            FASTCALL_12(void, 0x4F6C00, objName.String(),
                1, -1, -1,
                -1, 0,
                -1, 0,
                -1, 0,
                -1, -777);
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
        H3PictureCategories pics(ePictureCategories::EXPERIENCE, expGiven);

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

BOOL DreamTeacherExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                        const BOOL isHuman) const noexcept
{

    if (auto dreamTeacher = GetFromMapItem(mapItem))
    {
        ProcObjectFlagsVisitedByTeam(hero, objectType, objectSubtype);

        const bool isVisitedByHero = H3MapItemDreamTeacher::IsVisitedByHero(dreamTeacher, hero);

        if (isVisitedByHero)
        {
            if (isHuman)
            {
                ShowMessage(mapItem, 0, isVisitedByHero);
            }
        }
        else
        {
            // get visited obj counter
            int visitedObjCount = H3MapItemDreamTeacher::GetVisitedObjectsCountByHero(hero);
            
            // calculate exp given considering the learning power
            float heroLearningPower = h3functions::GetHeroLearningPower(hero);
            int expGiven = static_cast<int>((EXP_GIVEN + (ADDITIONAL_EXP_GIVEN_FOR_EACH_VISIT * visitedObjCount)) * heroLearningPower);

            if (isHuman)
            {
                ShowMessage(mapItem, expGiven, isVisitedByHero);
            }
            
            // give exp
            THISCALL_4(void, 0x04E3620, hero, expGiven, 1, 1);

            // Hero::CheckLevelUps
            THISCALL_1(void, 0x04DA990, hero);

            // set obj as visited
            sprintf(h3_TextBuffer, H3MapItemDreamTeacher::ErmVariableFormat, dreamTeacher->id,
                hero->id);
            Era::SetAssocVarIntValue(h3_TextBuffer, 1);

            // increase visited obj counter
            visitedObjCount += 1;
            sprintf(h3_TextBuffer, H3MapItemDreamTeacher::VisitedObjectsCount, hero->id);
            Era::SetAssocVarIntValue(h3_TextBuffer, visitedObjCount);
        }

        return true;
    }

    return false;
}

BOOL DreamTeacherExtender::InitNewGameMapItemSetup(H3MapItem *mapItem) const noexcept
{

    if (auto dreamTeacher = GetFromMapItem(mapItem))
    {
        dreamTeacher->id = H3MapItemDreamTeacher::dreamTeacherCounter++;
        return true;
    }

    return false;
}

_LHF_(Game__NewGameBeforeSetObjectsInitialParameters)
{
    H3MapItemDreamTeacher::dreamTeacherCounter = 0;

    return EXEC_DEFAULT;
}

BOOL DreamTeacherExtender::SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* hero, const int playerId, BOOL isRightClick) const noexcept
{

    if (auto dreamTeacher = GetFromMapItem(mapItem))
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
            const bool isVisitedByHero = H3MapItemDreamTeacher::IsVisitedByHero(dreamTeacher, hero);
            AddHeroVisitedHint(&objName, isRightClick, isVisitedByHero);
        }

        sprintf(h3_TextBuffer, "%s", objName.String());

        return true;
    }

    return false;
}

void DreamTeacherExtender::CreatePatches()
{
    if (!m_isInited)
    {
        // Era::RegisterHandler
        // 004BFCBE
        _pi->WriteLoHook(0x04BFCBE, Game__NewGameBeforeSetObjectsInitialParameters);

        m_isInited = true;
    }
}

H3MapItemDreamTeacher* DreamTeacherExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemDreamTeacher *>(reinterpret_cast<const H3MapItemDreamTeacher*>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator * DreamTeacherExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

DreamTeacherExtender& DreamTeacherExtender::Get()
{
    static DreamTeacherExtender _instance;
    return _instance;
}
}
