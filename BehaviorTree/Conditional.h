#pragma once
#include "Decorator.h"

class Conditional : public Decorator
{
public:
	Conditional(Tree& tree, Behavior& child, class Predicate& predicate, std::string name = "Decorator");

protected:
	class Predicate& mPredicate;

	static BehaviorStatus OnUpdate(Behavior& bh, void* data);
};

