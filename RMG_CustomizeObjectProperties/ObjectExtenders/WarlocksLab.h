#pragma once

namespace warlocksLab
{

struct H3MapItemWarlocksLab
{

};

class WarlocksLabExtender : public extender::ObjectExtender
{
    WarlocksLabExtender();

    virtual ~WarlocksLabExtender();

  private:
    virtual void CreatePatches() override;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator* CreateRMGObjectGen(const RMGObjectInfo& objectInfo) const noexcept override final;
    virtual H3MapItemWarlocksLab* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static WarlocksLabExtender&Get();
};

}
