#include "LotteryScene.h"
#include "UiLayout.h"
#include "TitlePanel.h"
#include "ListView.h"
#include "EmptyBox.h"
#include "CommonMacros.h"
#include "MainScene.h"
#include "LotteryModel.h"
#include "CommonUtil.h"
#include "UserInfo.h"
#include "DataManager.h"
#include "PetManager.h"
#include "CCFunctionAction.h"
#include "StarsController.h"
#include "ThiefModel.h"
#include "GuideMgr.h"
#include "SoundMgr.h"
#include "PackageDialog.h"
#include "PackageModel.h"
#include "MyPurchase.h"
#include "PetScene.h"
#include "ActionRunner.h"
#include "NoTouchLayer.h"
#include "GameBackEndState.h"

USING_NS_CC;
using namespace std;

LotteryNode::LotteryNode(int touchPriority, LotteryScene *panel)
: TouchNode(touchPriority)
, m_panel(panel)
, m_isOpened(false)
{
	m_handle = function<void()>();
}

LotteryNode *LotteryNode::create(int touchPriority, LotteryScene *panel)
{
	auto node = new LotteryNode(touchPriority, panel);
	node->init();
	node->autorelease();
	return node;
}

bool LotteryNode::init()
{
	m_layout = UiLayout::create("layout/lottery_node.xml");
	addChild(m_layout);
	setContentSize(m_layout->getContentSize());

	m_layout->getChildById(2)->setVisible(false);
	return true;
}

bool LotteryNode::onTouchBegan(cocos2d::CCPoint pt, bool isInside)
{
	if (isInside && !m_isOpened)
	{
		if (!LotteryModel::theModel()->canOpenOneBox())
		{
			MainScene::theScene()->showDialog(PackageDialog::create(kPackageDiamond));
			MyPurchase::sharedPurchase()->showToast(kToastTextNotEnoughDiamond);
		}
		else
		{
			auto data = LotteryModel::theModel()->buyOneBox();
			openBox(data);
		}
	}
	return isInside;
}

void LotteryNode::openBox(const LotteryData data)
{
	LotteryModel::theModel()->getReward(data);
	m_panel->startOpenBox();

	m_isOpened = true;
	auto pos = getParent()->convertToWorldSpace(getPosition());
	auto size = getContentSize();
	pos = ccpAdd(pos, ccpMult(size, 0.5f));
	m_panel->runKeyMoveAction(pos, bind(&LotteryNode::showReward, this, data));
	GuideMgr::theMgr()->endGuide(kGuideEnd_lottery_open_box);
}

void LotteryNode::showReward(const LotteryData data)
{
	SoundMgr::theMgr()->playEffect(kEffectMusicButton);
	m_layout->getChildById(1)->setVisible(false);
	m_layout->getChildById(2)->setVisible(true);

	string resPath;
	int goodsNum = 0;
	if (data.type == kLotteryPet)
	{
		auto pet = PetManager::petMgr()->getPetById(data.param);
		assert(pet && "this pet is not opening");
		resPath = pet->getPetData().petImgRes;
		goodsNum = 1;
	}
	else
	{
		auto config = DataManagerSelf->getLotteryOutputConfig(data.type);
		resPath = config.resPath;
		goodsNum = data.param;
	}
	m_goodsLayout = UiLayout::create("layout/goods_view.xml");
	auto size = getContentSize();
	m_goodsLayout->setPosition(ccpMult(size, 0.5f));
	m_goodsLayout->setAnchorPoint(ccp(0.5f, 0.5f));
	addChild(m_goodsLayout);

	CCSprite *goods = dynamic_cast<CCSprite *>(m_goodsLayout->getChildById(1));
	goods->initWithFile(resPath.c_str());
	if (data.type == kLotteryPet)
	{
		auto scaleSmall = CCScaleTo::create(0.3f, 2.0f);
		auto scaleNormal = CCScaleTo::create(0.5f, 1.0f);
		auto seq = CCSequence::create(scaleSmall, scaleNormal, NULL);
		goods->runAction(seq);
	}
	m_goodsLayout->getChildById(2)->setVisible(false);
	
	m_goodsLayout->runAction(getRewardOutAction(goodsNum));
	//LotteryModel::theModel()->getReward(data);
	//m_panel->startOpenBox();
	auto func = CCFunctionAction::create([=]
	{
		m_panel->endOpenBox();
	});
	int delayTime = data.type == kLotteryPet ? 1.0f : 0.5f;
	runAction(CCSequence::create(CCDelayTime::create(delayTime), func, NULL));
}

