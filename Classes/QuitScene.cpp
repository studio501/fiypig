#include "QuitScene.h"

#include "NativeBridge.h"
#include "LGMenuItemImage.h"

#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

CCScene *QuitScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    QuitScene *layer = QuitScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

QuitScene *QuitScene::create()
{
    QuitScene *pRet = new QuitScene();
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

QuitScene::QuitScene()
    : m_Sound(false)
{
}

QuitScene::~QuitScene()
{
}

bool QuitScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    setKeypadEnabled(true);

    m_Sound = CCUserDefault::sharedUserDefault()->getBoolForKey("sound", true);

    CCPoint visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint visibleCenter = visibleOrigin + visibleSize / 2;

    CCSprite *sprite = CCSprite::create("bg1.jpg");
    sprite->setPosition(visibleCenter);
    addChild(sprite);

    sprite = CCSprite::create("quit_title.png");
    sprite->setPosition(ccp(visibleCenter.x, visibleOrigin.y + 0.90125 * visibleSize.height));
    addChild(sprite);

    CCMenuItemImage *pYes = LGMenuItemImage::create("quit_yes.png", NULL, this, menu_selector(QuitScene::yesCallback));
    pYes->setPosition(ccp(visibleOrigin.x + visibleSize.width * 0.1354167f, visibleOrigin.y + visibleSize.height * 0.05875f));

    CCMenuItemImage *pNo = LGMenuItemImage::create("quit_no.png", NULL, this, menu_selector(QuitScene::noCallback));
    pNo->setPosition(ccp(visibleOrigin.x + visibleSize.width * 0.8645833f, pYes->getPositionY()));

    CCMenuItemImage *pMore = LGMenuItemImage::create("quit_more.png", NULL, this, menu_selector(QuitScene::moreCallback));
    pMore->setPosition(ccp(visibleOrigin.x + visibleSize.width / 2, pYes->getPositionY()));

    CCMenu *pMenu = CCMenu::create(pYes, pNo, pMore, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void QuitScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::showAdMobMRect(true);
    NativeBridge::showAdMobBanner(false);
}

void QuitScene::onExit()
{
    CCLayer::onExit();
    NativeBridge::showAdMobMRect(false);
    NativeBridge::showAdMobBanner(true);
}

void QuitScene::yesCallback(CCObject *sender)
{
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void QuitScene::noCallback(CCObject *sender)
{
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    CCDirector::sharedDirector()->popScene();
}

void QuitScene::moreCallback(CCObject *sender)
{
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    NativeBridge::onMoreClicked();
}

void QuitScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }
    CCDirector::sharedDirector()->popScene();
}
