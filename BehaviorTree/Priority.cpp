#include "stdafx.h"
#include "Priority.h"

using namespace BehaviorTree;

Priority::Priority(Tree& tree, const std::string& name)
	: Composite(tree, name)
{
	SetUpdateCallback(&OnUpdate);
}

BehaviorStatus Priority::OnUpdate(Behavior& bh, void* data)
{
	auto& self = reinterpret_cast<Priority&>(bh);

	Behavior* selected = nullptr;
	for (auto child : self.mChildren)
	{
		if (selected)
		{
			if (child == self.mRunningChild)
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
		self.mRunningChild = selected;
		return selected->GetStatus();
	}

	return BehaviorStatus::Failure;
}
