#include <QtTest>

#include "autoTest.h"

// add necessary includes here
#include "cat_sharedPtr.h"

using namespace cat;
using namespace cat::_sharedPtr_internal;

struct CTorMock {
	int* cntr;

	CTorMock(int* cntr): cntr(cntr) { (*cntr)++; }
	~CTorMock() { }
};

struct DTorMock {
	int* cntr;

	DTorMock(int* cntr): cntr(cntr) {}
	~DTorMock() { (*cntr)++; }
};

struct DTorMockVirtBase {
	virtual ~DTorMockVirtBase() { }
};

struct DTorMockVirt: DTorMockVirtBase {
	int* cntr;

	DTorMockVirt(int* cntr): cntr(cntr) {}
	virtual ~DTorMockVirt() { (*cntr)++; }
};


class SharedPtrRefCntSeparate_Test : public QObject
{
	Q_OBJECT

public:
	SharedPtrRefCntSeparate_Test() {}
	~SharedPtrRefCntSeparate_Test() {}

private slots:
	void initTestCase() {}
	void cleanupTestCase() {}

	void test_ctor() {
		SharedPtrRefCntSeparate_<int> obj{7};
        QCOMPARE(obj.getUsageCnt(), 7ull);
	}

	void test_incUsageCnt() {
		SharedPtrRefCntSeparate_<int> obj{7};
		obj.incUsageCnt();
        QCOMPARE(obj.getUsageCnt(), 8ull);
	}

	void test_decUsageCnt_1() {
		int* val = new int(5);
		SharedPtrRefCntSeparate_<int> obj{7};
		obj.decUsageCnt(val);
        QCOMPARE(obj.getUsageCnt(), 6ull);
		delete val;
	}

	void test_decUsageCnt_2() {
		int counter = 0;
		DTorMock* mock = new DTorMock(&counter);
		SharedPtrRefCntSeparate_<DTorMock>* obj = new SharedPtrRefCntSeparate_<DTorMock>{1};
		obj->decUsageCnt(mock);
		QCOMPARE(counter, 1);
	}

	void test_dispose_1() {
		int counter = 0;
		DTorMock* mock = new DTorMock(&counter);
		SharedPtrRefCntSeparate_<DTorMock>* obj = new SharedPtrRefCntSeparate_<DTorMock>{7};
		obj->dispose(mock);
		QCOMPARE(counter, 1);
	}

	void test_dispose_2() {
		int counter = 0;
		DTorMock* mock = new DTorMock(&counter);
		SharedPtrRefCnt_* obj = new SharedPtrRefCntSeparate_<DTorMock>{7};
		obj->dispose(mock);
		QCOMPARE(counter, 1);
	}

};
CAT_DECLARE_TEST(SharedPtrRefCntSeparate_Test);


class SharedPtrRefCntInplace_Test : public QObject
{
	Q_OBJECT

public:
	SharedPtrRefCntInplace_Test() {}
	~SharedPtrRefCntInplace_Test() {}

private slots:
	void initTestCase() {}
	void cleanupTestCase() {}

	void test_ctor() {
		int counter = 0;
		SharedPtrRefCntInplace_<CTorMock> obj{8, &counter};
        QCOMPARE(obj.getUsageCnt(), 8ull);
		QCOMPARE(counter, 1);
	}

	void test_incUsageCnt() {
		SharedPtrRefCntInplace_<int> obj{8, 0};
		obj.incUsageCnt();
        QCOMPARE(obj.getUsageCnt(), 9ull);
	}

	void test_decUsageCnt_1() {
		SharedPtrRefCntInplace_<int> obj{8, 0};
		obj.decUsageCnt(nullptr);
        QCOMPARE(obj.getUsageCnt(), 7ull);
	}

	void test_decUsageCnt_2() {
		int counter = 0;
		SharedPtrRefCntInplace_<DTorMock>* obj = new SharedPtrRefCntInplace_<DTorMock>{1, &counter};
		obj->decUsageCnt(nullptr);
		QCOMPARE(counter, 1);
	}

	void test_dispose_1() {
		int counter = 0;
		SharedPtrRefCntInplace_<DTorMock>* obj = new SharedPtrRefCntInplace_<DTorMock>{8, &counter};
		obj->dispose(nullptr);
		QCOMPARE(counter, 1);
	}

   void test_dispose_2() {
	   int counter = 0;
	   SharedPtrRefCnt_* obj = new SharedPtrRefCntInplace_<DTorMock>{8, &counter};
	   obj->dispose(nullptr);
	   QCOMPARE(counter, 1);
   }

   void test_get() {
	   SharedPtrRefCntInplace_<int>* obj = new SharedPtrRefCntInplace_<int>{8};
	   QCOMPARE(obj->get(), &obj->data);
   }

};
CAT_DECLARE_TEST(SharedPtrRefCntInplace_Test);


