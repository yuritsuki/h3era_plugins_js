#pragma once

namespace mineralSpring
{
constexpr int MOVE_POINTS_GIVEN = 600;
constexpr int LUCK_GIVEN = 1;

struct H3MapItemMineralSpring
{
    static constexpr LPCSTR ErmVariableFormat = "mineralSpring_%d";

  public:
    static BOOL IsVisitedByHero(const H3Hero *hero) noexcept;
    static void SetAsVisited(const H3Hero* hero) noexcept;
    static void SetAsNotVisited(const H3Hero* hero) noexcept;
};

class MineralSpringExtender : public extender::ObjectExtender
{

    MineralSpringExtender();

    virtual ~MineralSpringExtender();

  private:
    virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int playerId,
          BOOL isRightClick) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemMineralSpring* GetFromMapItem(H3MapItem* mapItem) const noexcept;


  public:
    static MineralSpringExtender &Get();
};

}
