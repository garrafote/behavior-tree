#include "stdafx.h"
#include "Decorator.h"


Decorator::Decorator(Tree& tree, Behavior& child, std::string name)
	: Behavior(tree, name)
	, mChild(child)
{

}
