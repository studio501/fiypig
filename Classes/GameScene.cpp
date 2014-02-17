#include "GameScene.h"
#include "LGMenuItemImage.h"
#include "LGLayer.h"

#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "GameManager.h"
#include "Util.h"
#include "NativeBridge.h"

USING_NS_CC;

#define kTagFlutter 1
#define kTagRaise 2
#define kTagOverScoreBoard 3
#define kTagOverMenu 4

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
    , m_fFallVelocity(0)
    , m_fMaxVelocity(-600)
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
    , m_pRateLayer(NULL)
    , m_CurrentLayer(NULL)
    , m_ToShowOverView(false)
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
    CC_SAFE_RELEASE_NULL(m_pRateLayer);
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
    m_ScoreLabel->setScale(0.85f);
    m_ScoreLabel->setPosition(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height * 0.86125);
    addChild(m_ScoreLabel);

    m_Title = CCSprite::create("title_ready.png");
    m_Title->retain();
    m_Title->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.x + m_VisibleSize.height * 0.725f));
    addChild(m_Title);
    m_Hint = CCSprite::create("hint.png");
    m_Hint->retain();
    m_Hint->setScale(0.76f);
    m_Hint->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height / 2));
    addChild(m_Hint);

    m_pPause = LGMenuItemImage::create("pause.png", NULL, this, menu_selector(GameScene::pauseCallback));
    m_pPause->retain();
    m_pPause->setVisible(false);
    m_pPause->setScale(0.85f);
    m_pPause->setPosition(m_VisibleOrigin.x + 43, m_VisibleOrigin.y + m_VisibleSize.height - 41);
    CCMenu *pMenu = CCMenu::create(m_pPause, NULL);
    pMenu->setZOrder(3);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

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


    if (m_ToShowOverView)
    {
        showOverView();
    }
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
            m_fFallVelocity += -1400 * delta;
            m_fFallVelocity = MAX(m_fFallVelocity, m_fMaxVelocity);
            float posY = m_pPig->getPositionY() +m_fFallVelocity * delta;
            if (posY <= m_VisibleOrigin.y + m_GroundPosY + m_pPig->getContentSize().height / 2)
            {
                posY = m_VisibleOrigin.y + m_GroundPosY + m_pPig->getContentSize().height / 2;
                m_Crashed = true;
            }

            m_pPig->setPositionY(posY);
            float orgRotation = m_pPig->getRotation();
            float rotation = 0;
            if (orgRotation < -24)
            {
                rotation = orgRotation + delta * 10;
            }
            else if (orgRotation < -22)
            {
                rotation = orgRotation + delta * 30;
            }
            else if (orgRotation < -16)
            {
                rotation = orgRotation + delta * 60;
            }
            else
            {
                rotation = orgRotation + delta * 500;
            }

            rotation = MIN(rotation, 80);
            m_pPig->setRotation(rotation);
        }

        if (m_Crashed)
        {
            if (!oldCrashed)
            {
                m_pPig->stopAllActions();

                GameManager *gameManager = GameManager::getInstance();
                gameManager->addGoalToday();
                if (gameManager->getGoalsToday() >= 2 && m_CurrentScore >= 5 && gameManager->getRated() == false && gameManager->getLaterTS() != Util::getCurrentDays())
                {
                    showRateView();
                }
                else
                {
                    showOverView();
                }

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

    if (m_Sound)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SFX_SWOOSHING);
    }

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

void GameScene::rateCallback(CCObject *pSender)
{
    GameManager::getInstance()->saveRated(true);

    NativeBridge::rateApp();

    m_pRateLayer->removeFromParent();

    CC_SAFE_RELEASE_NULL(m_CurrentLayer);
    onShowOverlay(false);

    scheduleOnce(schedule_selector(GameScene::showOverViewSchedule), 2.0f);
}

