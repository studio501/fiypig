#include "GameScene.h"
#include "LGMenuItemImage.h"
#include "LGLayer.h"

#include "SimpleAudioEngine.h"

USING_NS_CC;

#define kTagFlutter 1
#define kTagRaise 2

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
    , m_ScoreLabel(NULL)
    , m_Title(NULL)
    , m_Hint(NULL)
    , m_fForwordVelocity(-210)
    , m_fFallVelocity(-600)
    , m_pPig(NULL)
    , m_PassedTime(0)
    , m_CurrentScore(0)
    , m_LastPassedPillar(NULL)
    , m_GroundPosY(0)
    , m_Gap(160)
    , m_Started(false)
    , m_Paused(false)
    , m_Crashed(false)
    , m_pPause(NULL)
    , m_pPauseLayer(NULL)
    , m_pOverLayer(NULL)
    , m_CurrentLayer(NULL)
{
    memset(m_Pillars, 0, sizeof(m_Pillars));
    m_VisibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
    m_VisibleSize = CCDirector::sharedDirector()->getVisibleSize();
    m_Sound = CCUserDefault::sharedUserDefault()->getBoolForKey("sound");
}

GameScene::~GameScene()
{
    CC_SAFE_RELEASE_NULL(m_pBackground1);
    CC_SAFE_RELEASE_NULL(m_pBackground2);
    CC_SAFE_RELEASE_NULL(m_ScoreLabel);
    CC_SAFE_RELEASE_NULL(m_Title);
    CC_SAFE_RELEASE_NULL(m_Hint);
    CC_SAFE_RELEASE_NULL(m_pPig);
    for (int i = 0; i < sizeof(m_Pillars) / sizeof(m_Pillars[0]); i++)
    {
        CC_SAFE_RELEASE_NULL(m_Pillars[i]);
    }
    m_LastPassedPillar =  NULL;

    CC_SAFE_RELEASE_NULL(m_pPause);
    CC_SAFE_RELEASE_NULL(m_pPauseLayer);
    CC_SAFE_RELEASE_NULL(m_pOverLayer);
    CC_SAFE_RELEASE_NULL(m_CurrentLayer);
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
    setKeypadEnabled(true);

    m_GroundPosY = 187/* ground position in bg1.jpg(bg2.jpg) */ - m_VisibleOrigin.y;

    m_pBackground1 = CCSprite::create("bg1.jpg");
    m_pBackground1->retain();
    m_pBackground1->setPosition(m_VisibleOrigin + m_VisibleSize / 2);
    addChild(m_pBackground1);

    m_pBackground2 = CCSprite::create("bg1.jpg");
    m_pBackground2->retain();
    m_pBackground2->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 1.5f, m_VisibleOrigin.y + m_VisibleSize.height * 0.5f));
    addChild(m_pBackground2);

    m_ScoreLabel = CCLabelAtlas::create(CCString::createWithFormat("%d", m_CurrentScore)->getCString(), "fonts/font1.png", 56, 72, '0');
    m_ScoreLabel->retain();
    m_ScoreLabel->setZOrder(1);
    m_ScoreLabel->setAnchorPoint(ccp(0.5, 0.5));
    m_ScoreLabel->setPosition(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height * 0.86125);
    addChild(m_ScoreLabel);

    m_Title = CCSprite::create("title_ready.png");
    m_Title->retain();
    m_Title->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.x + m_VisibleSize.height * 0.725f));
    addChild(m_Title);
    m_Hint = CCSprite::create("hint.png");
    m_Hint->retain();
    m_Hint->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height / 2));
    addChild(m_Hint);

    m_pPause = LGMenuItemImage::create("pause.png", NULL, this, menu_selector(GameScene::pauseCallback));
    m_pPause->retain();
    m_pPause->setVisible(false);
    m_pPause->setPosition(m_VisibleOrigin.x + 43, m_VisibleOrigin.y + m_VisibleSize.height - 41);
    CCMenu *pMenu = CCMenu::create(m_pPause, NULL);
    pMenu->setZOrder(3);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    addPig();

    scheduleUpdate();
    schedule(schedule_selector(GameScene::updatePillar), 0.1f, kCCRepeatForever, 0);

    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_DIE);
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_HIT);
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_POINT);
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_SWOOSHING);
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(SFX_WING);
    }

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

