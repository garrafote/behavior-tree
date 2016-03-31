#pragma once
#include <string>

namespace BehaviorTree
{
	enum class BehaviorStatus
	{
		Invalid,
		Success,
		Failure,
		Running,
		Suspended,
	};

	class Behavior;
	typedef void(*ObserverCallback)(Behavior&, void*, BehaviorStatus);
	typedef BehaviorStatus(*UpdateCallback)(Behavior&, void*);
	typedef void(*ResetCallback)(Behavior&, void*);
	typedef void(*InitializeCallback)(Behavior&, void*);
	typedef void(*TerminateCallback)(Behavior&, void*, BehaviorStatus);

	struct BehaviorObserver
	{
		ObserverCallback callback;
		Behavior*        behavior;
		void*            data;

		static BehaviorObserver Default() { return{ nullptr, nullptr, nullptr }; }
	};

	class Behavior
	{
		friend class Tree;

	public:

		Behavior(Tree& tree, std::string name = "Behavior");
		~Behavior();

		void SetInitializeCallback(InitializeCallback callback)
		{
			mOnInitialize = callback;
		}

		void SetResetCallback(ResetCallback callback)
		{
			mOnReset = callback;
		}

		void SetUpdateCallback(UpdateCallback callback)
		{
			mOnUpdate = callback;
		}

		void SetTerminateCallback(TerminateCallback callback)
		{
			mOnTerminate = callback;
		}

		BehaviorStatus GetStatus() const
		{
			return mStatus;
		}

		bool IsRunningOrSuspended()
		{
			return mStatus == BehaviorStatus::Running || mStatus == BehaviorStatus::Suspended;
		}

		void SetObserver(BehaviorObserver observer)
		{
			mObserver = observer;
		}

		void NotifyObserver(BehaviorStatus status) const
		{
			if (mObserver.callback)
			{
				mObserver.callback(*mObserver.behavior, mObserver.data, status);
			}
		}

		BehaviorStatus Tick(void* userData = nullptr);
		void Reset(void* userData);

	protected:
		Tree&              mTree;
		std::string        mName;
		BehaviorStatus     mStatus;
		BehaviorObserver   mObserver;

		UpdateCallback     mOnUpdate;
		ResetCallback      mOnReset;
		InitializeCallback mOnInitialize;
		TerminateCallback  mOnTerminate;
	};
}