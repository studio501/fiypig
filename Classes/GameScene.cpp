#include "GameScene.h"

USING_NS_CC;

#define kTagFlutter 1
#define kTagMove 2

CCScene* GameScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

GameScene::GameScene()
    : m_pPig(NULL)
    , m_pFallAction(NULL)
    , m_pPillarAction(NULL)
{

}

GameScene::~GameScene()
{
    CC_SAFE_RELEASE_NULL(m_pPig);
    CC_SAFE_RELEASE_NULL(m_pFallAction);
    CC_SAFE_RELEASE_NULL(m_pPillarAction);
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    setTouchEnabled(true);

    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(GameScene::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "GameScene" splash screen"
    CCSprite* pSprite = CCSprite::create("GameScene.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);

    schedule(schedule_selector(GameScene::updatePillar), 0.1f, kCCRepeatForever, 0);
    return true;
}


void GameScene::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}

bool GameScene::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    raisePig();

    return true;
}

void GameScene::update(float delta)
{
    check();
}

void GameScene::addPig()
{
    CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
    cache->addSpriteFramesWithFile("pigs.plist");

    m_pPig = CCSprite::createWithSpriteFrameName("grossini_dance_01.png");
    m_pPig->retain();
    addChild(m_pPig);

    const int fameCount = 3;
    CCArray* animFrames = CCArray::createWithCapacity(fameCount);

    char str[100] = {0};
    for(int i = 1; i < fameCount; i++)
    {
        sprintf(str, "pig_%d.png", i);
        CCSpriteFrame* frame = cache->spriteFrameByName(str);
        animFrames->addObject(frame);
    }

    CCAction *action = CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 0.3f)));
    action->setTag(kTagFlutter);
    m_pPig->runAction(action);
}

void GameScene::raisePig()
{
    if (m_pFallAction == NULL)
    {
        m_pFallAction = CCMoveBy::create(2.0f, ccp(0, -800));
        m_pFallAction->retain();
    }

    CCActionInterval* raiseEaseOut = CCEaseOut::create(CCMoveBy::create(1, ccp(0, 70)), 2.5f);
    CCSequence *action = CCSequence::createWithTwoActions(raiseEaseOut, m_pFallAction);
    action->setTag(kTagMove);

    m_pPig->stopActionByTag(kTagMove);
    m_pPig->runAction(action);
}

void GameScene::updatePillar(float dt)
{
    CCPoint visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

    // 0.54
    float lastPillarPosX = -10;
    if (m_Pillars[0] != NULL)
    {
        lastPillarPosX = m_Pillars[2] == NULL ? m_Pillars[0]->getPositionX() : m_Pillars[2]->getPositionX();
    }

    if (lastPillarPosX + visibleSize.width * 0.5625 > visibleOrigin.x + visibleSize.width)
    {
        //add pillar
    }
}

void GameScene::check()
{

}
