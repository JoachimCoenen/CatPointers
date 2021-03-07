#ifndef CAT_SHAREDPTR_H
#define CAT_SHAREDPTR_H

#include "cat_weakPtr.h"

#include <cstddef>
#include <utility>

//#include <memory>
//std::make_shared()

namespace cat {

namespace _sharedPtr_internal {

/**
 * SharedPtrRefCnt_ is a self-deleting type. i.e. it deconstructs itself,
 * when _usageCnt reaches zero.
 */
struct SharedPtrRefCnt_ {
public:
	using CntT = size_t;
private:
	mutable CntT _usageCnt;

protected:
	SharedPtrRefCnt_(CntT usageCnt): _usageCnt(usageCnt) {}

public:
	SharedPtrRefCnt_() = delete;
	SharedPtrRefCnt_(const SharedPtrRefCnt_&) = delete;
	SharedPtrRefCnt_(SharedPtrRefCnt_&&) = delete;
	SharedPtrRefCnt_& operator =(const SharedPtrRefCnt_&) const = delete;
	SharedPtrRefCnt_& operator =(SharedPtrRefCnt_&&) = delete;

	virtual void dispose(void* payload) = 0;

	virtual ~SharedPtrRefCnt_() {};

	inline const CntT& getUsageCnt() const noexcept { return _usageCnt; }
	inline void incUsageCnt() const noexcept { _usageCnt += 1; }
	inline void decUsageCnt(void* payload) {
		_usageCnt -= 1;
		if (_usageCnt == 0) {
			dispose(payload);
		}
	}
};


template <class T_>
struct SharedPtrRefCntSeparate_ final: public SharedPtrRefCnt_ {
public:
	using T = T_;

public:
	SharedPtrRefCntSeparate_(typename SharedPtrRefCnt_::CntT usageCnt): SharedPtrRefCnt_(usageCnt) {}
	// delete them all:
	SharedPtrRefCntSeparate_() = delete;
	SharedPtrRefCntSeparate_(const SharedPtrRefCntSeparate_&) = delete;

	virtual void dispose(void* payload) override {
		T* data = reinterpret_cast<T*>(payload);
		delete data;
		delete this;
	}

	virtual ~SharedPtrRefCntSeparate_() { /* NOP() */}
};


template <class T_>
struct SharedPtrRefCntInplace_ final: public SharedPtrRefCnt_ {
public:
	using T = T_;
public:
	mutable T  data;

	SharedPtrRefCntInplace_(typename SharedPtrRefCnt_::CntT usageCnt): SharedPtrRefCnt_(usageCnt) {}
	// delete them all:
	SharedPtrRefCntInplace_() = delete;
	SharedPtrRefCntInplace_(const SharedPtrRefCntInplace_&) = delete;

	template <class... Args>
	explicit SharedPtrRefCntInplace_(typename SharedPtrRefCnt_::CntT usageCnt, Args&& ...args)
		: SharedPtrRefCnt_(usageCnt),
		  data{std::forward<Args>(args)...}
	{}

	virtual void dispose([[maybe_unused]]void* payload) override {
		delete this;
	}

	virtual ~SharedPtrRefCntInplace_() { /* NOP() */}

	T* get() { return &data; }
};


template <class T_>
struct SharedPtrData_ {
	WeakPtr<SharedPtrRefCnt_> refCnt = nullptr;
	WeakPtr<T_> payload = nullptr;

	inline SharedPtrData_(std::nullptr_t) noexcept
	{ }

	inline SharedPtrData_(WeakPtr<SharedPtrRefCnt_> refCnt, WeakPtr<T_> payload) noexcept
		: refCnt(refCnt),
		  payload(payload)
	{
		_incRefCnt(refCnt);
	}

	inline SharedPtrData_(const SharedPtrData_& other) noexcept
		: SharedPtrData_(other.refCnt, other.payload)
	{}

	template<class T2_, std::enable_if_t<std::is_base_of_v<T_, T2_>, int> = 0>
	inline explicit SharedPtrData_(const SharedPtrData_<T2_>& other) noexcept
		: SharedPtrData_(other.refCnt, other.payload)
	{}

	inline SharedPtrData_( SharedPtrData_&& other) noexcept
		: refCnt(std::move(other.refCnt)),
		  payload(std::move(other.payload))
	{
		other.refCnt = nullptr;
		other.payload = nullptr;
	}

	inline void set(const SharedPtrData_& other) {
		set(other.refCnt, other.payload);
	}

	void set(WeakPtr<SharedPtrRefCnt_> refCnt, WeakPtr<T_> payload) {
		auto oldRefCnt = this->refCnt;
		_incRefCnt(refCnt);
		this->refCnt = refCnt;
		this->payload = payload;
		_decRefCnt(oldRefCnt);
	}

	void reset() {
		_decRefCnt(this->refCnt);
		this->refCnt = nullptr;
		this->payload = nullptr;
	}

	inline void swap(SharedPtrData_& other) noexcept {
		std::swap(refCnt, other.refCnt);
		std::swap(payload, other.payload);
	}

	inline bool isNull() const noexcept {
		return this->refCnt == nullptr;
	}

private:
	void _incRefCnt(const WeakPtr<SharedPtrRefCnt_> ptr) const  noexcept {
		if (ptr != nullptr) {
			ptr->incUsageCnt();
		}
	}

