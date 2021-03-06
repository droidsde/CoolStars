#include "MainScene.h"
#include "BackgroundLayer.h"
#include "TitlePanel.h"
#include "BasePanel.h"
#include "CommonMacros.h"
#include "MenuScene.h"
#include "PetScene.h"
#include "ShopScene.h"
#include "LotteryScene.h"
#include "RankingScene.h"
#include "PreStageScene.h"
#include "StageScene.h"
#include "ScaleDialog.h"
#include "GameResultLayer.h"
#include "GuideView.h"
#include "GuideEditLayer.h"
#include "CommonUtil.h"
#include "UserInfo.h"
#include "RankingPanelUtil.h"
#include "KeyPadWatcher.h"
#include "PackageDialog.h"

USING_NS_CC;
using namespace std;

MainScene *MainScene::s_scene = NULL;

bool MainScene::init()
{
	CCScene::init();
	//监听手机按键
	addChild(KeyPadWatcher::create());

	m_bkLayer = CCNode::create();
	m_uiLayer = CCNode::create();
	m_dialogLayer = CCNode::create();
	m_guideLayer = CCNode::create();
	addChild(m_bkLayer);
	addChild(m_uiLayer);
	addChild(m_dialogLayer);
	addChild(m_guideLayer);

	auto winSize = CCDirector::sharedDirector()->getWinSize();
	setContentSize(winSize);
	auto bk = BackgroundLayer::create();
	bk->setPosition(ccpMult(winSize, 0.5f));
	m_bkLayer->addChild(bk);
	showPanel(kMainMenu);
	showInitialUi();//显示游戏开始 可能需要弹出的ui
	addGuideEditBtn();//新手引导编辑器
	return true;
}

MainScene::MainScene()
: m_curPanel(NULL)
{

}

MainScene* MainScene::theScene()
{
	if (!s_scene)
	{
		s_scene = new MainScene;
		s_scene->init();
		s_scene->autorelease();
	}
	return s_scene;
}

void MainScene::addUiPanel(BasePanel *panel, bool closeBehind)
{
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	if (panel != NULL)
	{
		recordPanel(panel->getPanelId(), panel->getUsage());
		if (closeBehind)
		{
			m_uiLayer->removeAllChildrenWithCleanup(true);
		}

		panel->setAnchorPoint(ccp(0.5f, 0.5f));
		panel->setPosition(ccpMult(winSize, 0.5f));
		m_uiLayer->addChild(panel);
		m_curPanel = panel;
	}
}

void MainScene::backPanel()
{
	if (!m_panelRecord.empty())
	{
		m_panelRecord.pop_back();
	}

	if (!m_panelRecord.empty())
	{
		auto record = m_panelRecord.back();
		showPanel(record.panelId);
	}
}

void MainScene::recordPanel(int panelId, int usage)
{
	if (panelId == kDefaultPanel) return;
	for (auto iter = m_panelRecord.begin(); iter != m_panelRecord.end(); ++iter)
	{
		if (iter->panelId == panelId && iter->usage == usage)
		{
			return;
		}
	}

	PanelRecord record;
	record.panelId = panelId;
	record.usage = usage;
	m_panelRecord.push_back(record);
}

void MainScene::showPanel(int panelId, int usage, bool closeBehind)
{
	BasePanel *panel = NULL;
	switch (panelId)
	{
	case kMainMenu:
		panel = MenuScene::create();
		break;
	case kPetPanel:
		panel = PetScene::create();
		break;
	case kShopPanel:
		panel = ShopScene::create();
		break;
	case kLotteryPanel:
		panel = LotteryScene::create(usage);
		break;
	case kPackagePanel:
		panel = PackageScene::create();
		break;
	case kRankingPanel:
		panel = RankingScene::create();
		break;
	case kPreStagePanel:
		panel = PreStageScene::create();
		break;
	case kStageView:
		panel = StageScene::theScene();
		break;
	case kStageFailPanel:
		panel = GameFailLayer::create();
		break;
	case kStageWinPanel:
		panel = GameWinLayer::create();
		break;
	default:
		assert(false && "no this panelId");
		break;
	}

	addUiPanel(panel, closeBehind);
}

void MainScene::showDialog(ScaleDialog *dialog)
{
	m_dialogLayer->removeAllChildren();

	auto winSize = CCDirector::sharedDirector()->getWinSize();
	dialog->setAnchorPoint(ccp(0.5f, 0.5f));
	dialog->setPosition(ccpMult(winSize, 0.5f));
	m_dialogLayer->addChild(dialog);
}

void MainScene::showGuideView(int guideId)
{
	m_guideLayer->removeAllChildren();

	auto view = GuideView::create(guideId);
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	view->setAnchorPoint(ccp(0.5f, 0.5f));
	view->setPosition(ccpMult(winSize, 0.5f));
	m_guideLayer->addChild(view);
}

void MainScene::removeGuideView()
{
	m_guideLayer->removeAllChildren();
}

void MainScene::addGuideEditBtn()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	string path = "guide_edit/open.png";
	auto imageItem = CommonUtil::getScaleMenuItemSpr(path);
	imageItem->setTarget(this, menu_selector(MainScene::onGuideBtnClicked));

	auto menu = CCMenu::create(imageItem, NULL);
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	imageItem->setPosition(ccp(-winSize.width * 0.45f, winSize.height * 0.3f));
	addChild(menu);	
#endif
}

void MainScene::onGuideBtnClicked(cocos2d::CCObject* pSender)
{
	auto layer = GuideEditLayer::create();
	addChild(layer);
}

void MainScene::showInitialUi()
{
	if(RankingOpponent::theOpponent()->needUpdate())
	{
		auto dialog = RankingOpponentUpgradePanel::create();
		showDialog(dialog);
	}
}

void MainScene::handleKeyBackTouch()
{
	if (m_curPanel)
	{
		m_curPanel->onBackKeyTouched();
	}
}