bool GameScene::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    if (!m_Started)
    {
        m_Started = true;
        m_pPause->setVisible(true);
        m_Title->removeFromParent();
        CC_SAFE_RELEASE_NULL(m_Title);
        m_Hint->removeFromParent();
        CC_SAFE_RELEASE_NULL(m_Hint);

        if (m_Sound)
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
        }
    }
    else if (!m_Crashed)
    {
        if (m_Sound)
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_WING);
        }
    }

    raisePig();

    return true;
}

void GameScene::draw()
{
    CCLayer::draw();

    //glLineWidth(1);
    //ccDrawColor4B(255, 100, 100, 255);
    //for (int i = 0; i < 4; i++)
    //{
    //    if (m_Pillars[i] != NULL)
    //    {
    //        ccDrawRect(m_Pillars[i]->boundingBox().origin, m_Pillars[i]->boundingBox().origin + m_Pillars[i]->boundingBox().size);
    //    }
    //}

    //if (m_pPig)
    //{
    //    float w = m_pPig->getContentSize().width * m_pPig->getScaleX(), h = m_pPig->getContentSize().height * m_pPig->getScaleY();
    //    CCRect pigRect = CCRectMake(m_pPig->getPositionX() - w / 2, m_pPig->getPositionY() - h / 2, w, h);

    //    ccDrawRect(pigRect.origin, pigRect.origin + pigRect.size);
    //}
}

void GameScene::update(float delta)
{
    bool oldCrashed = m_Crashed;
    if (m_Started && !m_Paused)
    {
        if (!m_Crashed)
        {
            float deltaX = m_fForwordVelocity * delta;

            // move background
            m_pBackground1->setPositionX(m_pBackground1->getPositionX() + deltaX);
            m_pBackground2->setPositionX(m_pBackground1->getPositionX() + m_pBackground1->getContentSize().width);

            if (m_pBackground1->getPositionX() + m_pBackground1->getContentSize().width / 2 <= m_VisibleOrigin.x)
            {
                CCSprite *pTemp = m_pBackground1;
                m_pBackground1 = m_pBackground2;
                m_pBackground2 = pTemp;

                m_pBackground2->setPositionX(m_pBackground1->getPositionX() + m_pBackground1->getContentSize().width);
            }

            // move pillars
            for (int i = 0; i < 4; i++)
            {
                if (m_Pillars[i] != NULL)
                {
                    m_Pillars[i]->setPositionX(m_Pillars[i]->getPositionX() + deltaX);
                    float w = m_pPig->getContentSize().width * m_pPig->getScaleX(), h = m_pPig->getContentSize().height * m_pPig->getScaleY();
                    CCRect pigRect = CCRectMake(m_pPig->getPositionX() - w / 2, m_pPig->getPositionY() - h / 2, w, h);
                    if (m_Pillars[i]->boundingBox().intersectsRect(pigRect))
                    {
                        m_Crashed = true;
                        //m_Pillars[i]->setPositionX(m_pPig->getPositionX() + m_pPig->getContentSize().width * m_pPig->getScale() / 2 + 44);
                    }
                }
            }
        }

        // fall & rotate pig
        CCAction *pAction = m_pPig->getActionByTag(kTagRaise);
        if (!pAction || pAction->isDone())
        {
            float posY = m_pPig->getPositionY() +m_fFallVelocity * delta;
            if (posY <= m_VisibleOrigin.y + m_GroundPosY + m_pPig->getContentSize().height / 2)
            {
                posY = m_VisibleOrigin.y + m_GroundPosY + m_pPig->getContentSize().height / 2;
                m_Crashed = true;
            }

            m_pPig->setPositionY(posY);
            float orgRotation = m_pPig->getRotation();
            float rotation = m_pPig->getRotation() + ((orgRotation >= 0 ? 300 : (25 + orgRotation) * 2000 / 25)* delta);
            if (orgRotation < -10)
            {
                rotation = rotation * 0.1f + orgRotation * 0.9f;
            }
            rotation = MIN(rotation, 80);
            m_pPig->setRotation(rotation);
        }

        if (m_Crashed)
        {
            if (!oldCrashed)
            {
                m_pPig->stopAllActions();

                showOverView();

                if (m_Sound)
                {
                    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_HIT);
                    scheduleOnce(schedule_selector(GameScene::playDieEffect), 0.5f);
                }
            }
        }
        else
        {
            if (m_pPig && m_Pillars[0] && m_pPig->getPositionX() > m_Pillars[0]->getPositionX() && m_LastPassedPillar != m_Pillars[0])
            {
                m_CurrentScore++;
                m_LastPassedPillar = m_Pillars[0];

                m_ScoreLabel->setString(CCString::createWithFormat("%d", m_CurrentScore)->getCString());
                if (m_Sound)
                {
                    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_POINT);
                }
            }
        }
    }
}

