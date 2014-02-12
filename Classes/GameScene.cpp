#include "GameScene.h"

USING_NS_CC;

#define kTagFlutter 1
#define kTagRaise 2

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
    : m_VisibleOrigin(CCPointZero)
    , m_VisibleSize(CCSizeZero)
    , m_pBackground1(NULL)
    , m_pBackground2(NULL)
    , m_fForwordVelocity(-210)
    , m_pPig(NULL)
    , m_CurrentScore(0)
    , m_GroundPosY(0)
    , m_Gap(160)
{
    memset(m_Pillars, 0, sizeof(m_Pillars));
    m_VisibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    m_VisibleSize = CCDirector::sharedDirector()->getVisibleSize();
}

GameScene::~GameScene()
{
    CC_SAFE_RELEASE_NULL(m_pBackground1);
    CC_SAFE_RELEASE_NULL(m_pBackground2);
    CC_SAFE_RELEASE_NULL(m_pPig);
    for (int i = 0; i < sizeof(m_Pillars) / sizeof(m_Pillars[0]); i++)
    {
        CC_SAFE_RELEASE_NULL(m_Pillars[i]);
    }
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

    m_GroundPosY = 187/* ground position in bg1.jpg(bg2.jpg) */ - m_VisibleOrigin.y;

    m_pBackground1 = CCSprite::create("bg1.jpg");
    m_pBackground1->retain();
    m_pBackground1->setPosition(m_VisibleOrigin + m_VisibleSize / 2);
    addChild(m_pBackground1);

    m_pBackground2 = CCSprite::create("bg1.jpg");
    m_pBackground2->retain();
    m_pBackground2->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 1.5f, m_VisibleOrigin.y + m_VisibleSize.height * 0.5f));
    addChild(m_pBackground2);

    addPig();

    scheduleUpdate();
    schedule(schedule_selector(GameScene::updatePillar), 0.1f, kCCRepeatForever, 0);

    return true;
}

void GameScene::onEnter()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}

void GameScene::onExit()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}  

bool flag =  false;
bool GameScene::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    flag = true;
    raisePig();

    return true;
}

void GameScene::update(float delta)
{
    float deltaX = m_fForwordVelocity * delta;
    m_pBackground1->setPositionX(m_pBackground1->getPositionX() + deltaX);
    m_pBackground2->setPositionX(m_pBackground1->getPositionX() + m_pBackground1->getContentSize().width);

    if (m_pBackground1->getPositionX() + m_pBackground1->getContentSize().width / 2 <= m_VisibleOrigin.x)
    {
        CCSprite *pTemp = m_pBackground1;
        m_pBackground1 = m_pBackground2;
        m_pBackground2 = pTemp;

        m_pBackground2->setPositionX(m_pBackground1->getPositionX() + m_pBackground1->getContentSize().width);
    }

    for (int i = 0; i < 4; i++)
    {
        if (m_Pillars[i] != NULL)
        {
            m_Pillars[i]->setPositionX(m_Pillars[i]->getPositionX() + deltaX);
        }
    }

    CCAction *pAction = m_pPig->getActionByTag(kTagRaise);
    if (flag && (!pAction || pAction->isDone()))
    {
        m_pPig->setPositionY(m_pPig->getPositionY() + (-600 * delta));
    }
    //check();
}

void GameScene::addPig()
{
    const static int fameCount = 3;
    const static float animationDuration = 0.15f;

    CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
    cache->addSpriteFramesWithFile("pigs.plist");

    m_pPig = CCSprite::createWithSpriteFrameName("pig1");
    m_pPig->retain();
    m_pPig->setScale(0.8f);
    m_pPig->setZOrder(1);
    m_pPig->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 3, m_VisibleOrigin.y + m_VisibleSize.height / 2));
    addChild(m_pPig);

    CCArray *animFrames = CCArray::createWithCapacity(fameCount);
    for(int i = 1; i <= fameCount; i++)
    {
        CCSpriteFrame* frame = cache->spriteFrameByName(CCString::createWithFormat("pig%d", i)->getCString());
        animFrames->addObject(frame);
    }

    CCAction *action = CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, animationDuration)));
    action->setTag(kTagFlutter);
    m_pPig->runAction(action);
}

