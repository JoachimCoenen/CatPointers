#include "autoTest.h"

#include <vector>
#include <iostream>

namespace cat::autoTest
{

QList<TestBase*>& getAllTests() {
    static QList<TestBase*> allTests;
	return allTests;
}

int _argc;
char** _argv;


int get_argc() { return _argc; }
char** get_argv() { return _argv; }

void addTest(TestBase* test) {
    getAllTests().insert(0, test);
}

int run(int argc, char *argv[]) {
	_argc = argc;
	_argv = argv;
    QList<TestBase*> failures;
	int result = 0;
	for (TestBase* test: getAllTests()) {
        TESTLIB_SELFCOVERAGE_START(test->getName().c_str());
        auto testResult = test->run();
        result |= testResult;
        if (testResult != 0) {
            failures.append(test);
        }
	}
	if (result == 0) {
		std::cout << "SUCCESS running all tests." << std::endl;
	} else {
        std::cout << "FAILURE in some tests:" << std::endl;
        foreach (auto* failure, failures) {
            std::cout << "    " << failure->getName() << std::endl;
        }
	}
	return result;
}

void TestBase::_addEndlToCout()
{
	std::cout << std::endl;
}

}

int main(int argc, char *argv[])
{
	QTEST_SET_MAIN_SOURCE_PATH
	return cat::autoTest::run(argc, argv);
}