void GameScene::keyBackClicked()
{
    CCLayer::keyBackClicked();

    if (m_Started)
    {
        if (!m_CurrentLayer)
        {
            showPauseView();
        }
        else if (m_CurrentLayer == m_pPauseLayer)
        {
            m_CurrentLayer->removeFromParent();
            m_CurrentLayer->release();
            m_CurrentLayer = NULL;
            onShowOverlay(false);
        }
    }
    else
    {
        CCDirector::sharedDirector()->popScene();
    }
}

void GameScene::pauseCallback(CCObject* pSender)
{
    showPauseView();
}

void GameScene::continueCallback(CCObject* pSender)
{
    m_Paused = false;

    m_pPauseLayer->removeFromParent();
    CC_SAFE_RELEASE_NULL(m_CurrentLayer);
    onShowOverlay(false);
}

void GameScene::menuCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->popScene();
}

void GameScene::retryCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->replaceScene(GameScene::scene());
}

void GameScene::addPig()
{
    const static int fameCount = 3;
    const static float animationDuration = 0.15f;

    CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
    cache->addSpriteFramesWithFile("pigs.plist");

    m_pPig = CCSprite::createWithSpriteFrameName("pig1");
    m_pPig->retain();
    m_pPig->setScale(0.7f);
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
    if (!m_Crashed)
    {
        const float tapRaiseHeight = 80;
        float raiseHeight = MIN(tapRaiseHeight, m_VisibleOrigin.y + m_VisibleSize.height - m_pPig->getPositionY() - m_pPig->getContentSize().height / 2);

        CCActionInterval* raiseEaseOut = CCEaseSineOut::create(CCMoveBy::create(0.4f * raiseHeight / tapRaiseHeight, ccp(0, raiseHeight)));
        CCSpawn *action = CCSpawn::createWithTwoActions(raiseEaseOut, CCRotateTo::create(0.2f, -15));
        action->setTag(kTagRaise);

        m_pPig->stopActionByTag(kTagRaise);
        m_pPig->runAction(action);
    }
}

void GameScene::updatePillar(float dt)
{
    m_PassedTime += dt;
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
    if ((m_Pillars[0] == NULL && m_PassedTime > 6) || (m_Pillars[0] != NULL && lastPillarPosX + m_VisibleSize.width * 0.5625 <= m_VisibleOrigin.x + m_VisibleSize.width && firstEmptyIndex <= 2))
    {
        int bottomPillar_Height = minPillarHeight + CCRANDOM_0_1() * (m_VisibleSize.height - m_GroundPosY - minPillarHeight - minPillarHeight - m_Gap);
        int bottomPillar_BottomY = m_VisibleOrigin.y + m_GroundPosY;
        int topPillar_BottomY = bottomPillar_BottomY + bottomPillar_Height + m_Gap;
        int topPillar_Height = m_VisibleSize.height - topPillar_BottomY + m_VisibleOrigin.y;
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

void GameScene::showPauseView()
{
    m_Paused = true;

    if (m_pPauseLayer == NULL)
    {
        m_pPauseLayer = LGLayerColor::create(ccc4(0, 0, 0, 0));
        m_pPauseLayer->retain();
        m_pPauseLayer->setTouchMode(kCCTouchesOneByOne);
        m_pPauseLayer->setTouchEnabled(true);
        m_pPauseLayer->setZOrder(3);

        CCSprite *pTitle = CCSprite::create("title_pause.png");
        pTitle->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2 , m_VisibleOrigin.y + m_VisibleSize.height * 0.67375f));
        m_pPauseLayer->addChild(pTitle);

        CCMenuItemImage *pContinue = LGMenuItemImage::create("play.png", NULL, this, menu_selector(GameScene::continueCallback));
        CCMenuItemImage *pMenu = LGMenuItemImage::create("menu.png", NULL, this, menu_selector(GameScene::menuCallback));

        pContinue->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 0.26666667f, m_VisibleOrigin.y + m_VisibleSize.height * 0.41125));
        pMenu->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 0.73333333f, pContinue->getPositionY()));

        CCMenu *pCCMenu = CCMenu::create(pContinue, pMenu, NULL);
        pCCMenu->setAnchorPoint(CCPointZero);
        pCCMenu->setPosition(CCPointZero);
        m_pPauseLayer->addChild(pCCMenu);
    }

    if (m_pPauseLayer && m_pPauseLayer->getParent() == NULL)
    {
        getParent()->addChild(m_pPauseLayer);
        CC_SAFE_RELEASE_NULL(m_CurrentLayer);
        m_CurrentLayer = m_pPauseLayer;
        m_CurrentLayer->retain();
        onShowOverlay(true);
    }
}

