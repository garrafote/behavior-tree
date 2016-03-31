#include "stdafx.h"
#include "Priority.h"
#include "Tree.h"

using namespace BehaviorTree;

Priority::Priority(Tree& tree, const std::string& name)
	: Composite(tree, name)
{
	SetInitializeCallback(&OnInitialize);
	SetUpdateCallback(&OnUpdate);
	SetResetCallback(&OnReset);
}

void Priority::OnInitialize(Behavior& bh, void* data)
{
	auto& self = reinterpret_cast<Priority&>(bh);
	self.mCurrent = self.mChildren.end();
/*
	self.mCurrent = self.mChildren.begin();
	self.mTree.Start(**self.mCurrent, { &OnChildComplete, &self, data });*/
}

void Priority::OnChildComplete(Behavior& bh, void* data, BehaviorStatus status)
{
	auto& self = static_cast<Priority&>(bh);

	const Behavior& child = **self.mCurrent;

	// stop as soon as the current child has the same status 
	// as breakstatus (either success or failure)
	if (child.GetStatus() == BehaviorStatus::Success)
	{
		self.mTree.Stop(self, BehaviorStatus::Success);
		return;
	}

	// children can either complete with success or failure
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

	if (self.mRunning != *self.mCurrent || !self.mRunning->IsRunningOrSuspended())
	{
		self.mTree.Start(**self.mCurrent, { &OnChildComplete, &self, data });
	}
}


void Priority::OnReset(Behavior& bh, void* data)
{
	auto& self = static_cast<Priority&>(bh);

	self.mStatus = BehaviorStatus::Invalid;
}

BehaviorStatus Priority::OnUpdate(Behavior& bh, void* data)
{
	auto& self = reinterpret_cast<Priority&>(bh);

	if (self.mCurrent != self.mChildren.end())
	{
		self.mRunning = *self.mCurrent;
	}

	self.mCurrent = self.mChildren.begin();

	if (self.mRunning != *self.mCurrent || !self.mRunning->IsRunningOrSuspended())
	{
		self.mTree.Start(**self.mCurrent, { &OnChildComplete, &self, data });
	}

	//if (self.mRunning)
	//{
	//	for (auto child : self.mChildren)
	//	{
	//		if (child == self.mRunning)
	//		{
	//			// need to clear previously running child 
	//			// if it is after the currently running child
	//			child->Reset(data);
	//			break;
	//		}
	//	}
	//}

	return BehaviorStatus::Running;

	Behavior* selected = nullptr;
	for (auto child : self.mChildren)
	{
		if (selected)
		{
			if (child == self.mRunning)
			{
				// need to clear previously running child 
				// if it is after the currently running child
				child->Reset(data);
			}
			continue;
		}

		if (child->Tick() != BehaviorStatus::Failure)
		{
			selected = child;
		}
	}

	if (selected)
	{
		self.mRunning = selected;
		return selected->GetStatus();
	}

	return BehaviorStatus::Failure;
}
