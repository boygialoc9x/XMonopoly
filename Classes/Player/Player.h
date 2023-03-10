#pragma once

#include "ZyUwU/ZyUwU.h"
#include "ui/CocosGUI.h"

#include "ChampionInGame/ChampionInGame.h"
//#include "GameObject/Economy/Economy.h"
#include "GameObject/Economy/Manager/EconomyManager.h"


USING_NS_ALL;

class Property;
class SpecialArena;
BEGIN_CREATE_REFCLASS(Player, Layer)

public:
    enum class SelectType
    {
        NONE = 0,
        CHAMPION,
        ARENA,
        BOTH
    };

    enum class TheAction
    {
        IDLE = 0,
        CONTROLLING
    };

public:
    static Player* createClientPlayer();
    static Player* createBot();
    static Player* createPlayer();

public:
    virtual void update(float dt);

public:
    bool onTouch(Touch *touch, Event *event);
    bool endTouch(Touch *touch, Event *event);

public:
    void setChampionViewPoint(ChampionInGame *pChampion);
    void setControlChampion(ChampionInGame* champ);

    void onLandSpecialArena(SpecialArena* special);
    CREATE_SET_GET_FUNC(setSelectType, getSelectType, SelectType, m_eType);
    CREATE_SET_GET_FUNC(setSelectObject, getSelectObject, GameObject*, m_pSelectingObject)
    CREATE_SET_GET_FUNC(setTheColor, getTheColor, Color4F, m_Color);
    CREATE_GET_FUNC(getEconomy, IgEcoMng*, m_pEconomy);

    float getTotalPropertyValue();

    CREATE_GET_FUNC(getNetWorth, float, (m_pEconomy->getAmount() + getTotalPropertyValue()));

    bool doPay(Player* target, float money);
    void receiveMoney(float money);

    void disable();
    void enable();
    void finishAction();

    void startTurn(ChampionInGame* child);
    void endTurn();

    CREATE_GET_FUNC(getChampChildren, std::vector<ChampionInGame*>, m_vChampions);

    void addChampion(ChampionInGame* pChamp);
    void addOwnedProperty(Property* property);
    void lose();

//// PURCHASE PROPERTY /////////////////////////////////////////
    void onLandArena(Arena* arena);
    void onLandProperty(Property* property);

    void purchaseProperty(Property* property);
    void acquireProperty(Property* property);

    void cancelPurchase(Property* property);
    void confirmPurchase(Property* property);
    void pay(Player* target, float money);


protected:
    void showPurchasePromptHelper(const std::string& message, const ui::Widget::ccWidgetTouchCallback& yesCallBack, const ui::Widget::ccWidgetTouchCallback& noCallback);
    void showMessageHelper(const std::string& message);
    void showMessageHelper(const std::string& message, const float duration);

    ui::Button* createPurchaseButton(const std::string& title, int tag, const Point& pos);
    void onPurchaseButtonPressed(Ref* pSender, ui::Widget::TouchEventType type, bool bIsYes, Property* target);
    void removeAllMarkedChild();
/////////////////////////////////////////////////////////////////

protected:
    /// Must declare
    std::vector<ChampionInGame*> m_vChampions;           ///< array of children champions
    std::vector<Property*> m_vOwn;                       ///< weak reference to parent champion

    /// Container
    ChampionInGame* m_pControllingChampion;              ///< weak reference to the is-turn champion
    ChampionInGame* m_pViewPointChampion;                ///< weak reference to the viewpoint champion
    std::vector<int> m_vRemoveByTagList;                 ///< contain child's tag that will be removed after calling removeAllMarkedChild() func

    EventListenerTouchOneByOne *m_pEventListener;        ///< event listener
    GameObject* m_pSelectingObject;                      ///< weak reference to selecting object
    SelectType m_eType;                                  ///< mark the selecting object's type
    TheAction m_eAction;                                 ///< current action of the player

    Color4F m_Color;                                  ///< player's color

    IgEcoMng* m_pEconomy;
    int m_nChangeCount;


END_CREATE_REFCLASS