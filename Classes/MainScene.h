#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

// android effect only support ogg
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define SFX_DIE             "sfx_die.ogg"
#define SFX_HIT             "sfx_hit.ogg"
#define SFX_POINT         "sfx_point.ogg"
#define SFX_SWOOSHING "sfx_swooshing.ogg"
#define SFX_WING          "sfx_wing.ogg"
#else
#define SFX_DIE             "sfx_die.mp3"
#define SFX_HIT             "sfx_hit.mp3"
#define SFX_POINT         "sfx_point.mp3"
#define SFX_SWOOSHING "sfx_swooshing.mp3"
#define SFX_WING          "sfx_wing.mp3"
#endif // CC_PLATFORM_ANDROID

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
    bool m_Sound;
};

#endif // __MAIN_SCENE_H__
