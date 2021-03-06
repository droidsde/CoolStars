#ifndef __STAGEMODEL_H__
#define __STAGEMODEL_H__

#include "cocos2d.h"
#include "ConfData.h"
#include "StarNode.h"
#include "DataManager.h"
#include "StageTarget.h"
#include "StarsLoader.h"
#include "StarsBehavior.h"

struct IStarsControlView
{
	virtual void onCreateNewStar(StarNode *node){}
	virtual void onStarRemove(){}
	virtual void onShowGameResult(int isWon){}
	virtual void onRunOutSteps(){}
	virtual void onGameWin(){}
	virtual void onOneRoundBegan(){}
	virtual void onOneRoundEnd(){}
	virtual void onDesignatedStarChanged(int starType, int color, int rounds){}
};

class StarsController
{
public:
	static StarsController *theModel();
public: //对星星的操作接口
	StarNode *getStarNode(const LogicGrid &grid);
	std::vector<StarNode *> &getStarNodes(){ return m_starNodes; }
	void initStarsData();
    void moveStars();
	void removeStarNode(StarNode *node);
	void genNewStars();
	void moveOneStep(bool addStep = true);
	void onOneRoundBegan();
	void onOneRoundEnd();
	void addScore(int value);
	void replaceStar(const StarAttr &attr);
	void genStar(const StarAttr &attr);
	void resetStage(int gameType);
	int getStageAmount();
	void loadDesignatedStar(int starType, int color, int rounds);
	void gameOver(bool isWon);
	void onDesignatedStarChanged(int starType, int color, int rounds);
public:
	StageTarget *getStageTarget(){ return &m_target; }
public:
	void addView(IStarsControlView *view);
	void removeView(IStarsControlView *view);
private:
	StarsController();
	~StarsController();

    void moveStar(StarNode *node);
	bool isGridEmpty(const LogicGrid &grid);
	bool noStarsToErase();
	void reOrderStars(int times);
	void checkGameOver();
private:
	std::vector<StarNode *> m_starNodes;
	std::vector<IStarsControlView *> m_views;
	StageTarget m_target;
	StarsLoader m_starsLoader;
	StarsBehavior m_starsBehavior;
private:
	enum Direction
	{
		kMoveUp,		
		kMoveDown,	
		kMoveLeft,	
		kMoveRight,
	};
};
#endif