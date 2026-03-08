#pragma once
namespace hillFort
{

class HillFortExtender : public extender::ObjectExtender
{
  private:
    static H3MapItem *currentHillFort;

  private:
    HillFortExtender();
    virtual ~HillFortExtender();

  protected:
    virtual void CreatePatches() override;
    virtual H3RmgObjectGenerator *CreateRMGObjectGen(const RMGObjectInfo &objectInfo) const noexcept override final;
    virtual BOOL SetAiMapItemWeight(H3MapItem *mapItem, H3Hero *currentHero, const H3Player *activePlayer,
                                    int &aiMapItemWeight, int* moveDistance, const H3Position pos) const noexcept override final;

  private:
    static _LHF_(HillFort_AIMapWeight);

  public:
    static HillFortExtender &Get();
};

}
