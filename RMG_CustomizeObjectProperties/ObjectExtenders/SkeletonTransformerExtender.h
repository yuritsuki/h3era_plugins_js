#pragma once

namespace skeletonTransformer
{

struct H3MapItemSkeletonTransformer
{

};

class SkeletonTransformerExtender : public extender::ObjectExtender
{
    SkeletonTransformerExtender();

    virtual ~SkeletonTransformerExtender();

  private:
    virtual BOOL VisitMapItem(H3Hero *currentHero, H3MapItem *mapItem, const H3Position pos,
                              const BOOL isHuman) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual H3MapItemSkeletonTransformer* GetFromMapItem(H3MapItem* mapItem) const noexcept;

  public:
    static SkeletonTransformerExtender &Get();
};

}