void GameScene::laterCallback(CCObject *pSender)
{
    GameManager::getInstance()->saveLaterTS(Util::getCurrentDays());

    m_pRateLayer->removeFromParent();

    CC_SAFE_RELEASE_NULL(m_CurrentLayer);
    onShowOverlay(false);

    showOverView();
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
        CCSpawn *action = CCSpawn::createWithTwoActions(raiseEaseOut, CCRotateTo::create(0.1f, -25));
        action->setTag(kTagRaise);

        m_pPig->stopActionByTag(kTagRaise);
        m_pPig->runAction(action);

        m_fFallVelocity = 0;
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
    if ((m_Pillars[0] == NULL && m_PassedTime > 3.5f) || (m_Pillars[0] != NULL && lastPillarPosX + m_VisibleSize.width * 0.5625 <= m_VisibleOrigin.x + m_VisibleSize.width && firstEmptyIndex <= 2))
    {
        int bottomPillar_Height = minPillarHeight + CCRANDOM_0_1() * (m_VisibleSize.height - m_GroundPosY - minPillarHeight - minPillarHeight - m_Gap);
        int bottomPillar_BottomY = m_VisibleOrigin.y + m_GroundPosY;
        int topPillar_BottomY = bottomPillar_BottomY + bottomPillar_Height + m_Gap;
        int topPillar_Height = CCDirector::sharedDirector()->getWinSize().height - topPillar_BottomY + m_VisibleOrigin.y;
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
        //pBgScore->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height * 0.535f));
        pBgScore->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y - pBgScore->getContentSize().height / 2));
        pBgScore->setTag(kTagOverScoreBoard);
        m_pOverLayer->addChild(pBgScore);

        float bgW = pBgScore->getContentSize().width, bgH = pBgScore->getContentSize().height;

        int record = CCUserDefault::sharedUserDefault()->getIntegerForKey("record", 0);
        if (record < m_CurrentScore)
        {
            record = m_CurrentScore;
            CCUserDefault::sharedUserDefault()->setIntegerForKey("record", record);
            NativeBridge::submitScore(record);

            CCSprite *indicatorNew = CCSprite::create("new.png");
            indicatorNew->setPosition(ccp(bgW * (0.5 - 0.2448980f), bgH * (0.5 - 0.08035714f)));
            pBgScore->addChild(indicatorNew);
        }

        CCLabelAtlas *current = CCLabelAtlas::create(CCString::createWithFormat("%d", m_CurrentScore)->getCString(), "fonts/font2.png", 36, 48, '0');
        current->setAnchorPoint(ccp(0.5, 0.5));
        current->setPosition(bgW / 2, bgH * 0.6394f);
        pBgScore->addChild(current);
        CCLabelAtlas *best = CCLabelAtlas::create(CCString::createWithFormat("%d", record)->getCString(), "fonts/font2.png", 36, 48, '0');
        best->setAnchorPoint(ccp(0.5, 0.5));
        best->setPosition(bgW / 2, bgH * (0.5 - 0.27679f));
        pBgScore->addChild(best);

        CCMenuItemImage *pRetry = LGMenuItemImage::create("retry.png", NULL, this, menu_selector(GameScene::retryCallback));
        CCMenuItemImage *pMenu = LGMenuItemImage::create("menu.png", NULL, this, menu_selector(GameScene::menuCallback));

        pRetry->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 0.73333333f, m_VisibleOrigin.y + m_VisibleSize.height * 0.285f));
        pMenu->setPosition(ccp(m_VisibleOrigin.x + m_VisibleSize.width * 0.26666667f, pRetry->getPositionY()));

        CCMenu *pCCMenu = CCMenu::create(pRetry, pMenu, NULL);
        pCCMenu->setTag(kTagOverMenu);
        pCCMenu->setAnchorPoint(CCPointZero);
        pCCMenu->setPosition(ccp(0, m_VisibleOrigin.y - (pRetry->getPositionY() - m_VisibleOrigin.y) * 2));
        m_pOverLayer->addChild(pCCMenu);
    }

    m_pOverLayer->getChildByTag(kTagOverScoreBoard)->runAction(CCMoveTo::create(0.5f, ccp(m_VisibleOrigin.x + m_VisibleSize.width / 2, m_VisibleOrigin.y + m_VisibleSize.height * 0.535f)));
    m_pOverLayer->getChildByTag(kTagOverMenu)->runAction(CCMoveTo::create(1.0f, CCPointZero));

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

void GameScene::showRateView()
{
    if (m_pRateLayer == NULL)
    {
        m_pRateLayer = LGLayerColor::create(ccc4(0, 0, 0, 0));
        m_pRateLayer->retain();
        m_pRateLayer->setTouchMode(kCCTouchesOneByOne);
        m_pRateLayer->setTouchEnabled(true);
        m_pRateLayer->setZOrder(3);

        CCSprite *pBg = CCSprite::create("rate_bg.png");
        pBg->setPosition(m_VisibleOrigin + m_VisibleSize / 2);
        m_pRateLayer->addChild(pBg);

        float bgW = pBg->getContentSize().width, bgH = pBg->getContentSize().height;
        CCMenuItemImage *pRate = LGMenuItemImage::create("rate_rate.png", NULL, this, menu_selector(GameScene::rateCallback));
        pRate->setPosition(ccp(pBg->getPositionX() - 0.2592593f * bgW, pBg->getPositionY() - 0.6566f * bgH));

        CCMenuItemImage *pLater = LGMenuItemImage::create("rate_later.png", NULL, this, menu_selector(GameScene::laterCallback));
        pLater->setPosition(ccp(pBg->getPositionX() + 0.2592593f * bgW, pRate->getPositionY()));

        CCMenu *pMenu = CCMenu::create(pRate, pLater, NULL);
        pMenu->setAnchorPoint(CCPointZero);
        pMenu->setPosition(CCPointZero);
        m_pRateLayer->addChild(pMenu);
    }

    if (m_pRateLayer && m_pRateLayer->getParent() == NULL)
    {
        getParent()->addChild(m_pRateLayer);
        CC_SAFE_RELEASE_NULL(m_CurrentLayer);
        m_CurrentLayer = m_pRateLayer;
        m_CurrentLayer->retain();
        onShowOverlay(true);
    }
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

void GameScene::showOverViewSchedule(float dt)
{
    showOverView();
}