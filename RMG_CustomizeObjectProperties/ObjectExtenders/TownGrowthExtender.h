#pragma once
#include "../BaseClasses.h"

namespace townGrowth
{
    // type 144, subtype 16;
    constexpr int TOWN_GROWTH_OBJECT_SUBTYPE = 16;

    struct H3MapItemTownGrowth
    {
        static constexpr LPCSTR PlayerVisitedVariableFormat = "RMG.TownGrowthExtender.PlayerVisited.%d";
    };

    class TownGrowthExtender final
        : public H3ActiveObject<H3MapItemTownGrowth>
    {
    private:
        static TownGrowthExtender* instance;

        TownGrowthExtender();

        BOOL VisitMapItem(
            H3Hero* hero,
            H3MapItem* mapItem,
            H3Position pos,
            BOOL isHuman) const noexcept override final;

    public:
        static TownGrowthExtender& Get();
    };
}
