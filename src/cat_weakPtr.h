#ifndef CAT_WEAKPTR_H
#define CAT_WEAKPTR_H

#include <utility>
#include <type_traits>

namespace cat {

struct InplaceConstructorTag final {};

template <class T_>
struct WeakPtr {
public:
	using T = T_;

private:
	T* _ptr;

    template<typename T2_>
    friend struct WeakPtr;

public:
	WeakPtr() noexcept : _ptr(nullptr) {}
	WeakPtr(T* ptr) noexcept : _ptr(ptr) {}

	WeakPtr(const WeakPtr& other) : _ptr(other._ptr) {}
    WeakPtr(WeakPtr&& other) noexcept : _ptr(std::move(other._ptr)) {
        other._ptr = nullptr;
    }

	template<class T2_, std::enable_if_t<std::is_base_of_v<T, T2_>, int> = 0>
	explicit WeakPtr(WeakPtr<T2_>&& other) noexcept : _ptr(std::move(other.___getPtr())) {
        other._ptr = nullptr;
	}

	~WeakPtr() { /* NOP() */ }

    WeakPtr& operator =(const WeakPtr& other) {
        _ptr = other._ptr;
        return *this;
    }
    WeakPtr& operator =(WeakPtr&& other) noexcept {
        _ptr = std::move(other._ptr);
        other._ptr = nullptr;
        return *this;
    }

	template<class T2_, std::enable_if_t<std::is_base_of_v<T, T2_>, int> = 0>
    WeakPtr& operator =(WeakPtr<T2_>&& other) noexcept {
        _ptr = std::move(other._ptr);
        other._ptr = nullptr;
        return *this;
    }

public:
	template<class T2_, std::enable_if_t<std::is_base_of_v<T2_, T>, int> = 0>
	operator WeakPtr<T2_>(){
		return WeakPtr<T2_>(static_cast<T*>(_ptr));
	}

	template<class T2_, std::enable_if_t<std::is_base_of_v<T2_, T>, int> = 0>
	operator WeakPtr<const T2_>() {
		return WeakPtr<const T2_>(static_cast<const T*>(_ptr));
	}

	template<class T2_, std::enable_if_t<std::is_base_of_v<T2_, T>, int> = 0>
	operator const WeakPtr<T2_>() const {
		return WeakPtr<T2_>(static_cast<T*>(_ptr));
	}

	template<class T2_, std::enable_if_t<std::is_base_of_v<T2_, T>, int> = 0>
	operator const WeakPtr<const T2_>() const {
		return WeakPtr<const T2_>(static_cast<const T*>(_ptr));
	}

	/**
	 *  @brief  Performs a dynamic_cast<>().
	 */
	template<class T2_>
	WeakPtr<T2_> as() const {
		return WeakPtr<T2_>(dynamic_cast<T2_*>(_ptr));
	}

	/**
	 *  @brief  Performs a static_cast<>().
	 */
	template<class T2_>
	WeakPtr<T2_> asStatic() const {
		return WeakPtr<T2_>(static_cast<T2_*>(_ptr));
	}

	T& operator *() noexcept { return *_ptr; }
	const T& operator *() const noexcept { return *_ptr; }

	T* operator ->() noexcept { return _ptr; }
	const T* operator ->() const noexcept { return _ptr; }

	bool operator ==(const WeakPtr& other) const noexcept { return _ptr == other._ptr; }
	bool operator ==(std::nullptr_t) const noexcept { return _ptr == nullptr; }

	bool operator !=(const WeakPtr& other) const noexcept { return _ptr != other._ptr; }
	bool operator !=(std::nullptr_t) const noexcept { return _ptr != nullptr; }

	explicit operator bool () const noexcept { return _ptr != nullptr; }

    inline T* ___getPtr() noexcept {
		return _ptr;
	}

    inline T* ___getPtr() const noexcept {
		return _ptr;
	}
};

}

template <class T_>
struct std::hash<cat::WeakPtr<T_>> {
	size_t operator()(const cat::WeakPtr<T_>& v) const noexcept {
		return std::hash<T_*>()(v.___getPtr());
	}
};


#endif // CAT_WEAKPTR_H
