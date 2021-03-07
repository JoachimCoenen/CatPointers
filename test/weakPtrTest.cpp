#include <QtTest>

#include "autoTest.h"

// add necessary includes here
#include "cat_weakPtr.h"

using namespace cat;

struct IGeometry {
    size_t id;

    IGeometry() {
        static size_t nextId = 0;
        this->id = nextId;
        ++nextId;
    }

    virtual ~IGeometry() {}
};

struct PointBase { };

struct Point2: PointBase, IGeometry { int x, y;  Point2(int x, int y) : IGeometry(), x(x), y(y) {}; };

struct LineBase { };

struct Line: PointBase, IGeometry {
    Point2 p1, p2;
    Line(Point2 p1, Point2 p2)
        : IGeometry(), p1(p1), p2(p2)
    {};
};

class WeakPtrTest : public QObject
{
    Q_OBJECT

public:
    WeakPtrTest() {}
    ~WeakPtrTest() {}

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}

    void test_ctor_1() {
        WeakPtr<int> ptr{nullptr};
        QCOMPARE(ptr, nullptr);
    }

    void test_ctor_2() {
        int val = 0;
        WeakPtr<int> ptr{&val};
        QVERIFY(ptr != nullptr);
    }

    void test_ctor_3() {
        int val = 0;
        WeakPtr<int> ptr1{&val};
        WeakPtr<int> ptr2{ptr1};
        QCOMPARE(ptr1.___getPtr(), &val);
        QCOMPARE(ptr2.___getPtr(), &val);
    }

    void test_ctor_4() {
        int val = 0;
        WeakPtr<int> ptr1{&val};
        WeakPtr<int> ptr2{std::move(ptr1)};
        QCOMPARE(ptr1.___getPtr(), nullptr);
        QCOMPARE(ptr2.___getPtr(), &val);
    }

    void test_assign_1() {
        int val = 0;
        WeakPtr<int> ptr1{&val};
        WeakPtr<int> ptr2;
        ptr2 = ptr1;
        QCOMPARE(ptr1.___getPtr(), &val);
        QCOMPARE(ptr2.___getPtr(), &val);
    }

    void test_assign_2() {
        int val = 0;
        WeakPtr<int> ptr1{&val};
        WeakPtr<int> ptr2;
        ptr2 = std::move(ptr1);
        QCOMPARE(ptr1.___getPtr(), nullptr);
        QCOMPARE(ptr2.___getPtr(), &val);
    }

    void test_assign_3() {
        Point2 val{-5, 5};
        WeakPtr<Point2> ptr1{&val};
        WeakPtr<IGeometry> ptr2;
        ptr2 = std::move(ptr1);
        QCOMPARE(ptr1.___getPtr(), nullptr);
        QCOMPARE(ptr2.___getPtr(), &val);
        QCOMPARE(ptr2->id, val.id);
    }

    void test_conversionOperator() {
        Point2 val{-5, 5};
        WeakPtr<Point2> ptr1{&val};
        WeakPtr<IGeometry> ptr2;
        ptr2 = (WeakPtr<IGeometry>)ptr1;
        QCOMPARE(ptr1.___getPtr(), &val);
        QCOMPARE(ptr2.___getPtr(), &val);
        QCOMPARE(ptr2->id, val.id);
    }

    void test_as_1() {
        Point2 val{3, 5};
        WeakPtr<Point2> ptr(&val);
        auto base = ptr.as<PointBase>();
        QCOMPARE(base.___getPtr(), ptr.___getPtr());
    }

    void test_as_2() {
        Point2 val{3, 5};
        WeakPtr<Point2> ptr(&val);
        auto base = ptr.as<LineBase>();
        QCOMPARE(base.___getPtr(), nullptr);
    }

    void test_as_3() {
        WeakPtr<Point2> ptr{nullptr};
        auto base = ptr.as<PointBase>();
        QCOMPARE(base.___getPtr(), nullptr);
    }

    void test_operator_bool() {
        WeakPtr<int> ptr{nullptr};
        QVERIFY(not (bool)ptr);
    }

};
CAT_DECLARE_TEST(WeakPtrTest);



#include "weakPtrTest.moc"

