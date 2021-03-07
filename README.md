# CatPointers
A smart pointer implementation for C++ with clear ownership semantics

# Features
 - `WeakPtr<T>`: Signifies non-ownership. The target won't be deleted when the pointer gets destroyed.
 - `OwningPtr<T>`: Signifies ownership. The target will be deleted when the pointer gets destroyed.
 - `SharedPtr<T>`: Signifies shared ownership. The target won't be deleted untill all shared pointers to it are destroyed.
 - clear ownership semntics
 - less verbos type casting using:
   - `myPtr.as<T>()` instead of `std::dynamic_pointer_cast<T*>(myPtr)`
   -  `myPtr.asStatic<T>()` instead of `std::static_pointer_cast<T*>(myPtr)`

## Pointer Alias Macros
These are macros that help reduce the clutter and verbosity of your code.
They are:

- `ADD_PTR_TYPES(Cls)`
- `PTRS_FOR_STRUCT(Cls)` and `PTRS_FOR_CLASS(Cls)`

- `ADD_PTR_TYPES_TEMPLATE(Cls, tmplParams, tmplArgs)`
- `PTRS_FOR_TEMPLATE_STRUCT(Cls, tmplParams, tmplArgs)` and `PTRS_FOR_TEMPLATE_CLASS(Cls, tmplParams, tmplArgs)`

and are used like this:
```c++
PTRS_FOR_CLASS(Circle)
class Circle: Geometry {...};
```
This is the same as:
```c++
class Circle;
ADD_PTR_TYPES(Circle)
class Circle: Geometry {...};
```
... and expands to:
```c++
class Circle;
using CirclePtr  = cat::OwningPtr<Circle>;
using CircleCPtr = cat::OwningPtr<const Circle>;
using CircleWeakPtr  = cat::WeakPtr<Circle>;
using CircleCWeakPtr = cat::WeakPtr<const Circle>;
using CircleSharedPtr  = cat::SharedPtr<Circle>;
using CircleCSharedPtr = cat::SharedPtr<const Circle>;
class Circle: Geometry {...};
```


## Casting
Casting is done using the `as<>()` and `asStatic<>()` methods of the pointers.
```c++
    auto WeakPtr<T>::as<T2>()         -> WeakPtr<T2> // performs a dynamc_cast<T2*>(...)
    auto WeakPtr<T>::asStatic<T2>()   -> WeakPtr<T2> // performs a static_cast<T2*>(...)
    
    auto OwningPtr<T>::as<T2>()       -> WeakPtr<T2> // performs a dynamc_cast<T2*>(...)
    auto OwningPtr<T>::asStatic<T2>() -> WeakPtr<T2> // performs a static_cast<T2*>(...)
    
    auto SharedPtr<T>::as<T2>()       -> WeakPtr<T2> // performs a dynamc_cast<T2*>(...)
    auto SharedPtr<T>::asStatic<T2>() -> WeakPtr<T2> // performs a static_cast<T2*>(...)
    auto SharedPtr<T>::asShared<T2>()       -> SharedPtr<T2> // performs a dynamc_cast<T2*>(...)
    auto SharedPtr<T>::asSharedStatic<T2>() -> SharedPtr<T2> // performs a static_cast<T2*>(...)
```
### Casting Examples
```c++
bool isCircle(GeometryCWeakPtr geo) {
    return geo.as<Circle>() != nullptr;
}

void setRadius(GeometryWeakPtr geo, double newRadius) {
    if (CircleWeakPtr circle = geo.as<Circle>()) {
        circle->radius = newRadius;
    } else {
        throw NotACircleException();
    }
}
```

## Constructors
```c++
class Circle;
PTR_TYPES_FOR_STRUCT(Circle)
struct Circle: Geometry {
    double radius;
    Point position
};
```
```c++
// WeakPtr<>
CircleWeakPtr circle1{}; // initializes to nullptr
CircleWeakPtr circle1{new Circle(...) }; // initializes with a pointer

// OwningPtr<>
CirclePtr circle1{}; // initializes to nullptr
CirclePtr circle1{new Circle(...) }; // initializes with a pointer and takes ownership of the target.
CirclePtr circle1{ {}, 7.5, Position(...) }; // creates a new Circle instance.
//                 ^ InplaceConstructorTag

// SharedPtr<>
CircleSharedPtr circle1{}; // initializes to nullptr
CircleSharedPtr circle1{ {}, 7.5, Position(...) }; // creates a new Circle instance.
//                       ^ InplaceConstructorTag
```

## Example: TreeNode

``` c++
#include "catPointers.h"
#include <vector>

PTRS_FOR_TEMPLATE_STRUCT(TreeNode, class T_, T_); // adds pointer aliases for a templated struct BEFORE acual type definition.
template <class T_>
struct TreeNode {
    using T = T_;
    ADD_PTR_TYPES(T); // adds pointer aliases for T.
protected:
    TPtr _data;
    std::vector<TreeNodePtr<T>> _children; // TreeNode owns its children
    TreeNodeWeakPtr<T> _parent; // TreeNode does not own its parent

public:
    TreeNode();

    TreeNodeWeakPtr<T> parent() { return _parent; }
    TreeNodeCWeakPtr<T> parent() const { return _parent; }

    TWeakPtr data() { return _data.weak(); }
    TCWeakPtr data() const { return _data.weak(); }

   void setData(TPtr&& data) {
        _data = std::move(data); // transfers ownership of data to this TreeNode;
    }

    TPtr removeData() {
        return std::move(_data); // transfers ownership and sets _data = nullptr;
    }

    TreeNodeWeakPtr<T> addChild(TreeNodePtr<T>&& child) {
        //assert(child->parent() == nullptr);
        _children.push_back(std::move(child));
        child->_parent = this;
        return child.weak();
    }

    TreeNodePtr<T> removeChild(size_t index) {
        auto result = std::move(_children.at(index)); // transfer owneship
        _children.erase(_children.begin() + index); // remove from list
        result->_parent = nullptr; // set parent to null
        return result; // transfers owneship because copy elision
    }

    const std::vector<TreeNodePtr<T>>& children() { return _children; }
    
};
```

