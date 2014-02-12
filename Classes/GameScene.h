#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::CCLayer
{
public:
    GameScene();
    ~GameScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);

    virtual bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
    virtual void update(float delta);

private:
    void addPig();
    void raisePig();
    void updatePillar(float dt);
    void check();

private:
    cocos2d::CCSprite *m_pPig;
    cocos2d::CCSprite *m_Pillars[4];

    cocos2d::CCFiniteTimeAction *m_pFallAction;
    cocos2d::CCAction *m_pPillarAction;
};

#endif // __GAME_SCENE_H__
