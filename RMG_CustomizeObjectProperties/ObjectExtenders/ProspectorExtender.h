#pragma once

namespace prospector
{

struct H3MapItemProspector// : H3MapitemMysticGarden
{
    unsigned resourceType : 5;
    /** @brief [05] which players have come by*/
    unsigned visited : 8;
    /** @brief [13]*/
    unsigned resourceAmount : 10;
    unsigned _u1 : 9;

  public:
    void Reset();
};

class ProspectorExtender : public extender::ObjectExtender
{
    ProspectorExtender();

    virtual ~ProspectorExtender();

  private:
    virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int playerId,
        BOOL isRightClick) const noexcept override final;
    virtual BOOL InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept override final;
    virtual BOOL InitNewWeekMapItemSetup(H3MapItem* mapItem) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemProspector* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static ProspectorExtender&Get();
};

}