class SharedPtrData_Test : public QObject
{
	Q_OBJECT

public:
	SharedPtrData_Test() {}
	~SharedPtrData_Test() {}

private slots:
	void initTestCase() {}
	void cleanupTestCase() {}

	void test_ctor_1() {
		SharedPtrData_<int> obj{nullptr};
		QCOMPARE(obj.refCnt, nullptr);
		QCOMPARE(obj.payload, nullptr);
	}

	void test_ctor_2() {
		SharedPtrRefCntInplace_<int> refCnt = SharedPtrRefCntInplace_<int>(8);
		SharedPtrData_<int> obj{&refCnt, refCnt.get()};
        QCOMPARE(refCnt.getUsageCnt(), 9ull);
		QCOMPARE(obj.payload, refCnt.get());
		QCOMPARE(obj.refCnt, &refCnt);
	}

	void test_ctor_3() {
		SharedPtrRefCntInplace_<int> refCnt = SharedPtrRefCntInplace_<int>(8);
		SharedPtrData_<int> obj1{&refCnt, refCnt.get()};
		SharedPtrData_<int> obj2{obj1};
        QCOMPARE(refCnt.getUsageCnt(), 10ull);
		QCOMPARE(obj1.payload, refCnt.get());
		QCOMPARE(obj1.refCnt, &refCnt);
		QCOMPARE(obj2.payload, refCnt.get());
		QCOMPARE(obj2.refCnt, &refCnt);
	}

	void test_ctor_4() {
		SharedPtrRefCntInplace_<int> refCnt = SharedPtrRefCntInplace_<int>(8);
		SharedPtrData_<int> obj1{&refCnt, refCnt.get()};
		SharedPtrData_<int> obj2{std::move(obj1)};
        QCOMPARE(refCnt.getUsageCnt(), 9ull);
		QCOMPARE(obj1.payload, nullptr);
		QCOMPARE(obj1.refCnt, nullptr);
		QCOMPARE(obj2.payload, refCnt.get());
		QCOMPARE(obj2.refCnt, &refCnt);
	}

	void test_set_1() {
		SharedPtrRefCntInplace_<int> refCnt = SharedPtrRefCntInplace_<int>(8);

		SharedPtrData_<int> obj1{nullptr};
		obj1.set(&refCnt, refCnt.get());
        QCOMPARE(refCnt.getUsageCnt(), 9ull);
		QCOMPARE(obj1.payload, refCnt.get());
		QCOMPARE(obj1.refCnt, &refCnt);
	}

	void test_set_2() {
		SharedPtrRefCntInplace_<int> refCnt1 = SharedPtrRefCntInplace_<int>(8);
		SharedPtrRefCntInplace_<int> refCnt2 = SharedPtrRefCntInplace_<int>(8);

		SharedPtrData_<int> obj1{&refCnt1, refCnt1.get()};
		obj1.set(&refCnt2, refCnt2.get());
        QCOMPARE(refCnt1.getUsageCnt(), 8ull);
        QCOMPARE(refCnt2.getUsageCnt(), 9ull);
		QCOMPARE(obj1.payload, refCnt2.get());
		QCOMPARE(obj1.refCnt, &refCnt2);
	}

	void test_set_3() {
		SharedPtrRefCntInplace_<int> refCnt = SharedPtrRefCntInplace_<int>(8);

		SharedPtrData_<int> obj1{&refCnt, refCnt.get()};
		obj1.set(&refCnt, refCnt.get());
        QCOMPARE(refCnt.getUsageCnt(), 9ull);
		QCOMPARE(obj1.payload, refCnt.get());
		QCOMPARE(obj1.refCnt, &refCnt);
	}

	void test_set_4() {
		SharedPtrRefCntInplace_<int>* refCnt = new SharedPtrRefCntInplace_<int>(0);

		SharedPtrData_<int> obj1{refCnt, refCnt->get()};
		obj1.set(refCnt, refCnt->get());
        QCOMPARE(refCnt->getUsageCnt(), 1ull);
		QCOMPARE(obj1.payload, refCnt->get());
		QCOMPARE(obj1.refCnt, refCnt);
	}

	void test_set_5() {
		int counter = 0;
		SharedPtrRefCntInplace_<DTorMock>* refCnt = new SharedPtrRefCntInplace_<DTorMock>(0, &counter);

		SharedPtrData_<DTorMock> obj1{refCnt, refCnt->get()};
		obj1.set(nullptr, nullptr);
		QCOMPARE(counter, 1);
		QCOMPARE(obj1.payload, nullptr);
		QCOMPARE(obj1.refCnt, nullptr);
	}

