#pragma once
#include "Behavior.h"
#include "Tree.h"
#include <deque>

namespace BehaviorTree
{

	class EventTree : public Tree
	{
	public:
		EventTree(std::string name = "Root");

	protected:
		std::deque<Behavior*> mBehaviors;

		bool Step(void* userData);

		void OnStart(Behavior& bh, bool isRoot = true);
		void OnStartBehavior(Behavior& bh, BehaviorObserver observer);
		void OnStop(Behavior& bh, BehaviorStatus result);
		static BehaviorStatus OnUpdate(Behavior& bh, void* userData);
	};

}
