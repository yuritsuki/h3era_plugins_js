#pragma once

namespace bogHermit
{
constexpr INT32 BOG_HERMIT_TERRAIN_TYPE = eTerrain::SWAMP;

struct H3MapItemBogHermit
{
    static constexpr LPCSTR ErmVariableFormat = "bog_hermit_%d"; // heroid

  public:
    static BOOL IsVisitedByHero(H3Hero *hero) noexcept;
    static void SetAsVisited(H3Hero* hero) noexcept;
    static void SetAsNotVisited(H3Hero* hero) noexcept;
};

class BogHermitExtender : public extender::ObjectExtender
{
    BogHermitExtender();

    virtual ~BogHermitExtender();

  private:
    virtual void CreatePatches() override;
    virtual BOOL SetHintInH3TextBuffer(H3MapItem *mapItem, const H3Hero *currentHero, int playerId,
                                       BOOL isRightClick) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemBogHermit* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static BogHermitExtender &Get();
};

}
