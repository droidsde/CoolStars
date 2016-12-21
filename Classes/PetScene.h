#ifndef __PETSCENE_H__
#define __PETSCENE_H__ 
#include "cocos2d.h"
#include "BasePanel.h"
#include <unordered_map>
#include "PetSceneMoveHelper.h"
#include "PetManager.h"
class UiLayout;

enum PetSceneUsage
{
	kPetSceneFromMenuScene,
	kPetSceneFromLotteryScene,
	kPetSceneFromStageScene,
};

class PetScene 
	: public BasePanel
	, public IPetView
	, public cocos2d::CCTouchDelegate
{
public:
	static PetScene *create(int usage);
	virtual bool init();
private:
	PetScene(int usage);
	~PetScene(){}
	virtual void onEnter();
	virtual void onExit();
	void initMainLayout();
	void initBottomLayout();
	void initColorPets();
	void refreshUi();
	void refreshPetCost();
	void refreshArrows();
	void changePetsColor(int color);
	int parsePetType(int petId);
	void refrshRedPoint();
	void addRedPoint(cocos2d::CCNode *target);
private:
	void onLeftPetBtnClicked(cocos2d::CCObject* pSender);
	void onRigthPetBtnClicked(cocos2d::CCObject* pSender);
	void onUpgradeBtnClicked(cocos2d::CCObject* pSender);
	void onBuyBtnClicked(cocos2d::CCObject* pSender);
	void onOpenUpgradeTipsBtnClicked(cocos2d::CCObject* pSender);

	void onBackBtnClicked(cocos2d::CCObject* pSender);
	void onGreenPetBtnClicked(cocos2d::CCObject* pSender);
	void onPurplePetBtnClicked(cocos2d::CCObject* pSender);
	void onRedPetBtnClicked(cocos2d::CCObject* pSender);
	void onBluePetBtnClicked(cocos2d::CCObject* pSender);
	void onYellowPetBtnClicked(cocos2d::CCObject* pSender);
	void handleColorBtnClicked(int color);

	virtual void onBackKeyTouched();
	void runUpgradeGuide();
	void toGuidePage();
private:
	virtual void onNewPetAdd(int petId);
	virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
private:
	enum
	{
		kPetForGuide,
		kPetForDiamondPurchase,
		kPetAlreadyOwned,
		kPetForRmbPurchase,
	};
private:
	UiLayout *m_bottomLayout;
	UiLayout *m_mainLayout;
	UiLayout *m_emptyPetLayout;
	PetSceneMoveHelper m_moveHelper;
	static const float kBtnSelectedScale;
	std::unordered_map<int, std::vector<int>>m_colorPets;
	static int s_curPetColor;
	int m_curColorPetIndex;
	cocos2d::CCNode *m_redPointLayer;
};
#endif