CCAction *LotteryNode::getRewardOutAction(int num)
{
	CCMoveBy *moveBy = CCMoveBy::create(0.5f, ccp(0, 30));
	auto func = CCFunctionAction::create([=]()
	{
		CCLabelAtlas *numLabel = dynamic_cast<CCLabelAtlas *>(m_goodsLayout->getChildById(2));
		numLabel->setString(CommonUtil::intToStr(num));
		numLabel->setVisible(true);

		m_handle();
	});
	return CCSequence::create(CCEaseBackInOut::create(moveBy), func, NULL);
}
////////////////////////////////////////////////////////////////
LotteryScene *LotteryScene::create(int usage)
{
	LotteryScene *scene = new LotteryScene(usage);
	scene->init();
	scene->autorelease();
	return scene;
}

LotteryScene::LotteryScene(int usage)
: m_openedBoxNum(0)
, m_curOpenedNum(0)
, m_isOpeningAllBox(false)
{
	setUsage(usage);
	m_runner = ActionRunner::create();
	m_runner->retain();
}

LotteryScene::~LotteryScene()
{
	m_runner->clear();
	m_runner->release();
}

void LotteryScene::onEnter()
{
	BasePanel::onEnter();
	UserInfo::theInfo()->addView(this);
}

void LotteryScene::onExit()
{
	BasePanel::onExit();
	UserInfo::theInfo()->removeView(this);
}

bool LotteryScene::init()
{
	setPanelId(kLotteryPanel);

	auto winSize = CCDirector::sharedDirector()->getWinSize();
	setContentSize(winSize);

	m_titlePanel = TitlePanel::create(m_touchPriority);
	m_titlePanel->setTopThief(kThiefLotteryPanel);
	addChild(m_titlePanel, 1);

	m_bottomLayout = UiLayout::create("layout/pre_stage_bottom.xml");
	addChild(m_bottomLayout, 1);
	m_bottomLayout->setMenuTouchPriority(m_touchPriority);
	initBottomLayout();

	initPanel();

	m_noTouchLayer = NoTouchLayer::create();
	addChild(m_noTouchLayer);
	GuideMgr::theMgr()->startGuide(kGuideStart_lottery_in, bind(&LotteryScene::hideBottomBtns, this));
	if (m_usage == kLotterySceneFromPetScene)
	{
		GuideMgr::theMgr()->startGuide(kGuideStart_lottery_in_by_petScene);
	}
	return true;
}

void LotteryScene::initPanel()
{
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	m_layout = UiLayout::create("layout/lottery_panel.xml");
	m_layout->setMenuTouchPriority(m_touchPriority);
	m_layout->setAnchorPoint(ccp(0.5f, 0.5f));
	auto sourcePos = ccp(winSize.width * 0.5f, winSize.height * 1.5f);
	auto targetPos = ccpMult(winSize, 0.5f);
	m_layout->setPosition(sourcePos);
	auto func = CCFunctionAction::create([=]()
	{
		GuideMgr::theMgr()->startGuide(kGuideStart_lottery_panel_move_done);
	});
	m_layout->runAction(CCSequence::create(CCEaseBackInOut::create(CCMoveTo::create(0.5f, targetPos)), func, NULL));
	addChild(m_layout);
	initLayout();
	refreshUi();
}

