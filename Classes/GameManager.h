#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

#include "cocos2d.h"
#include <string.h>

class GameManager
{
public:
    GameManager(void);
    ~GameManager(void);

    void addGoalToday();
    int getGoalsToday();
    void saveRated(bool rated);
    bool getRated();
    void saveLaterTS(int days);
    int getLaterTS();

    static GameManager *getInstance();
    static void destroyInstance();

private:
    static GameManager *sInstance;
};

#endif // !_GAME_MANAGER_H
