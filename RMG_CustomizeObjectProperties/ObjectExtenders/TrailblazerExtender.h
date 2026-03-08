#pragma once

namespace trailblazer
{
constexpr INT32 TRAILBLAZER_TERRAIN_TYPE = eTerrain::ROUGH;

struct H3MapItemTrailblazer
{
    static constexpr LPCSTR ErmVariableFormat = "trailblazer_%d"; // heroid

  public:
    static BOOL IsVisitedByHero(H3Hero *hero) noexcept;
    static void SetAsVisited(H3Hero* hero) noexcept;
    static void SetAsNotVisited(H3Hero* hero) noexcept;
};

class TrailblazerExtender : public extender::ObjectExtender
{
    TrailblazerExtender();

    virtual ~TrailblazerExtender();

  private:
    virtual void CreatePatches() override;
    virtual BOOL SetHintInH3TextBuffer(H3MapItem *mapItem, const H3Hero *currentHero, int playerId,
                                       BOOL isRightClick) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemTrailblazer* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static TrailblazerExtender &Get();
};

}
