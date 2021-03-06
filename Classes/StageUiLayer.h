#ifndef __STAGEUILAYER_H___
#define __STAGEUILAYER_H___ 

#include "cocos2d.h"
#include "StarsController.h"
#include "StageLayersMgr.h"
#include "StageDataMgr.h"
#include <unordered_map>
#include "RedPackageModule.h"
#include "DataConfig.h"
USING_NS_CC;

class StagePetNode;
class UiLayout;
class StageStateOwner;
class NoTouchLayer;
class StageUiLayer
    : public cocos2d::CCNode
	, public IStageDataView
	, public IStarsControlView
	, public IStageLayer
	, public cocos2d::CCTouchDelegate
{
public:
	StageUiLayer();
    ~StageUiLayer(void);

	static StageUiLayer *create();
    virtual bool init();
	void initTopUi();
	void initPets();
	void initBottomUi();
	void showTargetPanel();
	std::unordered_map<int, cocos2d::CCPoint> getPetViewsInfo();
	void showPetSpreadStarsAction(int petId, const StarAttr &attr, std::function<void()> callback);
	void gameOverSpreadStars(const GoodsData &data, const LogicGrid &targetGrid, std::function<void()> callback);
public:
	virtual void onStepsChanged();
	virtual void onScoreChanged();
	virtual void onCoinsChanged();
	virtual void onShowGameResult(int isWon);
	virtual void onNormalStarErased(cocos2d::CCPoint pos, int starType, int color);
	virtual void onHighLightPets(const std::vector<int> &petIds);
	virtual void onToNormalState();
	virtual void onRunOutSteps();
	virtual void onGameWin();
	virtual void onScoreBouble();
	virtual void onTouchEnable(bool canTouch);
	virtual void onEraseStarsStart();
	virtual void onEraseStarsEnd();
public:
	void showChangeColorPanel(int myColor, const LogicGrid &grid);
private:
	virtual void onEnter();
	virtual void onExit();
	void handlePetClicked(int petId);
	void handlePropsItemClicked(int type);
	void showGameOverHint();
	void showPetsSkillPanel();
	void refreshRedPackage();

	void onPauseBtnClicked(CCObject *pSender);
	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	void playExplosionAction(cocos2d::CCPoint pos);
	void removeExplosionAnimation(cocos2d::extension::CCArmature *, cocos2d::extension::MovementEventType, const char *);
private:

private:
	static const int kZorder_Pet = 10;
	static const int kZorder_Dialog = 20;
	UiLayout *m_bottomUi;
	UiLayout *m_topUi;
	StageStateOwner *m_stateOwner;
	cocos2d::CCProgressTimer *m_scoreProgress;
	std::unordered_map<int , StagePetNode *>m_petViews;
	RedPackageModule m_redPackage;
	NoTouchLayer *m_noTouchLayer;
};
#endif // __PANELLLAYER_H__