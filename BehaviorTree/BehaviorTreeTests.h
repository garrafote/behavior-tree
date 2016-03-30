#pragma once
#include "Behavior.h"
#include "Predicate.h"

struct MockBehavior : public Behavior
{
	int mInitializeCalled;
	int mTerminateCalled;
	int mUpdateCalled;
	BehaviorStatus mReturnStatus;
	BehaviorStatus mTerminateStatus;

	MockBehavior(Tree& tree, std::string name = "Mock Behavior")
		: Behavior(tree, name)
		, mInitializeCalled(0)
		, mTerminateCalled(0)
		, mUpdateCalled(0)
		, mReturnStatus(BehaviorStatus::Running)
		, mTerminateStatus(BehaviorStatus::Invalid)
	{
		SetInitializeCallback(&OnInitialize);
		SetUpdateCallback(&OnUpdate);
		SetTerminateCallback(&OnTerminate);
	}

	~MockBehavior()
	{
	}

	static void OnInitialize(Behavior& bh, void* data)
	{
		auto& self = static_cast<MockBehavior&>(bh);

		++self.mInitializeCalled;
	}

	static void OnTerminate(Behavior& bh, void* data, BehaviorStatus status)
	{
		auto& self = static_cast<MockBehavior&>(bh);

		++self.mTerminateCalled;
		self.mTerminateStatus = status;
	}

	static BehaviorStatus OnUpdate(Behavior& bh, void* data)
	{
		auto& self = static_cast<MockBehavior&>(bh);

		++self.mUpdateCalled;
		return self.mReturnStatus;
	}
};


template <class COMPOSITE>
class MockComposite : public COMPOSITE
{
public:
	MockComposite(class Tree& tree, size_t size, std::string name = "Mock Composite")
		: COMPOSITE(tree, name)
	{
		for (size_t i = 0; i< size; ++i)
		{
			std::stringstream childName;
			childName << "Mock Behavior [" << i << "]";
			COMPOSITE::Add(*new MockBehavior(tree, childName.str()));
		}
	}

	~MockComposite()
	{
		for (size_t i = 0; i<COMPOSITE::mChildren.size(); ++i)
		{
			delete COMPOSITE::mChildren[i];
		}
	}

	MockBehavior& operator[](size_t index)
	{
		ASSERT(index < COMPOSITE::mChildren.size());
		return *static_cast<MockBehavior*>(COMPOSITE::mChildren[index]);
	}
};


struct MockPredicate : public Predicate
{
	bool mPredicateResult;

	MockPredicate(Tree& tree, std::string name = "Mock Predicate")
		: Predicate(tree, name)
		, mPredicateResult(true)
	{
		SetPredicateCallback(OnPredicate);
	}

	static bool OnPredicate(Behavior& bh, void* data)
	{
		auto& self = reinterpret_cast<MockPredicate&>(bh);
		return self.mPredicateResult;
	}
};