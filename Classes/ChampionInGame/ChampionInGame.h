#pragma once

#include "../GameObject/GameObject.h"
#include "../ZyUwU/ZyUwU.h"
#include "../Champion/Champion.h"
#include "../Support/Coordinate.h"
#include "Extension.h"

USING_NS_ALL;

enum class HeadDir
{
    FRONT = 1,
    BEHIND = -1
};

enum class MoveType
{
    MOVE_TO,
    MOVE_BY
};

class Player;
class Arena;
class GameEffect;
class Dice;
class ChampionUI;
class SkillManager;
BEGIN_CREATE_REFCLASS(ChampionInGame, Champion)

public:
    enum class ChampionAction
    {
        IDLE,
        MOVING,
        ATTACKING,
        CASTING_SKILL
    };

    enum class ChampionStatus
    {
        NORMAL,
        DYING,
        DEATH
    };

public:
    static ChampionInGame* createWithChampion(Champion *pChamp, bool bIsClone = true, bool bIsDeleteCloner = false);
    static ChampionInGame* createWithProperties(Champion *pChamp, ChampionUI *pUI, Dice* pDice, SkillManager* vSkillDeck);

public:
    struct sortChampion
    {
        inline bool operator() (ChampionInGame* l, ChampionInGame* r)
        {
            return l->getStatics()->getStatics()->getSpeed() < r->getStatics()->getStatics()->getSpeed();
        }
    };

public:
    virtual void update(float dt);
    virtual void setPosition(Coordinate& coord);
    virtual void setPosition(Arena* pArena);
    virtual void setPosition(cocos2d::Vec2 pos);
    virtual Point getPosition();
    virtual void config();

public:
    void setOwner(Player *pOwner, bool bIsRepresent);

    CREATE_GET_FUNC(getOwner, Player*, m_pOwner);

    CREATE_SET_GET_FUNC(setDice, getDice, Dice*, m_pDice);
    CREATE_SET_FUNC(setPreDiceSkillDeck, SkillManager*, m_pPreDiceSkillDeck);
    CREATE_SET_FUNC(setPostDiceSkillDeck, SkillManager*, m_pPostDiceSkillDeck);
    CREATE_SET_FUNC(setUI, ChampionUI*, m_pChampionUI);

    CREATE_GET_FUNC(getCoordinate, Coordinate, m_cCoordinate);
    CREATE_SET_GET_FUNC(setStatics, getStatics, IngameStatics*, m_pIngameStatics);
    CREATE_GET_FUNC(isTurn, bool, m_bIsTurn);
    CREATE_GET_FUNC(getLandingArena, Arena* , m_pLandingArena);
    CREATE_GET_FUNC(isNotMoving, bool, m_eAction != ChampionAction::MOVING);
    CREATE_GET_FUNC(isCastingSkill, bool, m_eAction == ChampionAction::CASTING_SKILL);
    CREATE_GET_FUNC(getIconSize, Size, m_pIcon->getContentSize());
    bool isValidTurn();

public:
    bool initWithProperties(ChampionUI *pUI, Dice* pDice, SkillManager* vSkillDeck);


//    void move(Coordinate &coord, MoveType eMoveType);
    void autoFlip();

    //// IN UPDATE LOOP
    // Life checker
    void lifeCheck();
    void onDeath();
    void onDying();
    void respawn();

    void setLandingArena(Arena *pArena);
    void reloadDataFromMemory(int nRound);
    void addChampChild(ChampionInGame *pChild);
    void setParent(ChampionInGame *pParent);
    std::string getChildsString(int nTab);
    std::string toStringHelper(int nTab = 2, bool bIsShowParent = true);

    void onLand(bool attack = true);
    void endLand();

    void applyEffectToSelf(std::vector<GameEffect*> vEffects);

    //// Attack
    void attack(std::vector<ChampionInGame*> vChampions);
    void beAttacked(ChampionInGame* attacker);

    /// Self-Button
    bool onTouch(cocos2d::Touch *touch, cocos2d::Event *event);
    bool endTouch(cocos2d::Touch *touch, cocos2d::Event *event);
    void run(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);

    void jumpTo(int num);
    void jumpTo(Point pos);
    void jumpTo(Coordinate coord);
    void jumpTo(Arena* arena);

    void startTurn();
    void endTurn();

    void castingSkill();

    void disable();
    void enable();

public:
    void enableDice();

protected:
    /// Container
    std::vector<ChampionInGame*> m_vChilds;
    std::vector<ChampionAction> m_vActionMemory;

    /// Must declare
    IngameStatics* m_pIngameStatics;
    Dice* m_pDice;
    Player *m_pOwner;
    ChampionUI* m_pChampionUI;
    SkillManager* m_pPreDiceSkillDeck, *m_pPostDiceSkillDeck;

    /// Auto declare, has first init value
    Coordinate m_cCoordinate;
    ChampionAction m_eAction;
    ChampionStatus m_eStatus;
    HeadDir m_eHead;
    Arena *m_pLandingArena, *m_pMemArena;
    ui::Button *m_pSelfButton;

    /// Auto declare, can be nullptr
    ChampionInGame* m_pParent;

    ///
    bool m_bIsRepresentPlayer;
    bool m_bIsTurn, m_bIsEndTurn;
    bool m_bIsAction;

private:


END_CREATE_REFCLASS