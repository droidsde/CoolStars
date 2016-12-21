#include "GameBackEndState.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
#include <jni.h> 
#include "platform/android/jni/JniHelper.h" 
#endif
#include "StageDataMgr.h"
using namespace std;

GameBackEndState::GameBackEndState()
{

}

GameBackEndState *GameBackEndState::theModel()
{
	static GameBackEndState model;
	return &model;
}

void GameBackEndState::recordStageStart()
{
	int stage = StageDataMgr::theMgr()->getCurStage();
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	const char* funstr = "org/cocos2dx/lib/PayAndroidApi";
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,
		funstr,
		"rtnActivity",
		"()Ljava/lang/Object;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(minfo,
		funstr,
		"startStage",
		"(I)V");
	if (isHave) {
		minfo.env->CallVoidMethod(jobj, minfo.methodID, stage);
	}
#endif
}

void GameBackEndState::recordStageEnd(bool win)
{
	int stage = StageDataMgr::theMgr()->getCurStage();
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	const char* funstr = "org/cocos2dx/lib/PayAndroidApi";
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,
		funstr,
		"rtnActivity",
		"()Ljava/lang/Object;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(minfo,
		funstr,
		"endStage",
		"(IZ)V");
	if (isHave) {
		minfo.env->CallVoidMethod(jobj, minfo.methodID, stage, win);
	}
#endif
}

bool GameBackEndState::isBusinessMode()
{
	bool isBusinessMode = true;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
	const char* funstr = "org/cocos2dx/lib/PayAndroidApi";
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo,
		funstr,
		"rtnActivity",
		"()Ljava/lang/Object;");
	jobject jobj;
	if (isHave) {
		jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(minfo,
		funstr,
		"isForBusiness",
		"()Z");
	if (isHave) {
		isBusinessMode = minfo.env->CallBooleanMethod(jobj, minfo.methodID);
	}
#endif
	return isBusinessMode;
}