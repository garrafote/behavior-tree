#include "stdafx.h"
#include "Conditional.h"
#include "Predicate.h"
#include "Tree.h"

using namespace BehaviorTree;

Conditional::Conditional(Tree& tree, Behavior& child, Predicate& predicate, std::string name)
	: Decorator(tree, child, name)
	, mPredicate(predicate)
{
	SetInitializeCallback(&OnInitialize);
	SetUpdateCallback(&OnUpdate);
	//child.SetObserver({ OnChildComplete, this, nullptr });
}

void Conditional::OnInitialize(Behavior& bh, void* data)
{
	//auto& self = static_cast<Conditional&>(bh);

	//self.mTree.Start(self.mChild, { &OnChildComplete, &self, data });
}

BehaviorStatus Conditional::OnUpdate(Behavior& bh, void* data)
{
	auto& self = reinterpret_cast<Conditional&>(bh);

	self.mPredicate.Tick();

	if (!self.mPredicate.IsValid())
	{
		self.mTree.Stop(self.mChild, BehaviorStatus::Failure);
		return BehaviorStatus::Failure;
	}
	
	if (self.mStatus != BehaviorStatus::Running)
	{
		self.mTree.Start(self.mChild, { &OnChildComplete, &self, data });
		return BehaviorStatus::Running;
	}

	switch (self.mChild.GetStatus())
	{
	case BehaviorStatus::Failure:
		return BehaviorStatus::Failure;
	
	case BehaviorStatus::Success:
		return BehaviorStatus::Success;

	default:
		return BehaviorStatus::Running;
	}
}

void Conditional::OnChildComplete(Behavior& bh, void* data, BehaviorStatus status)
{
	auto& self = static_cast<Conditional&>(bh);

	ASSERT(self.mChild.GetStatus() != BehaviorStatus::Running);

	self.mTree.Stop(self, status);
}
