#include "stdafx.h"

int main()
{
	auto& suite = test::TestSuite::SharedInstance();
	suite.SetVerbose(true);
	suite.RunAllTests();

	getchar();
    return 0;
}

