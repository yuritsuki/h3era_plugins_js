#pragma once


namespace colosseumOfTheMagi
{
    // type 144, subtype 2
    constexpr int COLOSSEUM_OF_THE_MAGI_OBJECT_SUBTYPE = 2;

    struct H3MapItemColosseumOfTheMagi
    {
        static int colosseumOfTheMagiCounter;

        static constexpr LPCSTR ErmVariableFormat = "colosseumOfTheMagi_%d_%d";

      public:
        INT32 id;

        static BOOL IsVisitedByHero(const H3MapItemColosseumOfTheMagi *colosseumOfTheMagi, const H3Hero *hero) noexcept;
        static void SetAsVisited(const H3MapItemColosseumOfTheMagi* colosseumOfTheMagi, const H3Hero* hero) noexcept;
    };

    class ColosseumOfTheMagiExtender final
        : public H3ActiveObject<H3MapItemColosseumOfTheMagi>
    {
    private:
        static ColosseumOfTheMagiExtender* instance;

        void CreatePatches() override;

        ColosseumOfTheMagiExtender();

        BOOL InitNewGameMapItemSetup(
            H3MapItem* mapItem) const noexcept override final;

        BOOL SetHintInH3TextBuffer(
            H3MapItem* mapItem,
            const H3Hero* currentHero,
            int interactPlayerId,
            BOOL isRightClick) const noexcept override final;

        BOOL VisitMapItem(
            H3Hero* hero,
            H3MapItem* mapItem,
            H3Position pos,
            BOOL isHuman) const noexcept override final;

        BOOL SetAiMapItemWeight(
            H3MapItem* mapItem,
            H3Hero* hero,
            const H3Player* activePlayer,
            int& aiMapItemWeight,
            int* moveDistance,
            H3Position pos) const noexcept override final;

        int AI_OnScouting_Value() const noexcept override final;

    public:
        static ColosseumOfTheMagiExtender& Get();
    };
}
