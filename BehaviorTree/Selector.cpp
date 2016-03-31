#include "stdafx.h"
#include "Selector.h"
#include "Tree.h"

using namespace BehaviorTree;

Selector::Selector(Tree& tree, std::string name) :
	Composite(tree, name)
{
	SetInitializeCallback(&OnInitialize);
	SetUpdateCallback(&OnUpdate);
	SetResetCallback(&OnReset);
}

void Selector::OnInitialize(Behavior& bh, void* data)
{
	auto& self = static_cast<Selector&>(bh);

	self.mCurrent = self.mChildren.begin();
	self.mTree.Start(**self.mCurrent, { &OnChildComplete, &self, data });
}

void Selector::OnReset(Behavior& bh, void* data)
{
	auto& self = static_cast<Selector&>(bh);

	self.mStatus = BehaviorStatus::Invalid;
	//self.mCurrent = self.mChildren.begin();
	//
	//if (self.mStatus != BehaviorStatus::Running)
	//{
	//	self.mBehaviorTree->Start(**self.mCurrent, { &OnChildComplete, &self, data });
	//}

}

void Selector::OnChildComplete(Behavior& bh, void* data, BehaviorStatus status)
{
	auto& self = static_cast<Selector&>(bh);

	const Behavior& child = **self.mCurrent;

	// stop as soon as the current child has the same status 
	// as breakstatus (either success or failure)
	if (child.GetStatus() == BehaviorStatus::Success)
	{
		self.mTree.Stop(self, BehaviorStatus::Success);
		return;
	}

	// childrens can either complete with success or failure
	ASSERT(child.GetStatus() == BehaviorStatus::Failure);

	// otherwise, move on to the next child
	++self.mCurrent;

	// return fallthrough (success or failure) if no children left
	if (self.mCurrent == self.mChildren.end())
	{
		self.mTree.Stop(self, BehaviorStatus::Failure);
		return;
	}

	// start the execution of the next child 
	// and get notified on it's completion
	self.mTree.Start(**self.mCurrent, { &OnChildComplete, &self, data });
}

BehaviorStatus Selector::OnUpdate(Behavior& bh, void* data)
{
	auto& self = reinterpret_cast<Selector&>(bh);

	// always return running,
	// mStatus will be set to SUCCESS or FAILURE when a children is completed
	// using BehaviorTree::Stop(this, status);
	return BehaviorStatus::Suspended;

	for (;;)
	{
		auto& self = static_cast<Selector&>(bh);

		Behavior& child = **self.mCurrent;

		child.Tick(data);

		// stop as soon as the current child has the same status 
		// as breakstatus (either success or failure)
		if (child.GetStatus() == BehaviorStatus::Success)
		{
			//self.mBehaviorTree->Stop(self, self.mBreakStatus);
			self.mCurrent = self.mChildren.begin();
			return BehaviorStatus::Success;
		}

		// childrens can either complete with success or failure
		//ASSERT(child.GetStatus() == fallthrough);
		if (child.GetStatus() == BehaviorStatus::Running)
		{
			return BehaviorStatus::Running;
		}

		// otherwise, move on to the next child
		++self.mCurrent;

		// return fallthrough (success or failure) if no children left
		if (self.mCurrent == self.mChildren.end())
		{
			// self.mBehaviorTree->Stop(self, fallthrough);
			self.mCurrent = self.mChildren.begin();
			return BehaviorStatus::Failure;
		}
	}
}