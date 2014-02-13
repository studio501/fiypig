#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class MainScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    MainScene();
    virtual ~MainScene();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static MainScene *create();

    void soundCallback(CCObject *sender);
    void playCallback(CCObject *sender);
    void rankCallback(CCObject *sender);
    void moreCallback(CCObject *sender);

    virtual void onEnter();
    virtual void keyBackClicked();

private:
    CCMenuItemImage *m_pSound;
};

#endif // __MAIN_SCENE_H__
