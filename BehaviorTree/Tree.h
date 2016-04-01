#pragma once
#include "Behavior.h"
#include <functional>
#include <deque>

namespace BehaviorTree
{

	class Tree : public Behavior
	{
	public:
		void Start(Behavior& bh, bool isRoot = true) { mOnStartTree(bh, isRoot); };
		void Start(Behavior& bh, BehaviorObserver observer) { mOnStartBehavior(bh, observer); };
		void Stop(Behavior& bh, BehaviorStatus result) { mOnStop(bh, result); };

	protected:
		Behavior* mRootBehavior;

		std::function<void(Behavior&, bool)> mOnStartTree;
		std::function<void(Behavior&, BehaviorObserver)> mOnStartBehavior;
		std::function<void(Behavior&, BehaviorStatus)> mOnStop;

		Tree(std::string name = "Root") : Behavior(*this, name), mRootBehavior(nullptr) {}

	};

	class BasicTree : public Tree
	{
	public:
		BasicTree(std::string name = "Root");

	protected:
		void OnStart(Behavior& bh, bool isRoot = true);
		void OnStartBehavior(Behavior& bh, BehaviorObserver observer);
		void OnStop(Behavior& bh, BehaviorStatus result);

		static BehaviorStatus OnUpdate(Behavior& bh, void* userData);
	};

}