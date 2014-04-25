#include "GameManager.h"
#include "Util.h"

USING_NS_CC;

GameManager *GameManager::sInstance = NULL;

GameManager *GameManager::getInstance()
{
    if (sInstance == NULL)
    {
        sInstance = new GameManager();
    }

    return sInstance;
}

GameManager::GameManager(void)
{
}

GameManager::~GameManager(void)
{
}

void GameManager::addGoalToday()
{
    CCUserDefault::sharedUserDefault()->setStringForKey("goals", CCString::createWithFormat("%d|%d", Util::getCurrentDays(), getGoalsToday() + 1)->getCString());
}

int GameManager::getGoalsToday()
{
    std::string strGoals = CCUserDefault::sharedUserDefault()->getStringForKey("goals", "");
    if (!strGoals.empty())
    {
        size_t pos = strGoals.find_first_of('|');
        CCString *strDays = CCString::create(strGoals.substr(0, pos));

        if (strDays->intValue() == Util::getCurrentDays())
        {
            return CCString::create(strGoals.substr(pos + 1))->intValue();
        }
    }

    return 0;
}

void GameManager::saveRated(bool rated)
{
    CCUserDefault::sharedUserDefault()->setBoolForKey("rated", rated);
}

bool GameManager::getRated()
{
    return CCUserDefault::sharedUserDefault()->getBoolForKey("rated", false);
}

void GameManager::saveLaterTS(int days)
{
    CCUserDefault::sharedUserDefault()->setIntegerForKey("laterTS", days);
}

int GameManager::getLaterTS()
{
    return CCUserDefault::sharedUserDefault()->getIntegerForKey("laterTS", 0);
}
