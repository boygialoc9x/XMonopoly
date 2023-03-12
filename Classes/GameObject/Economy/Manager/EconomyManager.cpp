#include "EconomyManager.h"
#include "Support/GameConstant.h"

///] Constructor

IngameEconomyManager::IngameEconomyManager() :
m_economy(Economy::IngameCoin),
m_pLabel(nullptr)
{

}

///] Static

IngameEconomyManager* IngameEconomyManager::createIngameEconomy()
{
    auto ret = new (std::nothrow) IngameEconomyManager();
    if(ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

///] Virtual

bool IngameEconomyManager::init()
{
    auto config = TTFConfig(defaultTTFConfig);
    config.bold = true;
    config.fontSize = 30;

    this->m_pLabel = ZYLabel::createWithTTF(config, "", TextHAlignment::CENTER);

    if(!m_pLabel) return false;

    this->addChild(m_pLabel);
    m_economy = Economy::IngameCoin;

    this->scheduleUpdate();
    return true;
}

void IngameEconomyManager::log()
{
    ZY_EZ_LOG;
}

std::string IngameEconomyManager::toString(int nTab)
{
    std::string ts;
    return ts;
}

void IngameEconomyManager::update(float dt)
{
    this->m_pLabel->setString("$: " + ZYSP_SD(m_economy.getAmount(), 1) + "K");
}

///] Public

void IngameEconomyManager::setPosition(const float xx, const float yy)
{
    this->m_pLabel->setPosition(xx, yy);
}

void IngameEconomyManager::setPosition(Point pos)
{
    this->m_pLabel->setPosition(pos);
}

void IngameEconomyManager::executeIndicator(float money, bool isPay)
{
    auto color = Color3B::GREEN;
    std::string text = "$" + ZYSP_SD(money, 1) + "K";
    std::string pre = "+";
    if(isPay)
    {
        color = Color3B::RED;
        pre = "-";
    }
    auto font = ZYLabel::createWithTTF(pre + text, globalFont, 20);
    font->setColor(color);
    this->addChild(font);
    font->setPosition(m_pLabel->getPosition());
    auto size = m_pLabel->getContentSize();

    auto hDir = autoSelectMoveHDirection();
    auto vDir = autoSelectMoveVDirection();
    auto move_by = MoveBy::create(0.5, Point(size.width * (float)hDir * 1, size.height*(float)vDir * 3));
    auto fadeout = FadeOut::create(0.5);
    auto remove = RemoveSelf::create(true);
    auto seq = Sequence::create(move_by, fadeout, remove, nullptr);
    font->runAction(seq);
}

void IngameEconomyManager::setAmount(float amount)
{
    this->m_economy.setAmount(amount);
}

bool IngameEconomyManager::pay(IngameEconomyManager *target, float money, bool animate)
{
    if(!this->pay(money, animate)) return false;

    target->receive(money, animate);
    return true;
}

void IngameEconomyManager::receive(float money, bool animate)
{
    this->m_economy.addMoney(money);
    if(animate) this->executeIndicator(money, false);
}

bool IngameEconomyManager::isPayable(float money)
{
    return this->m_economy.payable(money);
}

bool IngameEconomyManager::pay(float money, bool animate)
{
    if(!this->isPayable(money)) return false;

    this->m_economy.reduceMoney(money);
    if(animate) this->executeIndicator(money, true );

    return true;
}

void IngameEconomyManager::disable()
{
    this->m_pLabel->setVisible(false);
}

void IngameEconomyManager::enable()
{
    this->m_pLabel->setVisible(true);
}

///] Protected

IgEcoMng::MoveHDirection IngameEconomyManager::autoSelectMoveHDirection()
{
    if(this->m_pLabel->getPositionX() <= ZYDR_TGVS.width/2) return MoveHDirection::RIGHT;
    return MoveHDirection::LEFT;
}

IgEcoMng::MoveVDirection IngameEconomyManager::autoSelectMoveVDirection()
{
    if(this->m_pLabel->getPositionY() <= ZYDR_TGVS.height/2) return MoveVDirection::TOP;
    return MoveVDirection::BOTTOM;
}
