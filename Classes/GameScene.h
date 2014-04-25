#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class LGMenuItemImage;

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

    virtual void onEnter();
    virtual void onExit();

    virtual bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event);
    virtual void draw();
    virtual void update(float delta);
    virtual void keyBackClicked();

    void pauseCallback(CCObject* pSender);
    void continueCallback(CCObject* pSender);
    void menuCallback(CCObject* pSender);
    void retryCallback(CCObject* pSender);
    void rateCallback(CCObject *pSender);
    void laterCallback(CCObject *pSender);

private:
    void addPig();
    void raisePig();
    void updatePillar(float dt);
    void showPauseView();
    void showOverView();
    void showRateView();
    void onShowOverlay(bool show);
    void playDieEffect(float delta);
    void showOverViewSchedule(float dt);

private:
    cocos2d::CCPoint m_VisibleOrigin;
    cocos2d::CCSize m_VisibleSize;
    cocos2d::CCSprite *m_pBackground1;
    cocos2d::CCSprite *m_pBackground2;
    cocos2d::CCLabelAtlas *m_ScoreLabel;
    cocos2d::CCSprite *m_Title;
    cocos2d::CCSprite *m_Hint;
    float m_fForwordVelocity;
    float m_fFallVelocity;
    float m_fMaxVelocity;

    cocos2d::CCSprite *m_pPig;
    cocos2d::CCSprite *m_Pillars[4];
    float m_PassedTime;
    int m_CurrentScore;
    cocos2d::CCSprite *m_LastPassedPillar;
    int m_GroundPosY; // ground y position relative to visible origin
    int m_Gap;
    bool m_Started;
    bool m_Paused;
    bool m_Crashed;

    LGMenuItemImage *m_pPause;
    cocos2d::CCLayer *m_pPauseLayer;
    cocos2d::CCLayer *m_pOverLayer;
    cocos2d::CCLayer *m_pRateLayer;
    cocos2d::CCLayer *m_CurrentLayer;

    bool m_Sound;
    bool m_ToShowOverView;
};

#endif // __GAME_SCENE_H__
