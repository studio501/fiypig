#ifndef _NATIVE_BRIDGE_H
#define _NATIVE_BRIDGE_H

#include "cocos2d.h"

class NativeBridge
{
public:
    static int getAdMobBannerHeight();
    static void setAdMobBannerPosition(bool top);
    static void showAdMobBanner(bool show);
    static void onMoreClicked();
    static void showAdMobMRect(bool show);
    static void showInterstitial();
    static void showAdMobInterstitial();
    static void rateApp();
    static void submitScore(int score);
    static void showLeaderboard();
};

#endif /*_LAYOUT_MANAGER_H*/
