#pragma once
#include "Behavior.h"
#include <deque>

class Tree : public Behavior
{
public:
	Tree(std::string name = "Root");

	void Start(Behavior& bh, bool isRoot = true);
	void Start(Behavior& bh, BehaviorObserver observer);
	void Stop(Behavior& bh, BehaviorStatus result);
	bool Step(void* userData);

protected:
	std::deque<Behavior*> mBehaviors;
	Behavior* mRootBehavior;

	static BehaviorStatus OnUpdate(Behavior& bh, void* userData);
	//static void OnRootComplete(Behavior& bh, void* data, BehaviorStatus status);
};

