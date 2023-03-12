#include "Player.h"
#include "Arena/Arena.h"
#include "Support/GameConstant.h"
#include "Arena/Property/Property.h"
#include "GameMaster/GameMaster.h"

//// Constructor

Player::Player() :
m_pEconomy(nullptr),
m_pSelectingObject(nullptr),
m_eType(SelectType::NONE),
m_Color(Color4F::RED),
m_eAction(PlayerAction::IDLE)
{

}

//// Virtual

bool Player::init()
{
    if(!Layer::init()) return false;

    // Init Event Listener
    //m_pEventListener = EventListenerTouchOneByOne::create();
    //m_pEventListener->onTouchBegan = CC_CALLBACK_2(Player::onTouch, this);
    //m_pEventListener->onTouchEnded = CC_CALLBACK_2(Player::endTouch, this);

    m_pEconomy = IgEcoMng::create();
    m_pEconomy->setAmount(5000);
    this->addChild(m_pEconomy);
    m_pEconomy->setPosition(Point(ZYDR_GVS/6));
    this->scheduleUpdate();

    return true;
}

std::string Player::toString(int nTab)
{
    std::string ts;
    return ts;
}

void Player::log()
{

}

void Player::update(float dt)
{
}

//// Public

bool Player::onTouch(Touch *touch, Event *event)
{
    return true;
}
bool Player::endTouch(Touch *touch, Event *event)
{
    return true;
}

void Player::setControlChampion(ChampionInGame* champ)
{
    this->m_pControllingChampion = champ;
}

void Player::setChampionViewPoint(ChampionInGame *pChampion)
{

}

bool Player::yesOrNoSelector(std::string sMessage)
{
    return true;
}

void Player::onLandArena(Arena* arena)
{
    auto pro = dynamic_cast<Property*>(arena);
    if(pro)
    {
        if(!pro->hasOwner())
        {
            if(m_pEconomy->isPayable(pro->getPrice()))
            {
                std::string s = "WOULD U LIKE TO PURCHASE THIS PROPERTY\nFOR " + ZYSP_SD(pro->getPrice(), 1) + "K";
                auto font = ZYLabel::createWithTTF(s, globalFont, 50);
                font->setTag(77);
                font->setColor(Color3B::BLUE);
                this->addChild(font);
                font->setPosition(Point(ZYDR_GVS.width/2, ZYDR_GVS.height/3*2));
                font->setHorizontalAlignment(TextHAlignment::CENTER);

                auto button = ui::Button::create("button/n.png", "button/p.png");
                button->setTitleText("YES");
                this->addChild(button);
                button->setTag(88);
                button->setPosition(Point(ZYDR_GVS.width/4, ZYDR_GVS.height/4));

                auto no = ui::Button::create("button/n.png", "button/p.png");
                no->setTitleText("NO");
                no->setPosition(Point(ZYDR_GVS.width/4*3, ZYDR_GVS.height/4));
                this->addChild(no);
                no->setTag(99);

                button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type)
                                              {
                                                  if (type == ui::Widget::TouchEventType::ENDED)
                                                  {
                                                      auto prop = dynamic_cast<Property*>(this->m_pControllingChampion->getLandingArena());
                                                      if(prop)
                                                      {
                                                          this->m_pEconomy->pay(prop->getPrice());
                                                          this->m_vOwn.push_back(prop);
                                                          prop->setOwner(this);
                                                          prop->addDrawRectOrder();
                                                          prop->setRectColor(m_Color);
                                                      }

                                                      auto b = this->getChildByTag(88);
                                                      auto n = this->getChildByTag(99);
                                                      auto f = this->getChildByTag(77);
                                                      b->removeFromParentAndCleanup(true);
                                                      n->removeFromParentAndCleanup(true);
                                                      f->removeFromParentAndCleanup(true);
                                                      this->finishAction();
                                                  }
                                              });
                no->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type)
                                          {
                                              if (type == ui::Widget::TouchEventType::ENDED)
                                              {
                                                  auto b = this->getChildByTag(88);
                                                  auto n = this->getChildByTag(99);
                                                  auto f = this->getChildByTag(77);
                                                  b->removeFromParentAndCleanup(true);
                                                  n->removeFromParentAndCleanup(true);
                                                  f->removeFromParentAndCleanup(true);
                                                  this->finishAction();
                                              }
                                          });
            }
        }
        else
        {
            if(this->pay(pro->getOwner(), pro->getTax()))
            {
                this->finishAction();
            }
            else this->lose();
        }
        //if(pro->hasOwner()) this->pay(pro->getOwner(), pro->getTax());
        return;
    }
    this->finishAction();
}

void Player::addChampion(ChampionInGame* pChamp)
{
    if(pChamp)
    {
        m_vChampions.push_back(pChamp);
        m_vChampions.back()->setOwner(this, true);
        this->addChild(pChamp);
    }
}

void Player::finishAction()
{
    //m_pControllingChampion->endTurn();
    if(!m_pControllingChampion->isTurn())
    {
        this->disable();
    }
    GM_GI->calculateNewTurn();
}

void Player::disable()
{
    ///> disable economy node
    this->m_pEconomy->disable();

    ///> disable controlling champion
    if(m_pControllingChampion)
    {
        this->m_pControllingChampion->disable();
        //this->m_pControllingChampion = nullptr;
    }
}

void Player::enable()
{
    this->m_pEconomy->enable();
}

void Player::lose()
{

}

void Player::startTurn(ChampionInGame* child)
{
    this->setControlChampion(child);
    this->setChampionViewPoint(child);
}

void Player::endTurn()
{
}

bool Player::pay(Player* target, float money)
{
    if(!m_pEconomy->pay(target->m_pEconomy, money)) return false;
    return true;
}

void Player::receiveMoney(float money)
{
    this->m_pEconomy->receive(money);
}
