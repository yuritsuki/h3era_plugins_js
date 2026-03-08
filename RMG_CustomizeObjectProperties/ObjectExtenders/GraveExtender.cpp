#include "../pch.h"
#include "GraveExtender.h"
namespace grave
{
    GraveExtender::GraveExtender()
    : ObjectExtender(globalPatcher->CreateInstance("EraPlugin.GraveExtender.daemon_n"))
{
    objectType = extender::ERA_OBJECT_TYPE;
    objectSubtype = GRAVE_OBJECT_SUBTYPE;
    CreatePatches();
}

    GraveExtender::~GraveExtender()
{
}

BOOL GraveExtender::SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *hero, const H3Player *player,
                                               int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        aiMapItemWeight = static_cast<int>(player->resourceImportance[eResource::GOLD] * 2750.0);
        
        // H3Hero::CountArtifactsInBackpack
        if (THISCALL_2(int, 0x4D9330, hero, 1) < 64)
        {
            aiMapItemWeight += static_cast<int>(player->turnValueOfAvgArtifact);
        }
            
        int movement = hero->movement;
        if (*moveDistance < movement)
        {
            *moveDistance = movement;
        }  
        *moveDistance += hero->CalcMaxMovement();

        return true;
    }

    return false;
}


BOOL H3MapItemGrave::IsVisitedByHero(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    return Era::GetAssocVarIntValue(newMapItem::buffer);
}
void H3MapItemGrave::SetAsVisited(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 1);
}

void H3MapItemGrave::SetAsNotVisited(H3Hero* hero) noexcept
{
    sprintf(newMapItem::buffer, ErmVariableFormat, hero->id);
    Era::SetAssocVarIntValue(newMapItem::buffer, 0);
}


