#pragma once

namespace ancientLamp
{
constexpr int ANCIENT_LAMP_CREATURE_TYPE = 37;
constexpr int ANCIENT_LAMP_MIN_CREATURE_COUNT = 4;
constexpr int ANCIENT_LAMP_MAX_CREATURE_COUNT = 6;


struct H3MapItemAncientLamp
{
  public:
    unsigned _u1 : 5;
    /** @brief [05] which players have come by*/
    unsigned visited : 8;
    /** @brief [13]*/
    unsigned creatureType : 10;
    /** @brief [23]*/
    unsigned creatureCount : 9;

  public:
    void Reset();
};

class AncientLampExtender : public extender::ObjectExtender
{
    AncientLampExtender();

    virtual ~AncientLampExtender();

  private:
    virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int playerId,
        BOOL isRightClick) const noexcept override final;
    virtual BOOL InitNewGameMapItemSetup(H3MapItem *mapItem) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL AskQuestion(H3MapItem* mapItem) const noexcept;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemAncientLamp* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static AncientLampExtender&Get();
};

}