void GameScene::raisePig()
{
    const float tapRaiseHeight = 80;
    float raiseHeight = MIN(tapRaiseHeight, m_VisibleOrigin.y + m_VisibleSize.height - m_pPig->getPositionY() - m_pPig->getContentSize().height / 2);

    CCActionInterval* raiseEaseOut = CCEaseSineOut::create(CCMoveBy::create(0.4f * raiseHeight / tapRaiseHeight, ccp(0, raiseHeight)));
    CCSpawn *action = CCSpawn::createWithTwoActions(raiseEaseOut, CCRotateTo::create(0.2, -15));
    action->setTag(kTagRaise);

    m_pPig->stopActionByTag(kTagRaise);
    m_pPig->runAction(action);
}

void GameScene::updatePillar(float dt)
{
    const static int pillarWidth = 88;
    const static int minPillarHeight = 150;

    int firstEmptyIndex = 0;
    float lastPillarPosX = -1000;

    if (m_Pillars[0] != NULL)
    {
        // not visible
        if (m_Pillars[0]->getPositionX() + m_Pillars[0]->getContentSize().width / 2 <= 0)
        {
            m_Pillars[0]->removeFromParent();
            m_Pillars[1]->removeFromParent();
            CC_SAFE_RELEASE_NULL(m_Pillars[0]);
            CC_SAFE_RELEASE_NULL(m_Pillars[1]);

            m_Pillars[0] = m_Pillars[2];
            m_Pillars[1] = m_Pillars[3];
            m_Pillars[2] = NULL;
            m_Pillars[3] = NULL;
        }

        if (m_Pillars[0] == NULL)
        {
        }
        else if (m_Pillars[2] == NULL)
        {
            firstEmptyIndex = 2;
            if (m_Pillars[0] != NULL)
            {
                lastPillarPosX = m_Pillars[0]->getPositionX();
            }
        }
        else
        {
            firstEmptyIndex = 4;
            lastPillarPosX = m_Pillars[2]->getPositionX();
        }
    }

    // add new pillar
    if (lastPillarPosX + m_VisibleSize.width * 0.5625 <= m_VisibleOrigin.x + m_VisibleSize.width && firstEmptyIndex <= 2)
    {
        int bottomPillar_Height = minPillarHeight + CCRANDOM_0_1() * (m_VisibleSize.height - m_GroundPosY - minPillarHeight - minPillarHeight - m_Gap);
        int bottomPillar_BottomY = m_VisibleOrigin.y + m_GroundPosY;
        int topPillar_BottomY = bottomPillar_BottomY + bottomPillar_Height + m_Gap;
        int topPillar_Height = m_VisibleSize.height - topPillar_BottomY;
        m_Pillars[firstEmptyIndex] = CCSprite::create("pillar.png", CCRectMake(0, 0, pillarWidth, topPillar_Height));
        m_Pillars[firstEmptyIndex]->retain();
        m_Pillars[firstEmptyIndex]->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width + pillarWidth / 2, topPillar_BottomY + topPillar_Height / 2));
        m_Pillars[firstEmptyIndex]->setRotation(180);
        addChild(m_Pillars[firstEmptyIndex]);

        m_Pillars[firstEmptyIndex + 1] = CCSprite::create("pillar.png", CCRectMake(0, 0, pillarWidth, bottomPillar_Height));
        m_Pillars[firstEmptyIndex + 1]->retain();
        m_Pillars[firstEmptyIndex + 1]->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width + pillarWidth / 2, bottomPillar_BottomY + bottomPillar_Height / 2));
        addChild(m_Pillars[firstEmptyIndex + 1]);
    }
}

void GameScene::check()
{
    bool crashed = false;

    for (int i = 0; i < 4; i++)
    {
        if (m_Pillars[i] != NULL && m_Pillars[i]->boundingBox().intersectsRect(m_pPig->boundingBox()))
        {
            crashed = true;
            break;
        }
    }

    if (!crashed)
    {
        if (m_pPig->getPositionY() - m_pPig->getContentSize().height / 2 < m_GroundPosY)
        {
            crashed = true;
        }
    }

    if (crashed)
    {
        for (int i = 0; i < 4; i++)
        {
            if (m_Pillars[i] != NULL)
            {
                m_Pillars[i]->stopAllActions();
            }
        }
    }
}