void ShowMessage(const H3MapItem* mapItem)
{
    const bool skipMapMessage = globalPatcher->VarValue<int>("HD.UI.AdvMgr.SkipMapMsgs");

    H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));

    objName.Append(EraJS::read(
        H3String::Format("RMG.objectGeneration.%d.%d.text.visit", mapItem->objectType, mapItem->objectSubtype)
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

void ShowMessageAfterDig(const H3MapItem* mapItem, const int goldAmount, eArtifact artId)
{
    const bool skipMapMessage = globalPatcher->VarValue<int>("HD.UI.AdvMgr.SkipMapMsgs");

    H3String objName = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(mapItem));

    objName.Append(EraJS::read(
        H3String::Format("RMG.objectGeneration.%d.%d.text.visited", mapItem->objectType, mapItem->objectSubtype)
        .String()));

    H3PictureCategories picTwo(ePictureCategories::GOLD, goldAmount);
    if (artId > eArtifact::NONE)
    {
        H3PictureCategories picOne(ePictureCategories::ARTIFACT, artId);
        H3Messagebox::Show(objName, picOne, picTwo);
    }
    else
    {
        H3Messagebox::Show(objName, picTwo);
    }
}

void GraveExtender::GraveDiggingProccess(H3MapItem* mapItem, H3Hero* hero) const noexcept
{
    // get grave data
    H3MapItemGrave* grave = GetFromMapItem(mapItem);
    const int goldAmount = grave->goldAmount * 500;
    eArtifact artId = eArtifact(grave->artifactID);

    // add resource
    THISCALL_3(void, 0x04E3870, hero, eResource::GOLD, goldAmount);

    // check backpack is full
    if (THISCALL_2(int, 0x4D9330, hero, 1) >= 64)
    {
        artId = eArtifact::NONE;
    }

    // Check AI
    BOOL isHuman = THISCALL_1(bool, 0x4BAA60, P_ActivePlayer->Get());

    // add artifact
    if (artId > eArtifact::NONE)
    {
        H3Artifact artifact = H3Artifact(artId, -1);
        THISCALL_4(void, 0x04E32E0, hero, &artifact, 1, 1);

        // if AI - equip
        if (!isHuman)
        {
            // AI::H3Hero::EquipArtifacts
            FASTCALL_1(void, 0x433B40, hero);
        }
    }

    // add debuff
    if (!H3MapItemGrave::IsVisitedByHero(hero))
    {
        H3MapItemGrave::SetAsVisited(hero);
    }
    hero->moraleBonus -= MORALE_PENALTY;

    if (isHuman)
    {
        ShowMessageAfterDig(mapItem, goldAmount, artId);
    }

    // Grave deleting and hole placing proc
    H3Point coordinates = mapItem->GetCoordinates();
    int x = coordinates.x;
    int y = coordinates.y;
    int z = coordinates.z;
    H3Position pos = H3Position(x, y, z);

    // H3AdventureManager::DeleteObject
    THISCALL_4(void, 0x04AA820, P_AdventureManager->Get(), mapItem, pos, 1);

    //Game::Place_Object(o_Game, vX, vY, L, HOLE, -1, -1);
    THISCALL_7(void, 0x04C9550, P_Game->Get(), x, y, z, eObject::HOLE, -1, -1);
}

BOOL GraveExtender::VisitMapItem(H3Hero *hero, H3MapItem *mapItem, const H3Position pos,
                                         const BOOL isHuman) const noexcept
{
    if (GetFromMapItem(mapItem))
    {
        if (isHuman)
        {
            ShowMessage(mapItem);
        }
        else
        {
            hero->movement = 0;
            GraveDiggingProccess(mapItem, hero); // AI instantly digs (cheat)
        }
        return true;
    }

    return false;
}

_LHF_(OnDiggingBeforePuttingAHole)
{
    int x = c->esi;
    int y = c->ebx;
    int z = IntAt(c->ebp+0x10);

    H3MapItem *mapItem = P_AdventureMgr->Get()->GetMapItem(x, y, z);

    if (mapItem->objectType == extender::ERA_OBJECT_TYPE && mapItem->objectSubtype == GRAVE_OBJECT_SUBTYPE)
    {
        H3Hero* hero = *reinterpret_cast<H3Hero**>(c->ebp - 0x10);

        GraveExtender* ext = &GraveExtender::Get();
        ext->GraveDiggingProccess(mapItem, hero);

        c->return_address = 0x040F080;
        return NO_EXEC_DEFAULT;
    }

    return EXEC_DEFAULT;
}

_LHF_(OnCheckPlaceIsValidToDig)
{
    H3MapItem *mapItem = reinterpret_cast<H3MapItem *>(c->ecx);

    if (mapItem->objectType == extender::ERA_OBJECT_TYPE && mapItem->objectSubtype == GRAVE_OBJECT_SUBTYPE)
    {
        c->return_address = 0x4FD40A;
        return NO_EXEC_DEFAULT;
    }
	
    return EXEC_DEFAULT;
}

void GraveExtender::CreatePatches()
{
    if (!m_isInited)
    {
        _pi->WriteLoHook(0x40EE89, OnDiggingBeforePuttingAHole);
        _pi->WriteLoHook(0x4FD3B7, OnCheckPlaceIsValidToDig);

        m_isInited = true;
    }
}

H3MapItemGrave* GraveExtender::GetFromMapItem(H3MapItem *mapItem) const noexcept
{
    if (mapItem && mapItem->objectType == objectType && mapItem->objectSubtype == objectSubtype)
    {
        return const_cast<H3MapItemGrave*>(reinterpret_cast<const H3MapItemGrave*>(&mapItem->setup));
    }

    return nullptr;
}

H3RmgObjectGenerator * GraveExtender::CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
{
    if (objectInfo.type == objectType && objectInfo.subtype == objectSubtype)
    {
        return extender::ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(objectInfo);
    }
    return nullptr;
}

BOOL GraveExtender::InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept
{
    if (auto grave = GetFromMapItem(mapItem))
    {
        *grave = {};
        const int intMax = 8;
        const int intMin = 3;
        int range = intMax - intMin + 1;
        grave->goldAmount = rand() % range + intMin;  // rnd 3-8 included, *500 during diging
        
        //v121 = Random(0, 99);
        const int v121 = rand() % 100;
        //v122 = v121 >= 50 ? 4 : 2; //minor or treasure
        const int artLvl = v121 >= 50 ? 4 : 2;
        //v123 = map_item->SetUp & 0xFF80001F | ((GameMgr_GenRandomArt_ByLevel(o_Game, v122) & 0x3FF) << 13); 13 bit of warrior tomb is artId
        const int artId = THISCALL_2(int, 0x4C9190, P_Game->Get(), artLvl) & 0x3FF; // Restrict the artNum to 1023 (10 bit info)
        grave->artifactID = artId;

        return true;
    }
    return false;
}

GraveExtender& GraveExtender::Get()
{
    static GraveExtender _instance;
    return _instance;
}
}
