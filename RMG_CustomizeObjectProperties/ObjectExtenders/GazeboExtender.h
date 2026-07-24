#pragma once
//#include "../pch.h"

namespace gazebo
{
    // type 144, subtype 6
    constexpr int GAZEBO_OBJECT_SUBTYPE = 6;
    constexpr float EXP_GIVEN = 2000;
    constexpr int GOLD_REQUIRED = 1000;

    struct H3MapItemGazebo
    {
        static int gazeboCounter;

        static constexpr LPCSTR ErmVariableFormat = "gazebo_%d_%d";

      public:
        INT32 id;

      public:
        static inline BOOL IsVisitedByHero(const H3MapItemGazebo *gazebo, const H3Hero *hero) noexcept;
        static inline void SetAsVisited(const H3MapItemGazebo* hermitsShack, const H3Hero* hero) noexcept;
    };

    class GazeboExtender final
        : public H3ActiveObject<H3MapItemGazebo>
    {
    private:
        static GazeboExtender* instance;

        void CreatePatches() override;

        GazeboExtender();

    private:
        BOOL InitNewGameMapItemSetup(H3MapItem* mapItem) const noexcept override final;

        BOOL SetHintInH3TextBuffer(
            H3MapItem* mapItem,
            const H3Hero* hero,
            const int interactPlayerId,
            const BOOL isRightClick) const noexcept override final;

        BOOL VisitMapItem(
            H3Hero* hero,
            H3MapItem* mapItem,
            const H3Position pos,
            const BOOL isHuman) const noexcept override final;

        BOOL SetAiMapItemWeight(
            H3MapItem* mapItem,
            H3Hero* hero,
            const H3Player* player,
            int& aiMapItemWeight,
            int* moveDistance,
            const H3Position pos) const noexcept override final;

        int AI_OnScouting_Value() const noexcept override final;

    public:
        static GazeboExtender& Get();
    };
}
