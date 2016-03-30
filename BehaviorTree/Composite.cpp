#include "stdafx.h"
#include "Composite.h"

Composite::Composite(Tree& tree, std::string name) : Behavior(tree, name)
{
}

void Composite::Add(Behavior& behavior)
{
	mChildren.push_back(&behavior);
}
