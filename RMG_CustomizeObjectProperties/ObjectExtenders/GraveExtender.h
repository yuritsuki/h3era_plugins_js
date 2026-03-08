#pragma once

namespace grave
{
constexpr int GRAVE_OBJECT_SUBTYPE = 1;
constexpr int MORALE_PENALTY = 3;

struct H3MapItemGrave
{
  static constexpr LPCSTR ErmVariableFormat = "grave_%d";

  public:
      /** @brief [00] 0..1023 artifact ID*/
      unsigned  artifactID : 10;
      /** @brief [10] 0..1023 goldAmount*/
      unsigned goldAmount : 10;
      /** @brief [20]*/
      unsigned _u1 : 12;

  public:
    static BOOL IsVisitedByHero(H3Hero* hero) noexcept;
    static void SetAsVisited(H3Hero* hero) noexcept;
    static void SetAsNotVisited(H3Hero* hero) noexcept;
};

class GraveExtender : public extender::ObjectExtender
{
    GraveExtender();

    virtual ~GraveExtender();

  private:
    virtual void CreatePatches() override;
    //virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int playerId,
    //    BOOL isRightClick) const noexcept override final;
    virtual BOOL InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept override final;
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemGrave* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    virtual void GraveDiggingProccess(H3MapItem* mapItem, H3Hero* hero) const noexcept;
    static GraveExtender&Get();
};

}
