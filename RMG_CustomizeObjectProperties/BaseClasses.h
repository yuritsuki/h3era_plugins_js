#pragma once
//#include "ObjectExtender.h"


// Рефакторинг и создание иерархии классов


//ObjectExtender
//↓
//H3MapObject<TMapItem>
//// type/subtype, m_isInited,
//// GetFromMapItem, CreateRMGObjectGen
//↓
//H3VisitingObject<TMapItem>
//// сообщения, лупсаунды и звуки посещения
//↓
//H3ActiveObject<TMapItem>
//// активные действия, Visit, AI, new week/map start
//↓
//H3XTraitObject<TMapItem>
//// xtrait и посещение по игрокам/командам (хинты по маскам посещенности команды)
//↓
//ConcreteExtender





// Общая идентификация объекта, получение структуры из H3MapItem 
// и создание RMG-генератора
template<typename TMapItem>
class H3MapObject : public extender::ObjectExtender
{
    static constexpr LPCSTR name_key =
        "RMG.objectGeneration.%d.%d.name";

protected:
    H3MapObject(
        LPCSTR instanceName,
        int objectType,
        int objectSubtype)
        : ObjectExtender(
            globalPatcher->CreateInstance(instanceName))
    {
        this->objectType = objectType;
        this->objectSubtype = objectSubtype;
        this->m_isInited = true;
    }

    ~H3MapObject() override = default;

    //// Деструктор.
    //~H3MapObject()
    //{
    //    // Освобождаем память имени.
    //    if (this->name) MemFree(this->name);
    //}

    // Дефолтный rmc/hover hint с именем объекта
    virtual BOOL SetDefaultHint(H3MapItem* mapItem) const noexcept
    {
        if (!GetFromMapItem(mapItem))
            return false;

        H3String objName = RMGObjectInfo::GetObjectName(mapItem);
        sprintf(h3_TextBuffer, "%s", objName.String());

        //strcpy(
        //    h3_TextBuffer,
        //    RMGObjectInfo::GetObjectName(mapItem).String());

        return true;
    }

    virtual BOOL SetHintInH3TextBuffer(H3MapItem* mapItem, const H3Hero* currentHero, const int interactPlayerId,
        const BOOL isRightClick) const noexcept override
    {
        return SetDefaultHint(mapItem);
    }

    //TMapItem* GetFromMapItem(
    //    H3MapItem* mapItem) const noexcept
    //{
    //    if (!mapItem ||
    //        mapItem->objectType != this->objectType ||
    //        mapItem->objectSubtype != this->objectSubtype)
    //    {
    //        return nullptr;
    //    }

    //    return reinterpret_cast<TMapItem*>(&mapItem->setup);
    //}

    TMapItem* GetFromMapItem(
        H3MapItem* mapItem) const noexcept
    {
        return reinterpret_cast<TMapItem*>(
            GetSetupFromMapItem(mapItem));
    }

    const TMapItem* GetFromMapItem(
        const H3MapItem* mapItem) const noexcept
    {
        return reinterpret_cast<const TMapItem*>(
            GetSetupFromMapItem(mapItem));
    }

    H3RmgObjectGenerator* CreateRMGObjectGen(
        const RMGObjectInfo& objectInfo) const noexcept override
    {
        if (objectInfo.type != this->objectType ||
            objectInfo.subtype != this->objectSubtype)
        {
            return nullptr;
        }

        return extender::ObjectExtenderManager::
            CreateDefaultH3RmgObjectGenerator(objectInfo);
    }

    void CreatePatches() override
    {
        // По умолчанию патчи не нужны.
    }

    //// Установка имени объекта.
    //void Set_Name(const _cstr_ name, _bool_ not_init)
};


// Этот уровень содержит общую работу с сообщениями посещения.
// Поскольку ObjectExtender уже содержит GetStringMessage(), 
// его можно использовать, не дублируя реализацию.
template<typename TMapItem>
class H3VisitingObject : public H3MapObject<TMapItem>
{
protected:
    // Это добавка к стандартному хинту без посещения
    static constexpr LPCSTR objectInfo_key =
        "RMG.objectGeneration.%d.%d.text.info";

    // Это после посещения
    static constexpr LPCSTR extraObjectInfo_key = 
        "RMG.objectGeneration.%d.%d.text.hint";

    static constexpr LPCSTR visit_key =
        "RMG.objectGeneration.%d.%d.text.visit";

