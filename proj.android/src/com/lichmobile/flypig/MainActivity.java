/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.lichmobile.flypig;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.chartboost.sdk.Chartboost;
import com.chartboost.sdk.ChartboostDelegate;
import com.flurry.android.FlurryAgent;
import com.google.ads.AdRequest;
import com.google.ads.AdSize;
import com.google.ads.AdView;
import com.google.android.gms.appstate.AppStateClient;
import com.google.android.gms.games.GamesClient;
import com.google.android.gms.plus.PlusClient;

public class MainActivity extends Cocos2dxActivity implements GameHelper.GameHelperListener {
    private static final String ADMOB_ID = "a152fcc3d561c57";
    private static final String CHARTBOOST_APPID = "52fb04702d42da57bc7e8b95";
    private static final String CHARTBOOST_APPSIGNATURE = "1999c1fc640280a0e033219cc04010db536d85a8";
    private static final String FLURRY_ID = "FFMSX5CR5TMDCGDWQF6C";

    private static MainActivity sInstance;

    protected Chartboost mChartboost;
    protected AdView mAdMobBanner;
    protected AdView mAdMobMRect;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addChartboostAds();
        addAdMob();

        mHelper = new GameHelper(this);
        if (mDebugLog) {
            mHelper.enableDebugLog(mDebugLog, mDebugTag);
        }
        mHelper.setup(this, mRequestedClients, mAdditionalScopes);
    }

    public Cocos2dxGLSurfaceView onCreateView() {
        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);

        return glSurfaceView;
    }

    private void addChartboostAds() {
        mChartboost = Chartboost.sharedChartboost();
        mChartboost.onCreate(this, CHARTBOOST_APPID, CHARTBOOST_APPSIGNATURE, new ChartboostDelegate() {
            @Override
            public boolean shouldRequestMoreApps() {
                return true;
            }

            @Override
            public boolean shouldRequestInterstitialsInFirstSession() {
                return true;
            }

            @Override
            public boolean shouldRequestInterstitial(String arg0) {
                return true;
            }

            @Override
            public boolean shouldDisplayMoreApps() {
                return true;
            }

            @Override
            public boolean shouldDisplayLoadingViewForMoreApps() {
                return true;
            }

            @Override
            public boolean shouldDisplayInterstitial(String arg0) {
                return true;
            }

            @Override
            public void didShowMoreApps() {
            }

            @Override
            public void didShowInterstitial(String arg0) {
            }

            @Override
            public void didFailToLoadUrl(String arg0) {
            }

            @Override
            public void didFailToLoadMoreApps() {
            }

            @Override
            public void didFailToLoadInterstitial(String arg0) {
            }

            @Override
            public void didDismissMoreApps() {
            }

            @Override
            public void didDismissInterstitial(String arg0) {
            }

            @Override
            public void didCloseMoreApps() {
            }

            @Override
            public void didCloseInterstitial(String arg0) {
            }

            @Override
            public void didClickMoreApps() {
            }

            @Override
            public void didClickInterstitial(String arg0) {
            }

            @Override
            public void didCacheMoreApps() {
            }

            @Override
            public void didCacheInterstitial(String arg0) {
            }
        });
        mChartboost.setTimeout(5000);
        mChartboost.startSession();
        mChartboost.showInterstitial();
        mChartboost.cacheMoreApps();
    }

    private void addAdMob() {
        try {
            FrameLayout.LayoutParams adParams = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
            adParams.gravity = Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
            mAdMobBanner = new AdView(this, AdSize.SMART_BANNER, ADMOB_ID);
            mAdMobBanner.loadAd(new AdRequest());
            // mAdMobBanner = new AdView(this);
            // mAdMobBanner.setAdSize(AdSize.SMART_BANNER);
            // mAdMobBanner.setAdUnitId(ADMOB_ID);
            // mAdMobBanner.loadAd(new AdRequest.Builder().build());
            addContentView(mAdMobBanner, adParams);

            adParams = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
            adParams.gravity = Gravity.CENTER;
            mAdMobMRect = new AdView(this, AdSize.IAB_MRECT, ADMOB_ID);
            mAdMobMRect.setVisibility(View.GONE);
            mAdMobMRect.loadAd(new AdRequest());
            // mAdMobMRect = new AdView(this);
            // mAdMobMRect.setAdSize(AdSize.MEDIUM_RECTANGLE);
            // mAdMobMRect.setAdUnitId(ADMOB_ID);
            // mAdMobMRect.setVisibility(View.GONE);
            // mAdMobMRect.loadAd(new AdRequest.Builder().build());
            addContentView(mAdMobMRect, adParams);
        } catch (Exception e) {
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

        mChartboost.onStart(this);
        FlurryAgent.onStartSession(getApplicationContext(), FLURRY_ID);

        mHelper.onStart(this);
    }

    @Override
    protected void onResume() {
        super.onResume();

        sInstance = this;

        Cocos2dxHelper.sCocos2dxActivity = this;
    }

    @Override
    protected void onPause() {
        Cocos2dxHelper.sCocos2dxActivity = null;

        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();

        mChartboost.onStop(this);
        FlurryAgent.onEndSession(getApplicationContext());

        mHelper.onStop();
    }

    @Override
    protected void onDestroy() {
        ViewGroup rootView = (ViewGroup) findViewById(android.R.id.content);
        rootView.removeView(mAdMobBanner);
        mAdMobBanner.removeAllViews();
        mAdMobBanner.destroy();

        rootView.removeView(mAdMobMRect);
        mAdMobMRect.removeAllViews();
        mAdMobMRect.destroy();

        super.onDestroy();

        mChartboost.onDestroy(this);
    }

    @Override
    protected void onActivityResult(int request, int response, Intent data) {
        super.onActivityResult(request, response, data);
        mHelper.onActivityResult(request, response, data);
    }

    @Override
    public void onBackPressed() {
        if (mChartboost.onBackPressed())
            // If a Chartboost view exists, close it and return
            return;
        else
            // If no Chartboost view exists, continue on as normal
            super.onBackPressed();
    }

    public static void onMoreClicked() {
        sInstance.mChartboost.showMoreApps();
    }

    public static int getAdMobBannerHeight() {
        DisplayMetrics dm = new DisplayMetrics();
        sInstance.getWindowManager().getDefaultDisplay().getMetrics(dm);
        return (int) (dm.ydpi * 50 / 160);
    }

    public static void setAdMobBannerPosition(final boolean top) {
        sInstance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) sInstance.mAdMobBanner.getLayoutParams();
                params.gravity = (top ? Gravity.TOP : Gravity.BOTTOM) | Gravity.CENTER_HORIZONTAL;
                sInstance.mAdMobBanner.setLayoutParams(params);
            }
        });
    }

    public static void showAdMobBanner(final boolean show) {
        sInstance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                sInstance.mAdMobBanner.setVisibility(show ? View.VISIBLE : View.GONE);
            }
        });
    }

    public static void showAdMobMRect(final boolean show) {
        sInstance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                sInstance.mAdMobMRect.setVisibility(show ? View.VISIBLE : View.GONE);
            }
        });
    }

    public static void showInterstitial() {
        sInstance.runOnUiThread(new Runnable() {
            public void run() {
                sInstance.mChartboost.showInterstitial();
            }
        });
    }

    public static void rateApp() {
        sInstance.runOnUiThread(new Runnable() {
            public void run() {
                Intent intent = new Intent(Intent.ACTION_VIEW);
                String referrer = "market://details?id=" + sInstance.getPackageName();
                intent.setData(Uri.parse(referrer));

                try {
                    sInstance.startActivity(intent);
                } catch (Exception e) {
                    Toast toast = Toast.makeText(sInstance, "Market Not Work", Toast.LENGTH_LONG);
                    toast.show();
                }
            }
        });
    }

    public static void submitScore(final int score) {
        sInstance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!sInstance.isSignedIn()) {
                    if (sInstance.mHighScore < score) {
                        sInstance.mHighScore = score;
                    }
                } else {
                    sInstance.getGamesClient().submitScore(sInstance.getString(R.string.highscore), score);
                }
            }
        });
    }

    private boolean mToShowLeaderboard = false;

    public static void showLeaderboard() {
        sInstance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!sInstance.isSignedIn()) {
                    sInstance.mToShowLeaderboard = true;
                    sInstance.beginUserInitiatedSignIn();
                } else {
                    sInstance.startActivityForResult(sInstance.getGamesClient().getLeaderboardIntent(sInstance.getString(R.string.highscore)), 0);
                }
            }
        });
    }

    static {
        System.loadLibrary("flypig");
    }

    // The game helper object. This class is mainly a wrapper around this object.
    protected GameHelper mHelper;
    protected String mDebugTag = "MainActivity";
    protected boolean mDebugLog = false;
    // We expose these constants here because we don't want users of this class
    // to have to know about GameHelper at all.

    public static final int CLIENT_GAMES = GameHelper.CLIENT_GAMES;
    public static final int CLIENT_APPSTATE = GameHelper.CLIENT_APPSTATE;
    public static final int CLIENT_PLUS = GameHelper.CLIENT_PLUS;
    public static final int CLIENT_ALL = GameHelper.CLIENT_ALL;

    // Requested clients. By default, that's just the games client.
    protected int mRequestedClients = CLIENT_GAMES;

    // stores any additional scopes.
    private String[] mAdditionalScopes;

    protected GamesClient getGamesClient() {
        return mHelper.getGamesClient();
    }

    protected AppStateClient getAppStateClient() {
        return mHelper.getAppStateClient();
    }

    protected PlusClient getPlusClient() {
        return mHelper.getPlusClient();
    }

    protected boolean isSignedIn() {
        return mHelper.isSignedIn();
    }

    protected void beginUserInitiatedSignIn() {
        mHelper.beginUserInitiatedSignIn();
    }

    protected void signOut() {
        mHelper.signOut();
    }

    protected void showAlert(String title, String message) {
        mHelper.showAlert(title, message);
    }

    protected void showAlert(String message) {
        mHelper.showAlert(message);
    }

    protected void enableDebugLog(boolean enabled, String tag) {
        mDebugLog = true;
        mDebugTag = tag;
        if (mHelper != null) {
            mHelper.enableDebugLog(enabled, tag);
        }
    }

    protected String getInvitationId() {
        return mHelper.getInvitationId();
    }

    protected void reconnectClients(int whichClients) {
        mHelper.reconnectClients(whichClients);
    }

    protected String getScopes() {
        return mHelper.getScopes();
    }

    protected String[] getScopesArray() {
        return mHelper.getScopesArray();
    }

    protected boolean hasSignInError() {
        return mHelper.hasSignInError();
    }

    protected GameHelper.SignInFailureReason getSignInError() {
        return mHelper.getSignInError();
    }

    @Override
    public void onSignInFailed() {
    }

    private int mHighScore = -1;

    @Override
    public void onSignInSucceeded() {
        // if we have accomplishments to push, push them
        if (mHighScore >= 0) {
            getGamesClient().submitScore(getString(R.string.highscore), mHighScore);
        }
        if (mToShowLeaderboard) {
            startActivityForResult(getGamesClient().getLeaderboardIntent(getString(R.string.highscore)), 0);
            mToShowLeaderboard = false;
        }
    }
}
