#pragma once
#include "Composite.h"

namespace BehaviorTree
{

	class Priority : public Composite
	{

	public:
		Priority(Tree& tree, const std::string& name);

	protected:
		Behavior* mRunningChild;

		static BehaviorStatus OnUpdate(Behavior& bh, void* data);
	};

}