    static constexpr LPCSTR visited_key =
        "RMG.objectGeneration.%d.%d.text.visited";

    static constexpr LPCSTR cannotVisit_key =
        "RMG.objectGeneration.%d.%d.text.cannotVisit";

    H3VisitingObject(
        LPCSTR instanceName,
        int objectType,
        int objectSubtype)
        : H3MapObject<TMapItem>(
            instanceName,
            objectType,
            objectSubtype)
    {
    }

    ~H3VisitingObject() override = default;
    //// Деструктор.
    //~_VisitingObject_()
    //{
    //    // Освобождаем память звука посещения.
    //    if (this->visiting_sound) MemFree(this->visiting_sound);

    //    // Освобождаем память текста посещения.
    //    if (this->visiting_message) MemFree(this->visiting_message);
    //}

    // // Установка текста посещения объекта.
    //void Set_VisitingMessage(const _cstr_ visiting_message, _bool_ not_init)

    H3String GetStringMessage(LPCSTR key) const
    {
        H3String message = H3String::Format("{%s}", RMGObjectInfo::GetObjectName(this->objectType, this->objectSubtype));
        message.Append(
            EraJS::read(H3String::Format(key, this->objectType, this->objectSubtype)
                .String()));
        return message;
    }

    H3String GetVisitingMessage() const
    {
        return GetStringMessage(visit_key);
    }

    H3String GetVisitedMessage() const
    {
        return GetStringMessage(visited_key);
    }

    H3String GetCannotVisitMessage() const
    {
        return GetStringMessage(cannotVisit_key);
    }
};


template<typename TMapItem>
class H3ActiveObject : public H3VisitingObject<TMapItem>
{
protected:
    H3ActiveObject(
        LPCSTR instanceName,
        int objectType,
        int objectSubtype)
        : H3VisitingObject<TMapItem>(
            instanceName,
            objectType,
            objectSubtype)
    {
    }

    ~H3ActiveObject() override = default;

    virtual BOOL AI_MapGoal_Value(H3MapItem* mapItem, H3Hero* currentHero, const H3Player* activePlayer,
        int& aiResWeight, int* moveDistance, const H3Position pos) const noexcept
    {
        return false;
    }

    virtual BOOL AI_Scouting_Value(H3MapItem* mapItem, H3Hero* currentHero, const H3Player* activePlayer,
        int& aiResWeight, int* moveDistance, const H3Position pos) const noexcept
    {
        return false;
    }
};


// Этот уровень отвечает за:
// - дополнительный текст xtrait;
// - битовую маску посетивших игроков;
// - сохранение и очистку этой маски;
// - добавление дополнительной подсказки;
// - установку посещения для всей команды.
//template<typename TMapItem>
//class H3XTraitObject : public H3ActiveObject<TMapItem>
//{
//protected:
//    H3XTraitObject(
//        LPCSTR instanceName,
//        int objectType,
//        int objectSubtype)
//        : H3ActiveObject<TMapItem>(
//            instanceName, 
//            objectType, 
//            objectSubtype)
//    {
//    }
//
//    ~H3XTraitObject() override = default;
//
//    //// Деструктор.
//    //virtual ~H3XTraitObject()
//    //{
//    //    // Подсказка о свойствах объекта.
//    //    if (this->xtrait) MemFree(this->xtrait);
//    //}
//
//    //....
//
//    //  // Получение текста по ПКМ. GetRightClickHint
//    //virtual void RightClickText
//
//    //// Получение текста по mouse hover. GetMouseHoverHint
//    //virtual void CommandText
//
//    //// Посещение объекта.
//    //virtual void Visit(
//    //    H3XTraitObject* this,
//    //    H3Hero* hero,
//    //    H3MapItem* cell,
//    //    H3Position pos,
//    //    int playerId,
//    //    char notAI)
//    //{
//    //    // Делаем объект посещённым для игрока и его союзников.
//    //    if (playerId >= 0 && playerId < 8)
//    //    {
//    //        H3Game* game = P_Game->Get();
//    //        const int teamId = game->GetPlayerTeam(playerId);
//
//    //        for (int i = 0; i < 8; ++i)
//    //        {
//    //            // Игрок из команды посетившего объект.
//    //            if (game->mapInfo.playerTeam[i] == teamId)
//    //            {
//    //                // Объект посещён игроком.
//    //                this->playersVisitedObject |= 1u << i;
//    //            }   
//    //        }
//    //    }
//    //}
//
//
//
//
//    //// Установка подсказки о свойствах объекта.
//    //void Set_XTrait(const _cstr_ str, _bool_ not_init = TRUE)
//
//
//};





