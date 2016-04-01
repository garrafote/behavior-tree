#include "stdafx.h"
#include "Tree.h"

using namespace BehaviorTree;

Tree::Tree(std::string name) : Behavior(*this, name), mRootBehavior(nullptr)
{
	SetUpdateCallback(OnUpdate);
}

void Tree::Start(Behavior& bh)
{
	mRootBehavior = &bh;
}

BehaviorStatus Tree::OnUpdate(Behavior& bh, void* userData)
{
	auto& self = reinterpret_cast<Tree&>(bh);
	
	return self.mRootBehavior->Tick(userData);
}