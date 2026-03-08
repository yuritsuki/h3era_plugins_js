#pragma once

namespace templeOfLoyalty
{

struct H3MapItemTempleOfLoyalty
{
    static constexpr LPCSTR ErmVariableFormat = "templeOfLoyalty_%d";

  public:
    static BOOL IsVisitedByHero(H3Hero* hero) noexcept;
    static void SetAsVisited(H3Hero* hero) noexcept;
    static void SetAsNotVisited(H3Hero* hero) noexcept;
};

class TempleOfLoyaltyExtender : public extender::ObjectExtender
{
    TempleOfLoyaltyExtender();

    virtual ~TempleOfLoyaltyExtender();

  private:
    virtual void CreatePatches() override;

    virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int playerId,
        BOOL isRightClick) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemTempleOfLoyalty* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static TempleOfLoyaltyExtender &Get();
};

}