//template<typename TMapItem>
//struct H3ActiveObject1 : public H3VisitingObject<TMapItem>
//{
//    H3ActiveObject(
//        LPCSTR instanceName,
//        int objectType,
//        int objectSubtype)
//        : H3VisitingObject<TMapItem>(
//            instanceName,
//            objectType,
//            objectSubtype)
//    {
//    }
//
//    virtual ~H3ActiveObject() override = default;
//
//    // Настройка при инициализации.
//    virtual void InitSetup(TMemoryStream& data, std::vector<_cstr_>& texts)
//    {
//
//    }
//
//    // Инициализация сохранения глобальных данных карты в сохранение.
//    virtual void GlobalMapData_InitSaving()
//    {
//        // По-умолчанию нет глобальных данных.
//        // AddSaveInfo(addr, size, default_val);
//    }
//
//    // Очистка глобальных данных подтипа объекта для карты.
//    virtual void GlobalMapData_Clear()
//    {
//        // По-умолчанию нет глобальных данных.
//    }
//
//    //// Получение текста по ПКМ.
//    //virtual void RightClickText(char* str,
//    //    H3Hero* hero,
//    //    H3MapItem* cell,
//    //    H3Position pos,
//    //    int playerId)
//    //{
//    //    // По-умолчанию - просто имя.
//    //    strcpy(str, this->name);
//    //}
//
//    //// Получение текста подсказки.
//    //virtual void CommandText(char* str,
//    //    H3Hero* hero,
//    //    H3MapItem* cell,
//    //    H3Position pos,
//    //    int playerId)
//    //{
//    //    // По-умолчанию - просто имя.
//    //    strcpy(str, this->name);
//    //}
//
//      // Инициализация при старте карты.
//    virtual void MapStart(H3MapItem* cell)
//    {
//
//    }
//
//    // Обработка новой недели.
//    virtual void NewWeek(H3MapItem* cell)
//    {
//
//    }
//
//    //// Посещение объекта.
//    //virtual void Visit(
//    //    H3Hero* hero,
//    //    H3MapItem* cell,
//    //    H3Position pos,
//    //    int playerId,
//    //    bool notAI)
//    //{
//    //    // По-умолчанию показываем сообщение при посещении.
//    //    if (notAI)
//    //    {
//    //        // -777 - сигнал для HD для сворачивания сообщений.
//    //        FASTCALL_12(void, 0x4F6C00, this->Get_VisitingMessage(), 1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -777);
//    //    }
//    //}
//
//    //// Ценность объекта для ИИ.
//    //virtual int H3ActiveObject::AI_Value(
//    //    H3ActiveObject* this,
//    //    H3Hero* hero,
//    //    H3MapItem* cell,
//    //    H3Position pos,
//    //    H3Player* player,
//    //    int* distance)
//    //{
//    //    return 0;
//    //}
//
//    //virtual int Unknown10(/* пока неизвестные аргументы */)
//    //{
//    //    return 0;
//    //}
//
//    //virtual int Unknown11(/* пока неизвестные аргументы */)
//    //{
//    //    return 0;
//    //}
//
//
//
//
//
//
//
//
//
//    //LPCSTR name = nullptr;
//    //LPCSTR visitingMessage = nullptr;
//
//    //static constexpr LPCSTR name_key = "RMG.objectGeneration.%d.%d.name";
//
//    //LPCSTR GetString(LPCSTR key)
//    //{
//    //    bool readSuccess = false;
//    //    sprintf(h3_TextBuffer, key, this->objectType, this->objectSubtype);
//    //    LPCSTR tempStr = EraJS::read(h3_TextBuffer, readSuccess);
//    //    if (!readSuccess)
//    //    {
//    //        sprintf(h3_TextBuffer,
//    //            "Couldnot find a string for object type %d, subtype %d !",
//    //            this->objectType, this->objectSubtype);
//    //        LPCSTR tempStr = h3_TextBuffer;
//    //        H3Messagebox::Show(tempStr);
//    //    }
//    //    return tempStr;
//    //}
//
//    //LPCSTR GetName()
//    //{
//    //    return GetString(name_key);
//    //}
//
//    //LPCSTR GetVisitingMessage()
//    //{
//    //    return GetString(visit_key);
//    //}
//};
