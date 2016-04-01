#include "stdafx.h"
#include "Tree.h"

using namespace BehaviorTree;
using namespace std;
using namespace std::placeholders;

BasicTree::BasicTree(std::string name) : Tree(name)
{
	SetUpdateCallback(OnUpdate);

	mOnStartTree = std::bind(&BasicTree::OnStart, this, _1, _2);
	mOnStartBehavior = std::bind(&BasicTree::OnStartBehavior, this, _1, _2);
	mOnStop = std::bind(&BasicTree::OnStop, this, _1, _2);
}

void BasicTree::OnStartBehavior(Behavior& bh, BehaviorObserver observer)
{
//	bh.SetObserver(observer);
//	Start(bh, false);
}

void BasicTree::OnStart(Behavior& bh, bool isRoot)
{
	if (isRoot)
	{
		mRootBehavior = &bh;
	}
}

void BasicTree::OnStop(Behavior& bh, BehaviorStatus result)
{
	//ASSERT(result != BehaviorStatus::Running);

	//bh.mStatus = result;
	//bh.NotifyObserver(result);
}

BehaviorStatus BasicTree::OnUpdate(Behavior& bh, void* userData)
{
	auto& self = reinterpret_cast<BasicTree&>(bh);
	
	return self.mRootBehavior->Tick(userData);
}