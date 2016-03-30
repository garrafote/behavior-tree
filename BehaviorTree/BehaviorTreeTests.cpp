#include "stdafx.h"
#include "BehaviorTreeTests.h"
#include <map>
#include "Tree.h"
#include "Behavior.h"
#include "Selector.h"
#include "Sequence.h"
#include "Predicate.h"
#include "Conditional.h"

#pragma region Miscellaneous

namespace std
{
	inline ostream& operator<<(std::ostream& out, const BehaviorStatus& value) {
		static map<BehaviorStatus, std::string> strings;
		if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
			INSERT_ELEMENT(BehaviorStatus::Invalid);
			INSERT_ELEMENT(BehaviorStatus::Success);
			INSERT_ELEMENT(BehaviorStatus::Failure);
			INSERT_ELEMENT(BehaviorStatus::Running);
			INSERT_ELEMENT(BehaviorStatus::Suspended);
#undef INSERT_ELEMENT
		}

		return out << strings[value];
	}
}

typedef MockComposite<Selector> MockSelector;
typedef MockComposite<Sequence> MockSequence;

#pragma endregion

#pragma region Behavior Tests

TEST(Behavior, Should_InitializeOnlyOnce_When_TickIsCalled)
{
	Tree tree;
	MockBehavior behavior(tree);
	tree.Start(behavior);

	CHECK_EQUAL(0, behavior.mInitializeCalled);

	tree.Tick();
	CHECK_EQUAL(1, behavior.mInitializeCalled);

	// should not be called again
	tree.Tick();
	CHECK_EQUAL(1, behavior.mInitializeCalled);
}

TEST(Behavior, Should_Update_When_TickIsCalled)
{
	Tree tree;
	MockBehavior behavior(tree);
	tree.Start(behavior);

	CHECK_EQUAL(0, behavior.mUpdateCalled);

	tree.Tick();
	CHECK_EQUAL(1, behavior.mUpdateCalled);

	// should be called again
	tree.Tick();
	CHECK_EQUAL(2, behavior.mUpdateCalled);
}

TEST(Behavior, Should_Terminate_When_ActionSucceded)
{
	Tree tree;
	MockBehavior behavior(tree);
	tree.Start(behavior);

	tree.Tick();
	CHECK_EQUAL(0, behavior.mTerminateCalled);

	behavior.mReturnStatus = BehaviorStatus::Success;
	tree.Tick();
	CHECK_EQUAL(1, behavior.mTerminateCalled);
}

#pragma endregion

#pragma region Selector Tests


TEST(Selector, Should_Suspend_When_ChildIsRunning)
{
	Tree tree;
	MockSelector selector(tree, 1);
	tree.Start(selector);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, selector.GetStatus());
}

TEST(Selector, Should_Terminate_When_TickPassThroughToSibling)
{
	BehaviorStatus status[] = { BehaviorStatus::Success, BehaviorStatus::Failure };

	for (auto s : status)
	{
		Tree tree;
		MockSelector selector(tree, 1);
		tree.Start(selector);

		tree.Tick();

		CHECK_EQUAL(BehaviorStatus::Suspended, selector.GetStatus());
		CHECK_EQUAL(0, selector[0].mTerminateCalled);

		selector[0].mReturnStatus = s;
		tree.Tick();

		CHECK_EQUAL(s, selector.GetStatus());
		CHECK_EQUAL(1, selector[0].mTerminateCalled);
	}
}

TEST(Selector, Should_Succed_When_ChildSucceds)
{
	Tree tree;
	MockSelector selector(tree, 3);
	tree.Start(selector);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, selector.GetStatus());
	CHECK_EQUAL(0, selector[0].mTerminateCalled);
	CHECK_EQUAL(0, selector[1].mInitializeCalled);

	selector[0].mReturnStatus = BehaviorStatus::Failure;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, selector.GetStatus());
	CHECK_EQUAL(1, selector[0].mTerminateCalled);
	CHECK_EQUAL(1, selector[1].mInitializeCalled);

	selector[1].mReturnStatus = BehaviorStatus::Success;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Success, selector.GetStatus());
	CHECK_EQUAL(1, selector[0].mTerminateCalled);
	CHECK_EQUAL(1, selector[1].mTerminateCalled);
}

TEST(Selector, Should_NotInitializeSecond_When_FirstSucceeds)
{
	Tree tree;
	MockSelector selector(tree, 2);
	tree.Start(selector);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, selector.GetStatus());
	CHECK_EQUAL(0, selector[0].mTerminateCalled);
	CHECK_EQUAL(0, selector[1].mInitializeCalled);

	selector[0].mReturnStatus = BehaviorStatus::Success;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Success, selector.GetStatus());
	CHECK_EQUAL(1, selector[0].mTerminateCalled);
	CHECK_EQUAL(0, selector[1].mInitializeCalled);
}

TEST(Selector, Should_RestartOnTick_When_Terminated)
{
	Tree tree;
	MockSelector selector(tree, 2);
	tree.Start(selector);

	selector[0].mReturnStatus = BehaviorStatus::Success;

	for (size_t i = 1; i <= 5; i++)
	{
		tree.Tick();

		CHECK_EQUAL(BehaviorStatus::Success, selector.GetStatus());
		CHECK_EQUAL(i, selector[0].mUpdateCalled);
		CHECK_EQUAL(i, selector[0].mTerminateCalled);
		CHECK_EQUAL(0, selector[1].mInitializeCalled);
	}
}


