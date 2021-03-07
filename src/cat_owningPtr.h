#ifndef CAT_OWNINGPTR_H
#define CAT_OWNINGPTR_H

#include "cat_weakPtr.h"


namespace cat {


template <class T_>
struct OwningPtr {
public:
	using T = T_;

private:
	T* _ptr;

    template<typename T2_>
    friend struct OwningPtr;

public:
	OwningPtr() noexcept : _ptr(nullptr) {}
	OwningPtr(T* ptr) noexcept : _ptr(ptr) {}

	template<class... Args_>
	OwningPtr(InplaceConstructorTag, Args_&&... __args){
		_ptr = new T{std::forward<Args_>(__args)...};
	}

	OwningPtr(const OwningPtr& other) = delete;
	OwningPtr(OwningPtr&& other) noexcept : _ptr(std::move(other._ptr)) {
		other._ptr = nullptr;
	}

	template<class T2_, std::enable_if_t<std::is_base_of_v<T, T2_>, int> = 0>
    OwningPtr(OwningPtr<T2_>&& other) noexcept : _ptr(std::move(other._ptr)) {
        other._ptr = nullptr;
	}

	~OwningPtr() {
		reset();
	}

public:
	OwningPtr& operator =(const OwningPtr& other) = delete;
	OwningPtr& operator =(OwningPtr&& other) {
		// maybe use a std::swap of _ptrs instead of calling reset?
		reset(other._ptr);
		other._ptr = nullptr;
		return *this;
	}

public:
    WeakPtr<T> getWeak() {
		return WeakPtr(_ptr);
	}

    WeakPtr<const T> getWeak() const {
		return WeakPtr<const T>(_ptr);
	}

	/**
	 *  @brief  Performs a dynamic_cast<>().
	 */
	template<class T2_>
	WeakPtr<T2_> as() const {
		return WeakPtr(dynamic_cast<T2_*>(_ptr));
	}

	/**
	 *  @brief  Performs a static_cast<>().
	 */
	template<class T2_>
	auto asStatic() const -> WeakPtr<T2_> {
		return WeakPtr<T2_>(static_cast<T2_*>(_ptr));
	}


	/**
	 *  @brief  Performs a dynamic_cast<>().
	 */
	template<class T2_, std::enable_if_t<std::is_polymorphic_v<T> && std::is_polymorphic_v<T2_>, int> = 0>
	OwningPtr<T2_> asOwning() { // maybe noexcept?
		// what if _ptr is nullptr?
		if (auto result = as<T2_>()) {
			_ptr = nullptr;
			return {result.___getPtr()};
		} else {
			return nullptr;
		}
	}


	/**
	 *  @brief  Performs a static_cast<>().
	 */
	template<class T2_, std::enable_if_t<std::is_polymorphic_v<T> && std::is_polymorphic_v<T2_>, int> = 0>
	OwningPtr<T2_> asOwningStatic() { // maybe noexcept?
		auto result = asStatic<T2_>();
		_ptr = nullptr;
		return {result.___getPtr()};
	}

	inline T& operator *() noexcept { return *_ptr; }
	inline const T& operator *() const noexcept { return *_ptr; }

	inline T* operator ->() noexcept { return _ptr; }
	inline const T* operator ->() const noexcept { return _ptr; }

	inline bool operator ==(const OwningPtr& other) const noexcept { return _ptr == other._ptr; }
	inline bool operator ==(std::nullptr_t) const noexcept { return _ptr == nullptr; }

	inline bool operator !=(const OwningPtr& other) const noexcept { return _ptr != other._ptr; }
	inline bool operator !=(std::nullptr_t) const noexcept { return _ptr != nullptr; }

	explicit operator bool () const noexcept { return _ptr != nullptr; }

    inline T* ___getPtr() noexcept {
		return _ptr;
	}

    inline T* ___getPtr() const noexcept {
		return _ptr;
	}
protected:
   inline void reset(T* newPtr = nullptr) {
		if (_ptr != nullptr) {
			delete _ptr;
		}
		_ptr = newPtr;
	}

};

}

template <class T_>
struct std::hash<cat::OwningPtr<T_>> {
	size_t operator()(const cat::OwningPtr<T_>& v) const noexcept {
		return std::hash<T_*>()(v.___getPtr());
	}
};


#endif // CAT_OWNINGPTR_H