void LotteryScene::initLayout()
{
	CCMenuItem * toPetSceneBtn = dynamic_cast<CCMenuItem *>((m_layout->getChildById(16)));
	toPetSceneBtn->setTarget(this, menu_selector(LotteryScene::toPetScene));

	bool isBusiness = GameBackEndState::theModel()->isBusinessMode();
	CCMenuItem * openAllBtn = dynamic_cast<CCMenuItem *>((m_layout->getChildById(isBusiness ? 21 : 23)));
	openAllBtn->setTarget(this, menu_selector(LotteryScene::onOpenAllBoxesBtnClicked));
	m_layout->getChildById(21)->setVisible(isBusiness);
	m_layout->getChildById(23)->setVisible(!isBusiness);
	
	int boxIds[] = { 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	for (int i = 0; i < 9; ++i)
	{
		EmptyBox *box = dynamic_cast<EmptyBox *>((m_layout->getChildById(boxIds[i])));
		auto node = LotteryNode::create(m_touchPriority, this);
		node->setHandle(bind(&LotteryScene::onOpenReward, this));
		box->setNode(node);
	}
	
	int cost = DataManagerSelf->getSystemConfig().diamondsForOneKey;
	CCLabelAtlas *costDiamond = dynamic_cast<CCLabelAtlas *>(m_layout->getChildById(19));
	costDiamond->setString(CommonUtil::intToStr(cost));

	bool hasPetToUpgrade = PetManager::petMgr()->hasPetToUpgrade();
	m_layout->getChildById(20)->setZOrder(2);
	m_layout->getChildById(20)->setVisible(hasPetToUpgrade);

	if (!isBusiness)
	{
		auto costNum = dynamic_cast<CCSprite *>(m_layout->getChildById(22));
		costNum->initWithFile("lottery/pic_9.png");
	}
	onKeyChanged();
}


void LotteryScene::initBottomLayout()
{
	CCMenuItem *startGameBtn = dynamic_cast<CCMenuItem *>((m_bottomLayout->getChildById(1)));
	startGameBtn->setTarget(this, menu_selector(LotteryScene::onStartBtnClicked));

	auto scaleSmall = CCScaleTo::create(0.6f, 0.8f);
	auto scaleNormal = CCScaleTo::create(0.4f, 1.0f);
	auto scale = CCRepeatForever::create(CCSequence::create(scaleSmall, scaleNormal, NULL));
	startGameBtn->runAction(scale);

	m_bottomLayout->getChildById(2)->setVisible(false);
}

void LotteryScene::refreshUi()
{
	auto startGameBtn = m_bottomLayout->getChildById(1);
	if (m_usage == kLotterySceneFromStageScene)
	{
		int key = UserInfo::theInfo()->getKey();
		bool isShowUp = m_openedBoxNum >= kMinOpenBoxNum || key <= 0;
		m_layout->getChildById(16)->setVisible(isShowUp);
		startGameBtn->setVisible(isShowUp);
		m_titlePanel->setUiVisible(kTitlePanelBackHome, isShowUp);

		bool hasPetToUpgrade = PetManager::petMgr()->hasPetToUpgrade();
		m_layout->getChildById(20)->setVisible(hasPetToUpgrade && isShowUp);

		bool isBusiness = GameBackEndState::theModel()->isBusinessMode();
		m_layout->getChildById(23)->setVisible(!isBusiness && isShowUp);

	}
	else if (m_usage == kLotterySceneFromMenuScene)
	{
		m_bottomLayout->getChildById(1)->setVisible(false);
	}

	int key = UserInfo::theInfo()->getKey();
	bool hasEnoughKey = key > 0;
	m_layout->getChildById(3)->setVisible(hasEnoughKey);
	m_layout->getChildById(4)->setVisible(hasEnoughKey);
	m_layout->getChildById(17)->setVisible(hasEnoughKey);
	m_layout->getChildById(18)->setVisible(!hasEnoughKey);
	m_layout->getChildById(19)->setVisible(!hasEnoughKey);
}

void LotteryScene::toPetScene(CCObject* pSender)
{
	SoundMgr::theMgr()->playEffect(kEffectMusicButton);
	GuideMgr::theMgr()->endGuide(kGuideEnd_lottery_click_pet_btn);
	MainScene::theScene()->showPanel(kPetPanel, kPetSceneFromLotteryScene);
}

void LotteryScene::onOpenAllBoxesBtnClicked(cocos2d::CCObject* pSender)
{
	LotteryModel::theModel()->buyAllBox(bind(&LotteryScene::openAllBoxs, this, placeholders::_1));
}

void LotteryScene::openAllBoxs(std::vector<LotteryData> datas)
{
	m_runner->clear();
	SoundMgr::theMgr()->playEffect(kEffectMusicButton);

	m_noTouchLayer->setCanTouch(false);
	m_isOpeningAllBox = true;

	if (m_curOpenedNum != 0)
	{
		newRewardBoxs(false);
		m_runner->queueAction(DelayAction::withDelay(3.0f));
	}

	int boxIds[] = { 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	for (int i = 0; i < 9; ++i)
	{
		int id = boxIds[i];
	//remove
		m_runner->queueAction(CallFuncAction::withFunctor([=]()
		{
			auto box = dynamic_cast<EmptyBox *>(m_layout->getChildById(id));
			auto node = dynamic_cast<LotteryNode *>(box->getNode());
			node->openBox(datas[i]);
		}));
		m_runner->queueAction(DelayAction::withDelay(0.5f));
	}

	m_runner->queueAction(CallFuncAction::withFunctor([=]()
	{
		m_isOpeningAllBox = false;
	}));
}

void LotteryScene::onStartBtnClicked(CCObject* pSender)
{
	GuideMgr::theMgr()->endGuide(kGuideEnd_lottery_click_start_game_btn);
	SoundMgr::theMgr()->playEffect(kEffectMusicButton);
	MainScene::theScene()->showPanel(kPreStagePanel);
}

void LotteryScene::onOpenReward()
{
	m_openedBoxNum++;
	m_curOpenedNum++;
	GuideMgr::theMgr()->startGuide(kGuideStart_lottery_open_box, function<void()>(), m_openedBoxNum);

	if (m_curOpenedNum >= kRewardBoxNum)
	{
		newRewardBoxs(true);
	}
	refreshUi();
}

void LotteryScene::newRewardBoxs(bool enableTouch)
{
	m_curOpenedNum = 0;
	const float kMoveTime = 0.5f;
	auto moveUpFunc = CCFunctionAction::create([=]()
	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		auto targetPos = ccp(winSize.width * 0.5f, winSize.height * 1.5f);
		m_layout->runAction(CCEaseBackInOut::create(CCMoveTo::create(kMoveTime, targetPos)));
	});

	auto removeFunc = CCFunctionAction::create([=]()
	{
		removeChild(m_layout, true);
		stopAllActions();
		initPanel();
		m_noTouchLayer->setCanTouch(enableTouch);
	});
	runAction(CCSequence::create(CCDelayTime::create(0.5f), moveUpFunc, CCDelayTime::create(kMoveTime), removeFunc, NULL));
}

void LotteryScene::onKeyChanged()
{
	CCLabelAtlas * keyNum = dynamic_cast<CCLabelAtlas *>((m_layout->getChildById(17)));
	int amount = UserInfo::theInfo()->getKey();
	char str[100] = { 0 };
	sprintf(str, ":%d", amount);
	keyNum->setString(str);
}

void LotteryScene::hideBottomBtns()
{
	m_layout->getChildById(16)->setVisible(false);
	m_titlePanel->setUiVisible(kTitlePanelBackHome, false);
	m_bottomLayout->getChildById(1)->setVisible(false);
}

void LotteryScene::runKeyMoveAction(cocos2d::CCPoint target, std::function<void()>callback)
{
	auto sourcePos = convertToNodeSpace(m_layout->convertToWorldSpace(m_layout->getChildById(4)->getPosition()));
	auto targetPos = convertToNodeSpace(target);
	auto spr = CCSprite::create("lottery/cjjm_yaoshi.png");
	m_layout->addChild(spr);
	spr->setPosition(sourcePos);
	auto move = CCEaseExponentialInOut::create(CCMoveTo::create(0.3f, targetPos));
	auto func = CCFunctionAction::create([=]()
	{
		spr->removeFromParent();
		callback();
	});
	spr->runAction(CCSequence::create(move, func, NULL));

}

void LotteryScene::startOpenBox()
{
	m_noTouchLayer->setCanTouch(false);
}

void LotteryScene::endOpenBox()
{
	if (!m_isOpeningAllBox)
	{
		m_noTouchLayer->setCanTouch(true);
	}
}