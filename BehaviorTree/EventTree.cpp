#include "stdafx.h"
#include "EventTree.h"

using namespace BehaviorTree;
using namespace std;
using namespace std::placeholders;

EventTree::EventTree(string name) : Tree(name)
{
	SetUpdateCallback(OnUpdate);

	mOnStartTree = bind(&EventTree::OnStart, this, _1, _2);
	mOnStartBehavior = bind(&EventTree::OnStartBehavior, this, _1, _2);
	mOnStop = bind(&EventTree::OnStop, this, _1, _2);
}

void EventTree::OnStartBehavior(Behavior& bh, BehaviorObserver observer)
{
	bh.SetObserver(observer);
	Start(bh, false);
}

void EventTree::OnStart(Behavior& bh, bool isRoot)
{
	mBehaviors.push_front(&bh);

	if (isRoot)
	{
		mRootBehavior = &bh;
		//mRootBehavior->SetObserver({ OnRootComplete, this, nullptr });
	}
}

void EventTree::OnStop(Behavior& bh, BehaviorStatus result)
{
	ASSERT(result != BehaviorStatus::Running);

	bh.mStatus = result;
	bh.NotifyObserver(result);
}

BehaviorStatus EventTree::OnUpdate(Behavior& bh, void* userData)
{
	auto& self = reinterpret_cast<EventTree&>(bh);

	//return self.mRootBehavior->Tick(userData);
	if (self.mBehaviors.empty())
	{
		self.mRootBehavior->Reset(userData);
		self.mBehaviors.push_back(self.mRootBehavior);
	}

	// placeholder for end of frame
	self.mBehaviors.push_back(nullptr);

	// keep stepping until nullptr placeholder is found
	while (self.Step(userData))
	{
		continue;
	}

	return self.mRootBehavior->GetStatus();
}

bool EventTree::Step(void* userData)
{
	Behavior* current = mBehaviors.front();
	mBehaviors.pop_front();

	// if popped behavior is nullptr, return false (end of frame)
	if (!current)
	{
		return false;
	}

	if (current->mStatus != BehaviorStatus::Suspended)
	{
		current->Tick(userData);
	}

	if (current->mStatus == BehaviorStatus::Running)
	{
		mBehaviors.push_back(current);
	}
	else if (current->mStatus != BehaviorStatus::Suspended)
	{
		current->NotifyObserver(current->mStatus);
	}

	return true;
}
