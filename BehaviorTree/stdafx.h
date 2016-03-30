// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <functional>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

// TODO: reference additional headers your program requires here


#if defined(_MSC_VER) && (_MSC_VER >= 1600) // VS2010
#define ASSERT(X) assert(X); __analysis_assume(X)
#define ASSERT_MSG(X, M) ASSERT(X)
#else
#define ASSERT(X) assert(X)
#define ASSERT_MSG(X, M) ASSERT(X)
#endif

namespace test
{
	typedef std::function<void()> Test;

	class TestFailedException
	{
	public:
		TestFailedException(std::string message)
			: mMessage(message)
		{
		}

		std::string mMessage;
	};

	class TestSuite
	{
	private:
		typedef std::pair<std::string, Test> RegisteredTest;
		typedef std::vector<RegisteredTest> RegisteredTests;

	public:
		TestSuite()
			: mIsVerbose(false)
		{
		}

		void RegisterTest(const std::string& testName, Test test)
		{
			mTests.push_back(RegisteredTest(testName, test));
		}

		void SetVerbose(bool b)
		{
			mIsVerbose = b;
		}

		bool RunAllTests()
		{
			bool allPassed = true;
			for (RegisteredTests::const_iterator test = mTests.begin(); test != mTests.end(); ++test)
			{
				if (mIsVerbose)
				{
					std::cout << test->first << " " << std::flush;
				}
				try
				{
					test->second();
					if (mIsVerbose)
					{
						std::cout << "PASS" << std::endl << std::flush;
					}
					else
					{
						std::cout << "." << std::flush;
					}
				}
				catch (TestFailedException e)
				{
					allPassed = false;
					if (!mIsVerbose)
					{
						std::cout << test->first << " ";
					}
					std::cout << "FAIL: " << e.mMessage << std::endl << std::flush;
				}
			}
			if (!mIsVerbose)
			{
				std::cout << std::endl << std::flush;
			}
			return allPassed;
		}

		static TestSuite& SharedInstance()
		{
			static TestSuite instance;
			return instance;
		}

	private:
		bool mIsVerbose;
		RegisteredTests mTests;
	};

	class AutoTestRegister
	{
	public:
		AutoTestRegister(const std::string& name, Test test)
		{
			TestSuite::SharedInstance().RegisterTest(name, test);
		}
	};
}

#define TEST(SUITENAME, TESTNAME)                             \
    void SUITENAME##__##TESTNAME();                            \
    static test::AutoTestRegister autoTestRegister_##SUITENAME##__##TESTNAME(#SUITENAME "__" #TESTNAME, SUITENAME##__##TESTNAME); \
    void SUITENAME##__##TESTNAME()

#define CHECK(X)                                              \
    if (!(X))                                                 \
    {                                                         \
        std::ostringstream os;                                \
        os << #X << " ";                                      \
        os << __FILE__ << ":" << __LINE__;                    \
        throw test::TestFailedException(os.str());            \
    }

#define CHECK_EQUAL(X, Y)                                     \
    if (!((X) == (Y)))                                        \
    {                                                         \
        std::ostringstream os;                                \
        os << "(" << #X << " == " << #Y << ") ";              \
        os << "expected:" << (X) << " actual:" << (Y) << " "; \
        os << __FILE__ << ":" << __LINE__;                    \
        throw test::TestFailedException(os.str());            \
    }

#define NOT_IMPLEMENTED() throw test::TestFailedException("Not Implemented");
