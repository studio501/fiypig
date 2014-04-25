#include "MainScene.h"
#include "GameScene.h"
#include "QuitScene.h"
#include "NativeBridge.h"

#include <string>
#include "LGMenuItemImage.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

CCScene *MainScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    MainScene *layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

MainScene *MainScene::create()
{
    MainScene *pRet = new MainScene();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    } 
    else 
    { 
        delete pRet;
        pRet = NULL;
        return NULL;
    } 
}

MainScene::MainScene()
    : m_Sound(false)
{
}

MainScene::~MainScene()
{
}

bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    setKeypadEnabled(true);

    CCPoint visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint visibleCenter = visibleOrigin + visibleSize / 2;

    CCSprite *sprite = CCSprite::create("bg1.jpg");
    sprite->setPosition(visibleCenter);
    addChild(sprite);

    sprite = CCSprite::create("title.png");
    sprite->setAnchorPoint(ccp(0.5, 0));
    sprite->setPosition(ccp(visibleCenter.x, visibleCenter.y + 0.24125f * visibleSize.height));
    addChild(sprite);

    m_Sound = CCUserDefault::sharedUserDefault()->getBoolForKey("sound", true);
    CCUserDefault::sharedUserDefault()->setBoolForKey("sound", m_Sound);
    m_pSound = LGMenuItemImage::create("sound_off.png", "sound_on.png", this, menu_selector(MainScene::soundCallback));
    m_pSound->setScale(0.65f);
    m_pSound->setPosition(visibleOrigin.x + visibleSize.width * 0.9f, visibleOrigin.y + visibleSize.height * 0.94375f);
    if (m_Sound)
    {
        m_pSound->selected();
    }
    else
    {
        m_pSound->unselected();
    }

    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_DIE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_HIT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_POINT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_SWOOSHING);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_WING);

    CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
    cache->addSpriteFramesWithFile("pigs.plist");

    CCSprite *pig = CCSprite::createWithSpriteFrameName("pig1");
    pig->setPosition(ccp(visibleCenter.x, visibleCenter.y + 0.06125f * visibleSize.height));
    addChild(pig);

    CCMenuItemImage *pPlay = LGMenuItemImage::create("play.png", NULL, this, menu_selector(MainScene::playCallback));
    pPlay->setPosition(ccp(visibleOrigin.x + 0.272917f * visibleSize.width, visibleOrigin.y + 0.305625f * visibleSize.height));
    //pPlay->setPosition(ccp(visibleCenter.x, visibleCenter.y - 0.12125f * visibleSize.height));

    CCMenuItemImage *pRank = LGMenuItemImage::create("rank.png", NULL, this, menu_selector(MainScene::rankCallback));
    pRank->setPosition(ccp(visibleOrigin.x + 0.727083f * visibleSize.width, pPlay->getPositionY()));

    CCMenuItemImage *pMore = LGMenuItemImage::create("more.png", NULL, this, menu_selector(MainScene::moreCallback));
    pMore->setPosition(ccp(visibleCenter.x, visibleCenter.y - 0.05125f * visibleSize.height));
    //pMore->setPosition(ccp(visibleCenter.x, visibleOrigin.y + 0.225625f * visibleSize.height));

    CCMenu *pMenu = CCMenu::create(m_pSound, pPlay, pRank, pMore, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void MainScene::soundCallback(CCObject *sender)
{
    m_Sound = !m_Sound;
    CCUserDefault::sharedUserDefault()->setBoolForKey("sound", m_Sound);
    if (m_Sound)
    {
        m_pSound->selected();
    }
    else
    {
        m_pSound->unselected();
    }
}

void MainScene::playCallback(CCObject *sender)
{
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    CCDirector::sharedDirector()->pushScene(GameScene::scene());
}

void MainScene::rankCallback(CCObject *sender)
{
    scheduleOnce(schedule_selector(MainScene::rankSchedule), 0.1f);
}

void MainScene::moreCallback(CCObject *sender)
{
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    NativeBridge::onMoreClicked();
}

void MainScene::rankSchedule(float dt)
{
    NativeBridge::showLeaderboard();
}

void MainScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(false);
}

void MainScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    CCDirector::sharedDirector()->pushScene(QuitScene::scene());
}
