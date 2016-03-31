#include "stdafx.h"
#include "Decorator.h"

using namespace BehaviorTree;

Decorator::Decorator(Tree& tree, Behavior& child, std::string name)
	: Behavior(tree, name)
	, mChild(child)
{

}
