#ifndef AUTOTEST_H
#define AUTOTEST_H

#include <qobject.h>
#include <QtTest>

#include <string>

namespace cat::autoTest
{
int get_argc();
char** get_argv();

class TestBase {
public:
	virtual ~TestBase() {}

	virtual std::string getName() = 0;
	virtual int run() = 0;

protected:
	void _addEndlToCout();
};

void addTest(TestBase*);

int run(int argc, char *argv[]);


template <class T_>
class Test final : public TestBase {

	std::string _name;
	T_ test;
public:
	Test(const std::string& name)
		: _name(name),
		  test()
	{
		addTest(this);
	}

	virtual std::string getName() override { return _name; }

	virtual int run() override {
		auto resut = QTest::qExec(&test, get_argc(), get_argv());
		_addEndlToCout();
		return resut;
	};
};

#define CAT_DECLARE_TEST(testClass) static cat::autoTest::Test<testClass> t_##testClass(#testClass)

}


#endif // AUTOTEST_H