	void _decRefCnt(WeakPtr<SharedPtrRefCnt_> ptr) {
		if (ptr != nullptr) {
			ptr->decUsageCnt(payload.___getPtr());
		}
	}
};

}

template <class T_>
struct SharedPtr {
public:
	using T = T_;
private:
	_sharedPtr_internal::SharedPtrData_<T> _ptrData;

	template<typename T2_>
	friend struct SharedPtr;

public:

	SharedPtr(const SharedPtr& other)
		: _ptrData(other._ptrData)
	{}

	SharedPtr(SharedPtr&& other) noexcept
		: _ptrData(std::move(other._ptrData))
	{}

	template<class T2_, std::enable_if_t<std::is_base_of_v<T, T2_> && std::is_polymorphic_v<T>, int> = 0>
	SharedPtr(const SharedPtr<T2_>& other)
		: _ptrData(other._ptrData)
	{
		auto* staticCheckConvertabiity = static_cast<T*>(static_cast<T2_*>(nullptr));
	}

	template <typename... Args>
	explicit SharedPtr(InplaceConstructorTag, Args&& ...args)
		: _ptrData(nullptr)
	{
		auto* refCntPtr = new _sharedPtr_internal::SharedPtrRefCntInplace_<T>(0, std::forward<Args>(args)...);
		_ptrData.set(refCntPtr, &refCntPtr->data);
	}

	template<class TNullptr, std::enable_if_t<std::is_same_v<TNullptr, std::nullptr_t>, int> = 0>
	SharedPtr(TNullptr _) noexcept : _ptrData(_) {}

	~SharedPtr() {
		this->_ptrData.reset();  // strictly necessary!
	}

public:
	SharedPtr& operator=(const SharedPtr& other) {
		SharedPtr tmp(other);
		tmp.swap(*this);  // swaps moved to swap method.
		return *this;
	}

	SharedPtr& operator=(SharedPtr&& other) noexcept {
		other.swap(*this);  // swaps moved to swap method.
		return *this;
	}

public:

	inline void swap(SharedPtr& other) noexcept {
		_ptrData.swap(other._ptrData);
	}

    WeakPtr<T> getWeak() {
		return WeakPtr<T>(___getPtr());
	}

    WeakPtr<const T> getWeak() const {
		return WeakPtr<const T>(___getPtr());
	}

	/**
	 *  @brief  Performs a dynamic_cast<>().
	 */
	template<class T2_>
	auto as() const -> WeakPtr<T2_> {
		return WeakPtr<T2_>(dynamic_cast<T2_*>(___getPtr()));
	}

	/**
	 *  @brief  Performs a static_cast<>().
	 */
	template<class T2_>
	auto asStatic() const -> WeakPtr<T2_> {
		return WeakPtr<T2_>(static_cast<T2_*>(___getPtr()));
	}

	/**
	 *  @brief  Performs a dynamic_cast<>().
	 */
	template<class T2_, std::enable_if_t<std::is_polymorphic_v<T> && std::is_polymorphic_v<T2_>, int> = 0>
	SharedPtr<T2_> asShared() const {
		auto castPtr = as<T2_>();

		if (castPtr) {
			auto result = SharedPtr<T2_>(nullptr);
			result._ptrData.set(this->_ptrData.refCnt, castPtr);
			return result;
		}
		return SharedPtr<T2_>(nullptr);
	}

	/**
	 *  @brief  Performs a static_cast<>().
	 */
	template<class T2_, std::enable_if_t<std::is_polymorphic_v<T> && std::is_polymorphic_v<T2_>, int> = 0>
	SharedPtr<T2_> asSharedStatic() const {
		auto castPtr = asStatic<T2_>();

		auto result = SharedPtr<T2_>(nullptr);
		result._ptrData.set(this->_ptrData.refCnt, castPtr);
		return result;
	}

	inline T& operator*() noexcept { return *___getPtr(); }
	inline const T& operator*() const noexcept { return *___getPtr(); }

	inline T* operator->() noexcept { return ___getPtr(); }
	inline const T* operator->() const noexcept { return ___getPtr(); }

	bool operator ==(const SharedPtr& other) const noexcept { return _ptrData.refCnt == other._ptrData.refCnt; }
	bool operator ==(std::nullptr_t) const noexcept { return _ptrData.isNull(); }

	bool operator !=(const SharedPtr& other) const noexcept { return _ptrData.refCnt != other._ptrData.refCnt; }
	bool operator !=(std::nullptr_t) const noexcept { return not _ptrData.isNull(); }

	explicit operator bool () const noexcept { return not _ptrData.isNull(); }

	inline T* ___getPtr() noexcept {
		return _ptrData.payload.___getPtr();
    }

    inline T*  ___getPtr() const noexcept {
		return _ptrData.payload.___getPtr();
	}
};

}


template <class T_>
struct std::hash<cat::SharedPtr<T_>> {
	size_t operator()(const cat::SharedPtr<T_>& v) const noexcept {
		return std::hash<T_*>()(v.___getPtr());
	}
};


#endif // CAT_SHAREDPTR_H
