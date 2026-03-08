#pragma once
#include "RMGObjectsEditor.h"
namespace extender
{
constexpr int ERA_OBJECT_TYPE = eObject::BLANK3;
constexpr int WAREHOUSE_OBJECT_TYPE = eObject::BLANK4;
constexpr int HOTA_OBJECT_TYPE = eObject::BLANK5;
constexpr int HOTA_PICKUPABLE_OBJECT_TYPE = eObject::BLANK6;
constexpr int HOTA_UNREACHABLE_OBJECT_TYPE = eObject::BLANK7;

// namespace limits
struct RMGObjectProperties
{
    BOOL enabled = TRUE;
    INT32 mapLimit = 0;
    INT32 zoneLimit = 0;
    INT32 value = 0;
    INT32 density = 0;
};
class ObjectExtender
{
  protected:
    int objectType = eObject::NO_OBJ;
    int objectSubtype = eObject::NO_OBJ;
    BOOL m_isInited = FALSE;
    PatcherInstance *_pi = nullptr;

  public:
    static constexpr LPCSTR extraObjectInfo_key = "RMG.objectGeneration.%d.%d.text.hint";
    static constexpr LPCSTR visit_key = "RMG.objectGeneration.%d.%d.text.visit";
    static constexpr LPCSTR visited_key = "RMG.objectGeneration.%d.%d.text.visited";
    static constexpr LPCSTR cannotVisit_key = "RMG.objectGeneration.%d.%d.text.cannotVisit";

  public:
    ObjectExtender(PatcherInstance *_pi) : _pi(_pi)
    {
    }

    ObjectExtender() = default;
    virtual ~ObjectExtender()
    {
    }

  protected:
    virtual void CreatePatches() {};
    // virtual void GetObjectPreperties() noexcept = 0;


  public:
    // required override for some complex structures like creature banks
    virtual void AfterLoadingObjectsTxtProc(const INT16 *maxSubtypes)
    {
    }
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept
    {
        //   return ObjectExtenderManager::CreateDefaultH3RmgObjectGenerator(info);
        H3RmgObjectGenerator *objGen = nullptr;
        if (objGen = H3ObjectAllocator<H3RmgObjectGenerator>().allocate(1))
        {
            THISCALL_5(H3RmgObjectGenerator *, 0x534640, objGen, objectInfo.type, objectInfo.subtype, objectInfo.value,
                       objectInfo.density);
        }
        return objGen;
    }

    virtual BOOL InitNewGameMapItemSetup(H3MapItem *mapItem) const noexcept
    {
        return false;
    }
    virtual BOOL InitNewWeekMapItemSetup(H3MapItem *mapItem) const noexcept
    {
        return false;
    }
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept
    {
        return false;
    }
    virtual BOOL SetHintInH3TextBuffer(H3MapItem *mapItem, const H3Hero *currentHero, const int interactPlayerId,
                                       const BOOL isRightClick) const noexcept
    {
        return false;
    }
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiResWeight, int *moveDistance, const H3Position pos) const noexcept
    {
        return false;
    }

    virtual BOOL RMGDlg_ShowCustomObjectHint(const RMGObjectInfo &info, const H3ObjectAttributes *attributes,
                                             const H3String &defaultText) noexcept
    {
        return false;
    }

    void AddExtraInfoHint(H3String* objName, BOOL isRightClick) const noexcept
    {
        LPCSTR extraInfo = EraJS::read(H3String::Format(extraObjectInfo_key, objectType, objectSubtype).String());

        sprintf(h3_TextBuffer, "%s%s",
            isRightClick ? "\n" : " ",
            extraInfo);
        objName->Append(h3_TextBuffer);
    }

    void AddHeroVisitedHint(H3String* objName, BOOL isRightClick, BOOL isVisitedByHero) const noexcept
    {
        sprintf(h3_TextBuffer, "%s%s",
            isRightClick ? "\n\n" : " ",
            P_GeneralText->GetText(isVisitedByHero ? 354 : 355));
        objName->Append(h3_TextBuffer);
    }

  public:
    int GetObjectType() const noexcept
    {
        return objectType;
    }
    int GetObjectSubtype() const noexcept
    {
        return objectSubtype;
    }

    BOOL Register() noexcept
    {
        return RegisterExtender(this) != NULL;
    }
    //	virtual int AiMapItemWeightFunction(HookContext* c, const H3MapItem* mapItem, H3Player* player);
    // returns if object was visited by some of derived classes
    // virtual BOOL HeroMapItemVisitFunction(HookContext* c, const H3Hero* hero, const H3MapItem* mapItem, const BOOL
    // isPlayer, const BOOL skipMapMessage);

  public:
    static ObjectExtender *CreateExtenderInstance(int objectType, PatcherInstance *pi) noexcept
    {

        return nullptr;
    }

  public:
    static inline int RegisterExtender(ObjectExtender *extender) noexcept
    {

        constexpr const char *RMGPluginName = "RMG_CustomizeObjectProperties.era";
        typedef int(__stdcall * RegisterObjectExtender_t)(ObjectExtender *);
        HMODULE pl = LoadLibraryA(RMGPluginName);
        if (pl)
        {
            RegisterObjectExtender_t f = RegisterObjectExtender_t(GetProcAddress(pl, "_RegisterObjectExtender@4"));
            if (f)
                return f(extender);
        }
        return NULL;
    }
};

} // namespace extender
