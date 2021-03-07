# CatPointers
A smart pointer implementation for C++

# Features
 - `WeakPtr<T>`: Signifies non-ownership. The target won't be deleted when the pointer gets destroyed.
 - `OwningPtr<T>`: Signifies ownership. The target will be deleted when the pointer gets destroyed.
 - `SharedPtr<T>`: Signifies shared ownership. The target won't be deleted untill all shared pointers to it are destroyed.


