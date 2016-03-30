#pragma once
#pragma once
#include "Behavior.h"

class Composite : public Behavior
{
public:
	Composite(Tree& tree, std::string name);

	std::vector<Behavior*> mChildren;

	void Add(Behavior& behavior);
};
