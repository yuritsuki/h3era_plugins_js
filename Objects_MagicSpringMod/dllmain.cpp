#define _H3API_PLUGINS_
#include "framework.h"

Patcher* globalPatcher = nullptr;
PatcherInstance* _PI = nullptr;

namespace dllText
{
    LPCSTR instanceName = "EraPlugin.Objects_MagicSpringMod.daemon_n";
}

/**
 * Логика Минерального источника:
 * 1. Посещение возможно, если текущая мана < максимальной.
 * 2. При посещении дает +100 маны, но не выше максимума.
 * 3. Расходуется (флаг 0x40) только при успешном получении маны.
 */

_LHF_(MagicSpringHook)
{
    // edi = hero* a2 (в начале 0x4A32D8)
    h3::H3Hero* hero = (h3::H3Hero*)c->edi;
    // a3 = NewmapCell* (ebp+0Ch)
    h3::H3MapItem* cell = c->Arg<h3::H3MapItem*>(2);
    // a4 = bool showDialog (ebp+10h)
    bool showDialog = c->Arg<BOOL32>(3) != 0;

    // Рассчитываем максимальную ману героя
    int knowledge = hero->primarySkill[3];
    if (knowledge <= 0) knowledge = 1;

    // H3Hero::GetIntelligencePower(void)const = 0x4E4B20
    // Возвращает 1.0 (нет навыка), 1.5 (продв.), 2.0 (эксперт)
    float manaModifier = hero->GetIntelligencePower();
    int maxMana = (int)(manaModifier * (float)(10 * knowledge));
    
    int currentMana = hero->spellPoints;

    if (currentMana < maxMana)
    {
        int newMana = currentMana + 100;
        if (newMana > maxMana) newMana = maxMana;
        
        int gainedMana = newMana - currentMana;
        hero->spellPoints = (INT16)newMana;

        if (showDialog)
        {
            // Используем FASTCALL_12 для вывода сообщения с иконкой маны (35)
            h3::H3String msg;
            msg.Printf(Era::tr("objects.magic_spring.success"), gainedMana);
            FASTCALL_12(void, 0x4F6C00, msg.String(), 1, -1, -1, 35, gainedMana, -1, 0, -1, 0, -1, -777);
        }
        
        // Сбрасываем флаг (делаем источник недоступным до конца недели)
        cell->setup &= ~0x40;
    }
    else
    {
        if (showDialog)
        {
            // Сообщение если мана и так полна
            LPCSTR msg = Era::tr("objects.magic_spring.full");
            FASTCALL_12(void, 0x4F6C00, msg, 1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -777);
        }
    }

    // Возвращаемся в конец оригинального блока, где обновляется интерфейс
    c->return_address = 0x4A3385;
    return NO_EXEC_DEFAULT;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    static bool initialized = false;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!initialized)
        {
            initialized = true;
            globalPatcher = GetPatcher();
            _PI = globalPatcher->CreateInstance(dllText::instanceName);
            Era::ConnectEra(hModule, dllText::instanceName);
            
            // Ставим хук на адрес 0x4A32D8 (начало расчета параметров для проверки маны)
            _PI->WriteLoHook(0x4A32D8, MagicSpringHook);
        }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