void GameScene::showOverView()
{
    if (m_pOverLayer == NULL)
    {
        m_pOverLayer = LGLayerColor::create(ccc4(0, 0, 0, 0));
        m_pOverLayer->retain();
        m_pOverLayer->setTouchMode(kCCTouchesOneByOne);
        m_pOverLayer->setTouchEnabled(true);
        m_pOverLayer->setZOrder(3);

        CCSprite *pTitle = CCSprite::create("title_over.png");
        pTitle->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height * 0.76f));
        m_pOverLayer->addChild(pTitle);

        CCSprite *pBgScore = CCSprite::create("bg_score.png");
        pBgScore->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height * 0.535f));
        m_pOverLayer->addChild(pBgScore);

        float bgW = pBgScore->getContentSize().width, bgH = pBgScore->getContentSize().height;

        int record = CCUserDefault::sharedUserDefault()->getIntegerForKey("record", 0);
        if (record < m_CurrentScore)
        {
            record = m_CurrentScore;
            CCUserDefault::sharedUserDefault()->setIntegerForKey("record", record);

            CCSprite *indicatorNew = CCSprite::create("new.png");
            indicatorNew->setPosition(ccp(pBgScore->getPositionX() - bgW * 0.2448980f, pBgScore->getPositionY() - bgH * 0.08035714f));
            m_pOverLayer->addChild(indicatorNew);
        }

        CCLabelAtlas *current = CCLabelAtlas::create(CCString::createWithFormat("%d", m_CurrentScore)->getCString(), "fonts/font2.png", 36, 48, '0');
        current->setAnchorPoint(ccp(0.5, 0.5));
        current->setPosition(pBgScore->getPositionX(), pBgScore->getPositionY() + bgH * 0.1394f);
        m_pOverLayer->addChild(current);
        CCLabelAtlas *best = CCLabelAtlas::create(CCString::createWithFormat("%d", record)->getCString(), "fonts/font2.png", 36, 48, '0');
        best->setAnchorPoint(ccp(0.5, 0.5));
        best->setPosition(pBgScore->getPositionX(), pBgScore->getPositionY() - bgH * 0.27679f);
        m_pOverLayer->addChild(best);

        CCMenuItemImage *pRetry = LGMenuItemImage::create("retry.png", NULL, this, menu_selector(GameScene::retryCallback));
        CCMenuItemImage *pMenu = LGMenuItemImage::create("menu.png", NULL, this, menu_selector(GameScene::menuCallback));

        pRetry->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 0.26666667f, m_VisibleOrigin.y + m_VisibleSize.height * 0.285f));
        pMenu->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 0.73333333f, pRetry->getPositionY()));

        CCMenu *pCCMenu = CCMenu::create(pRetry, pMenu, NULL);
        pCCMenu->setAnchorPoint(CCPointZero);
        pCCMenu->setPosition(CCPointZero);
        m_pOverLayer->addChild(pCCMenu);
    }

    if (m_pOverLayer && m_pOverLayer->getParent() == NULL)
    {
        getParent()->addChild(m_pOverLayer);
        CC_SAFE_RELEASE_NULL(m_CurrentLayer);
        m_CurrentLayer = m_pOverLayer;
        m_CurrentLayer->retain();
        onShowOverlay(true);
    }
    m_ScoreLabel->setVisible(false);
}

void GameScene::onShowOverlay(bool show)
{
    m_pPause->setVisible(!show);
}

void GameScene::playDieEffect(float delta)
{
    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_DIE);
    }
}