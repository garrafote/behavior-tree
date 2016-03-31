#pragma once
#include "Composite.h"

namespace BehaviorTree
{

	class Selector : public Composite
	{
	public:
		Selector(Tree& tree, std::string name = "Selector");

	protected:
		/*class BehaviorTree* mBehaviorTree;*/
		std::vector<class Behavior*>::iterator mCurrent;

		static void OnChildComplete(Behavior& self, void* data, BehaviorStatus status);
		static void OnInitialize(Behavior& self, void* data);
		static void OnReset(Behavior& bh, void* data);
		static BehaviorStatus OnUpdate(Behavior& self, void* data);
	};

}