#pragma endregion

#pragma region Sequence Tests

TEST(Sequence, Should_Suspend_When_ChildIsRunning)
{
	Tree tree;
	MockSequence sequence(tree, 1);
	tree.Start(sequence);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, sequence.GetStatus());
}


TEST(Sequence, Should_Terminate_When_TickPassThroughToSibling)
{
	BehaviorStatus status[] = { BehaviorStatus::Success, BehaviorStatus::Failure };
	
	for (auto s : status)
	{
		Tree tree;
		MockSequence sequence(tree, 1);
		tree.Start(sequence);

		tree.Tick();

		CHECK_EQUAL(BehaviorStatus::Suspended, sequence.GetStatus());
		CHECK_EQUAL(0, sequence[0].mTerminateCalled);

		sequence[0].mReturnStatus = s;
		tree.Tick();

		CHECK_EQUAL(s, sequence.GetStatus());
		CHECK_EQUAL(1, sequence[0].mTerminateCalled);
	}
}

TEST(Sequence, Should_Fail_When_ChildFails)
{
	Tree tree;
	MockSequence sequence(tree, 3);
	tree.Start(sequence);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, sequence.GetStatus());
	CHECK_EQUAL(0, sequence[0].mTerminateCalled);
	CHECK_EQUAL(0, sequence[1].mInitializeCalled);

	sequence[0].mReturnStatus = BehaviorStatus::Success;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, sequence.GetStatus());
	CHECK_EQUAL(1, sequence[0].mTerminateCalled);
	CHECK_EQUAL(1, sequence[1].mInitializeCalled);

	sequence[1].mReturnStatus = BehaviorStatus::Failure;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Failure, sequence.GetStatus());
	CHECK_EQUAL(1, sequence[0].mTerminateCalled);
	CHECK_EQUAL(1, sequence[1].mTerminateCalled);
}

TEST(Sequence, Should_NotInitializeSecond_When_FirstFails)
{
	Tree tree;
	MockSequence sequence(tree, 2);
	tree.Start(sequence);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Suspended, sequence.GetStatus());
	CHECK_EQUAL(0, sequence[0].mTerminateCalled);
	CHECK_EQUAL(0, sequence[1].mInitializeCalled);

	sequence[0].mReturnStatus = BehaviorStatus::Failure;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Failure, sequence.GetStatus());
	CHECK_EQUAL(1, sequence[0].mTerminateCalled);
	CHECK_EQUAL(0, sequence[1].mInitializeCalled);
}

TEST(Sequence, Should_RestartOnTick_When_Terminated)
{
	Tree tree;
	MockSequence sequence(tree, 2);
	tree.Start(sequence);

	sequence[0].mReturnStatus = BehaviorStatus::Failure;

	for (size_t i = 1; i <= 5; i++)
	{
		tree.Tick();

		CHECK_EQUAL(BehaviorStatus::Failure, sequence.GetStatus());
		CHECK_EQUAL(i, sequence[0].mUpdateCalled);
		CHECK_EQUAL(i, sequence[0].mTerminateCalled);
		CHECK_EQUAL(0, sequence[1].mInitializeCalled);
	}
}

#pragma endregion

#pragma region Priority Selector Tests



#pragma endregion

#pragma region Predicate Tests

TEST(Predicate, Should_Succeed_When_NoPredicate)
{
	Tree tree;
	Predicate predicate(tree);
	tree.Start(predicate);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Success, predicate.GetStatus());
	CHECK(predicate.IsValid());
}

TEST(Predicate, Should_Succeed_When_PredicateReturnsTrue)
{
	Tree tree;
	MockPredicate predicate(tree);

	tree.Start(predicate);

	predicate.mPredicateResult = true;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Success, predicate.GetStatus());
	CHECK(predicate.IsValid());
}

TEST(Predicate, Should_Fail_When_PredicateReturnsFalse)
{
	Tree tree;
	MockPredicate predicate(tree);

	tree.Start(predicate);

	predicate.mPredicateResult = false;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Failure, predicate.GetStatus());
	CHECK(!predicate.IsValid());
}

#pragma endregion

#pragma region Conditional Tests


TEST(Conditional, Should_Fail_When_PredicateFails)
{
	Tree tree;
	MockPredicate predicate(tree);
	MockBehavior behavior(tree);
	Conditional conditional(tree, behavior, predicate);

	tree.Start(conditional);

	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Running, conditional.GetStatus());

	predicate.mPredicateResult = false;
	tree.Tick();

	CHECK_EQUAL(BehaviorStatus::Failure, conditional.GetStatus());
}


TEST(Conditional, Should_ForwardActionStatus_When_PredicateSucceeds)
{
	Tree tree;
	MockPredicate predicate(tree);
	MockBehavior behavior(tree);
	Conditional conditional(tree, behavior, predicate);

	tree.Start(conditional);

	BehaviorStatus status[] = { 
		BehaviorStatus::Success, 
		BehaviorStatus::Suspended, 
		BehaviorStatus::Failure, 
		BehaviorStatus::Success
	};

	for (auto s : status)
	{
		behavior.mReturnStatus = s;
		tree.Tick();

		CHECK_EQUAL(s, conditional.GetStatus());
	}
}

#pragma endregion

// test subtrees...

// test interaction between selectors and sequences