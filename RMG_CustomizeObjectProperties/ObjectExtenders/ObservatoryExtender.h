#pragma once

namespace observatory
{
constexpr int VISION_RADIUS_GIVEN = 2;

struct H3MapItemObservatory
{
    static constexpr LPCSTR ErmVariableFormat = "observatory_%d"; //heroid

  public:
    static BOOL IsVisitedByHero(H3Hero* hero) noexcept;
    static void SetAsVisited(H3Hero* hero) noexcept;
    static void SetAsNotVisited(H3Hero* hero) noexcept;
};

class ObservatoryExtender : public extender::ObjectExtender
{
    ObservatoryExtender();

    virtual ~ObservatoryExtender();

  private:
    virtual void CreatePatches() override;
    virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int playerId,
        BOOL isRightClick) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemObservatory* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static ObservatoryExtender &Get();
};

}
