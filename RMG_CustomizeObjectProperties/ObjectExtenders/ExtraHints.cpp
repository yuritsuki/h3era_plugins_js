// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "../pch.h"


LPCSTR teamsObjectVisited = "object_%d_%d_visitedByTeams"; // eObject, objSubtype

CHAR GetObjectFlagsVisitedByTeam(INT32 objId, INT32 objSubtype)
{
    sprintf(h3_TextBuffer, teamsObjectVisited, objId, objSubtype);
    return Era::GetAssocVarIntValue(h3_TextBuffer);
}

void SetObjectFlagsVisitedByTeam(INT32 objId, INT32 objSubtype, CHAR flags)
{
    sprintf(h3_TextBuffer, teamsObjectVisited, objId, objSubtype);
    Era::SetAssocVarIntValue(h3_TextBuffer, flags);
}

INT8 GetTeamBitOffset(int teamId)
{
	return 1 << teamId;
}

BOOL IsObjectVisitedByTeam(INT32 objId, INT32 objSubtype, int teamId)
{
    CHAR flags = GetObjectFlagsVisitedByTeam(objId, objSubtype);
    return flags & GetTeamBitOffset(teamId);
}

CHAR SetObjectFlags(H3Game* game, int teamId, CHAR teamFlags)
{
    if (game->mapInfo.playerTeam[0] == teamId)
    {
        teamFlags |= 1;
    }
    if (game->mapInfo.playerTeam[1] == teamId)
    {
        teamFlags |= 2;
    }
    if (game->mapInfo.playerTeam[2] == teamId)
    {
        teamFlags |= 4;
    }
    if (game->mapInfo.playerTeam[3] == teamId)
    {
        teamFlags |= 8;
    }
    if (game->mapInfo.playerTeam[4] == teamId)
    {
        teamFlags |= 0x10;
    }
    if (game->mapInfo.playerTeam[5] == teamId)
    {
        teamFlags |= 0x20;
    }
    if (game->mapInfo.playerTeam[6] == teamId)
    {
        teamFlags |= 0x40;
    }
    if (game->mapInfo.playerTeam[7] == teamId)
    {
        teamFlags |= 0x80;
    }
    return teamFlags;
}



BOOL ShowObjectHint(LoHook* h, HookContext* c, BOOL isRightClick)
{
    H3MapItem* cell = reinterpret_cast<H3MapItem*>(c->ebx);
    H3Hero* hero = *reinterpret_cast<H3Hero**>(c->ebp - 0x10);
    int playerId = P_CurrentPlayerID;
    int teamId = THISCALL_2(int, 0x4A55D0, P_Game->Get(), playerId);

    eObject type = static_cast<eObject>(cell->objectType);


    if (type == eObject::STABLES)
    {
        if (P_MePlayerID > 7 || !IsObjectVisitedByTeam(eObject::STABLES, 0, teamId))
        {
            return EXEC_DEFAULT;
        }
        LPCSTR objName = P_ObjectName[eObject::STABLES];
        LPCSTR extraInfo = EraJS::read(H3String::Format(extender::ObjectExtender::extraObjectInfo_key, eObject::STABLES, 0).String());
        extraInfo = H3String::Format(extraInfo, StablesBonus).String();

        if ((*(WORD*)&cell->mirror & 0x1000) != 0 && hero)
        {
            sprintf_s(h3_TextBuffer, 0x300u, isRightClick ? "%s\n%s\n\n%s" : "%s %s %s", objName, extraInfo, P_GeneralText->GetText((hero->flags & 2) ? 354 : 355));
        }
        else
        {
            sprintf_s(h3_TextBuffer, 0x300u, isRightClick ? "%s\n%s" : "%s %s", objName, extraInfo);
        }

        c->return_address = isRightClick ? 0x4159C0 : 0x40D09B;
        return NO_EXEC_DEFAULT;
    }
    // For RMC-hint there is a GEM erm code
    else if (type == eObject::REFUGEE_CAMP && !isRightClick)
    {
        if (P_MePlayerID > 7 || !IsObjectVisitedByTeam(eObject::REFUGEE_CAMP, cell->objectSubtype, teamId))
        {
            return EXEC_DEFAULT;
        }
        LPCSTR objName = P_ObjectName[eObject::REFUGEE_CAMP];
        LPCSTR extraInfo = EraJS::read(H3String::Format(extender::ObjectExtender::extraObjectInfo_key, eObject::REFUGEE_CAMP, -1).String());
        sprintf_s(h3_TextBuffer, 0x300u, "%s %s", objName,
            H3String::Format(extraInfo, P_CreatureInformation[cell->objectSubtype].namePlural, cell->setup).String());
        c->edi = (int)h3_TextBuffer;

        c->return_address = 0x40D059;
        return NO_EXEC_DEFAULT;
    }

    return EXEC_DEFAULT;
}

void ProcObjectFlagsVisitedByTeam(H3Hero* hero, INT32 objType, INT32 objSubtype)
{
    H3Game* game = P_Game->Get();
    int teamId = game->GetPlayerTeam(hero->owner);

    CHAR teamFlags = GetObjectFlagsVisitedByTeam(objType, objSubtype);
    teamFlags = SetObjectFlags(game, teamId, teamFlags);
    SetObjectFlagsVisitedByTeam(objType, objSubtype, teamFlags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


_LHF_(LoHook_AdvMgr_GetObjectRightClickText)
{
    return ShowObjectHint(h, c, true);
}

_LHF_(LoHook_AdvMgr_GetObjectCommand)
{
    return ShowObjectHint(h, c, false);
}



_LHF_(LoHook_004A5D18)
{
    H3Hero* hero = reinterpret_cast<H3Hero*>(c->esi);
    ProcObjectFlagsVisitedByTeam(hero, eObject::STABLES, 0);

    return EXEC_DEFAULT;
}

void __stdcall HiHook_004A4270(HiHook* h, H3AdventureManager* advMgr, H3Hero* hero, H3MapItem* cell, int isHuman)
{
    THISCALL_4(void, h->GetDefaultFunc(), advMgr, hero, cell, isHuman);
    ProcObjectFlagsVisitedByTeam(hero, eObject::REFUGEE_CAMP, cell->objectSubtype);

}



void ExtraHints()
{
    // RMC hints
    _PI->WriteLoHook(0x41391C, LoHook_AdvMgr_GetObjectRightClickText);
    // mouseover hints
    _PI->WriteLoHook(0x40B158, LoHook_AdvMgr_GetObjectCommand);


    // Stables - player flags
    _PI->WriteLoHook(0x4A5D18, LoHook_004A5D18);

    // Refugee camp - player flags
    _PI->WriteHiHook(0x4A4270, SPLICE_, EXTENDED_, THISCALL_, HiHook_004A4270);
}
