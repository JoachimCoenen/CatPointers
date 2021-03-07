#include <QtTest>

#include "autoTest.h"

// add necessary includes here
#include "cat_owningPtr.h"

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

class OwningPtrTest : public QObject
{
    Q_OBJECT

public:
    OwningPtrTest() {}
    ~OwningPtrTest() {}

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}

    void test_ctor_1() {
        OwningPtr<int> ptr{nullptr};
        QCOMPARE(ptr, nullptr);
    }

    void test_ctor_2() {
        int val = 0;
        OwningPtr<int> ptr{{}, val};
        QVERIFY(ptr != nullptr);
        QCOMPARE(*ptr, val);
    }

    void test_ctor_4() {
        int val = 0;
        OwningPtr<int> ptr1{{}, val};
        OwningPtr<int> ptr2{std::move(ptr1)};
        QCOMPARE(ptr1.___getPtr(), nullptr);
        QCOMPARE(*ptr2, val);
    }

    void test_assign_2() {
        int val = 0;
        OwningPtr<int> ptr1{{}, val};
        OwningPtr<int> ptr2;
        ptr2 = std::move(ptr1);
        QCOMPARE(ptr1.___getPtr(), nullptr);
        QCOMPARE(*ptr2, val);
    }

    void test_assign_3() {
        OwningPtr<Point2> ptr1{{}, -5, 5};
        OwningPtr<IGeometry> ptr2;
        auto* ptr1Ptr = ptr1.___getPtr();
        ptr2 = std::move(ptr1);
        QCOMPARE(ptr1.___getPtr(), nullptr);
        QCOMPARE(ptr2.___getPtr(), ptr1Ptr);
        QCOMPARE(ptr2->id, ptr1Ptr->id);
    }

    void test_as_1() {
        OwningPtr<Point2> ptr{{}, -5, 5};
        auto base = ptr.as<PointBase>();
        QCOMPARE(base.___getPtr(), ptr.___getPtr());
    }

    void test_as_2() {
        OwningPtr<Point2> ptr{{}, -5, 5};
        auto base = ptr.as<LineBase>();
        QCOMPARE(base.___getPtr(), nullptr);
    }

    void test_as_3() {
        OwningPtr<Point2> ptr{nullptr};
        auto base = ptr.as<PointBase>();
        QCOMPARE(base.___getPtr(), nullptr);
    }

    void test_operator_bool() {
        OwningPtr<int> ptr{nullptr};
        QVERIFY(not (bool)ptr);
    }

};
CAT_DECLARE_TEST(OwningPtrTest);



#include "owningPtrTest.moc"

