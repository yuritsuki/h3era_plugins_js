#pragma once

namespace townGate
{
struct H3MapItemTownGate
{

};

class TownGateExtender : public extender::ObjectExtender
{
    TownGateExtender();

    virtual ~TownGateExtender();

  private:
    virtual void CreatePatches() override;
    virtual BOOL InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemTownGate* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static TownGateExtender &Get();
};

}