	void test_reset_1() {
		int counter = 0;
		SharedPtrRefCntInplace_<DTorMock>* refCnt = new SharedPtrRefCntInplace_<DTorMock>(0, &counter);

		SharedPtrData_<DTorMock> obj1{refCnt, refCnt->get()};
		obj1.reset();
		QCOMPARE(counter, 1);
		QCOMPARE(obj1.payload, nullptr);
		QCOMPARE(obj1.refCnt, nullptr);
	}

	void test_reset_2() {
		SharedPtrData_<DTorMock> obj1{nullptr};
		obj1.reset();
		QCOMPARE(obj1.payload, nullptr);
		QCOMPARE(obj1.refCnt, nullptr);
	}

	void test_swap() {
		SharedPtrRefCntInplace_<int>* refCnt = new SharedPtrRefCntInplace_<int>(0);
		SharedPtrData_<int> obj1{nullptr};
		SharedPtrData_<int> obj2{refCnt, refCnt->get()};
		obj1.swap(obj2);
		QCOMPARE(obj1.payload, refCnt->get());
		QCOMPARE(obj1.refCnt, refCnt);
		QCOMPARE(obj2.payload, nullptr);
		QCOMPARE(obj2.refCnt, nullptr);
	}

	void test_isNull_1() {
		SharedPtrData_<DTorMock> obj1{nullptr};
		QCOMPARE(obj1.isNull(), true);
	}

	void test_isNull_2() {
		SharedPtrRefCntInplace_<int>* refCnt = new SharedPtrRefCntInplace_<int>(0);
		SharedPtrData_<int> obj1{refCnt, refCnt->get()};
		QCOMPARE(obj1.isNull(), false);
	}

};
CAT_DECLARE_TEST(SharedPtrData_Test);


struct PointBase { virtual ~PointBase() {} };
struct Point: PointBase { int x, y;  Point(int x, int y) : x(x), y(y) {}; };

struct LineBase { virtual ~LineBase() {} };

class SharedPtrTest : public QObject
{
	Q_OBJECT

public:
	SharedPtrTest() {}
	~SharedPtrTest() {}

private slots:
	void initTestCase() {}
	void cleanupTestCase() {}

	void test_ctor_1() {
		SharedPtr<int> shrPtr{nullptr};
		QCOMPARE(shrPtr, nullptr);
	}

	void test_ctor_2() {
		SharedPtr<int> shrPtr{{}};
		QVERIFY(shrPtr != nullptr);
	}

	void test_ctor_3() {
		int counter = 0;
		SharedPtr<CTorMock> shrPtr{{}, &counter};
		QCOMPARE(counter, 1);
	}

	void test_ctor_4() {
		SharedPtr<int> shrPtr{{}, 77};
		QCOMPARE(*shrPtr, 77);
	}

	void test_as_1() {
		SharedPtr<Point> shrPtr({}, 3, 5);
		auto base = shrPtr.as<PointBase>();
		QCOMPARE(base.___getPtr(), shrPtr.___getPtr());
	}

	void test_as_2() {
		SharedPtr<Point> shrPtr({}, 3, 5);
		auto base = shrPtr.as<LineBase>();
		QCOMPARE(base.___getPtr(), nullptr);
	}

	void test_as_3() {
		SharedPtr<Point> shrPtr{nullptr};
		auto base = shrPtr.as<PointBase>();
		QCOMPARE(base.___getPtr(), nullptr);
	}

	void test_asShared_1() {
		SharedPtr<Point> shrPtr({}, 3, 5);
		auto base = shrPtr.asShared<PointBase>();
		QCOMPARE(base.___getPtr(), shrPtr.___getPtr());
	}

	void test_asShared_2() {
		SharedPtr<Point> shrPtr({}, 3, 5);
		auto base = shrPtr.asShared<LineBase>();
		QCOMPARE(base.___getPtr(), nullptr);
	}

	void test_asShared_3() {
		SharedPtr<Point> shrPtr{nullptr};
		auto base = shrPtr.asShared<PointBase>();
		QCOMPARE(base.___getPtr(), nullptr);
	}

	void test_asShared_4() {
		int counter = 0;
		SharedPtr<DTorMockVirt> shrPtr{{}, &counter};
		auto basePtr = shrPtr.asShared<DTorMockVirtBase>();
		shrPtr = nullptr;
		QCOMPARE(counter, 0);
		basePtr = nullptr;
		QCOMPARE(counter, 1);
	}

	void test_operator_bool() {
		SharedPtr<int> shrPtr{nullptr};
		QVERIFY(not (bool)shrPtr);
	}

	void test_case1() {
		SharedPtr<Point> p({}, 7, -7);
	}

};
CAT_DECLARE_TEST(SharedPtrTest);



#include "sharedPtrTest.moc"

