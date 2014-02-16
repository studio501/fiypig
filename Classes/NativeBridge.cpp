#include "NativeBridge.h"
#include "GameScene.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "IOSPlatformCallback.h"
#endif

#define CLASS_NAME "com/lichmobile/flypig/MainActivity"

void NativeBridge::onMoreClicked()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "onMoreClicked", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, NULL);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_onMoreClicked();
#endif
}

int NativeBridge::getAdMobBannerHeight()
{
    int height = 0;
    cocos2d::CCSize frameSize = cocos2d::CCEGLView::sharedOpenGLView()->getFrameSize();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "getAdMobBannerHeight", "()I"))
    {
        height = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID, NULL);

    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    height = ios_getAdMobBannerHeight();
#endif
    height = height * cocos2d::CCDirector::sharedDirector()->getVisibleSize().height / frameSize.height;

    return height;
}

void NativeBridge::setAdMobBannerPosition(bool top)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "setAdMobBannerPosition", "(Z)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, top);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_setAdMobBannerPosition(top);
#endif
}

void NativeBridge::showAdMobBanner(bool show)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showAdMobBanner", "(Z)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, show);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_showAdMobBanner(show);
#endif
}

void NativeBridge::showAdMobMRect(bool show)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showAdMobMRect", "(Z)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, show);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_showAdMobMRect(show);
#endif
}

void NativeBridge::showInterstitial()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showInterstitial", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, NULL);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_showInterstitial();
#endif
}

void NativeBridge::rateApp()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "rateApp", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, NULL);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_rateApp();
#endif
}

void NativeBridge::submitScore(int score)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "submitScore", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, score);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_submitScore(score);
#endif
}

void NativeBridge::showLeaderboard()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showLeaderboard", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, NULL);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ios_showLeaderboard();
#endif
}