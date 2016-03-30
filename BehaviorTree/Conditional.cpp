#include "stdafx.h"
#include "Conditional.h"
#include "Predicate.h"


Conditional::Conditional(Tree& tree, Behavior& child, Predicate& predicate, std::string name)
	: Decorator(tree, child, name)
	, mPredicate(predicate)
{
	SetUpdateCallback(&OnUpdate);
}


BehaviorStatus Conditional::OnUpdate(Behavior& bh, void* data)
{
	auto& self = reinterpret_cast<Conditional&>(bh);

	self.mPredicate.Tick();

	if (self.mPredicate.IsValid())
	{
		return self.mChild.Tick(data);
	}

	return BehaviorStatus::Failure;
}
