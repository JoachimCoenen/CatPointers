#ifndef CAT_POINTERS_H
#define CAT_POINTERS_H

#include "cat_weakPtr.h"
#include "cat_owningPtr.h"
#include "cat_sharedPtr.h"

namespace cat {

#define ADD_PTR_TYPES(Cls)                             \
	using Cls##Ptr = cat::OwningPtr<Cls>;              \
	using Cls##CPtr = cat::OwningPtr<const Cls>;       \
	using Cls##WeakPtr = cat::WeakPtr<Cls>;            \
	using Cls##CWeakPtr = cat::WeakPtr<const Cls>;     \
	using Cls##SharedPtr = cat::SharedPtr<Cls>;        \
	using Cls##CSharedPtr = cat::SharedPtr<const Cls>;

#define PTRS_FOR_STRUCT(Cls) \
	struct Cls;              \
	ADD_PTR_TYPES(Cls)

#define PTRS_FOR_CLASS(Cls) \
	class Cls;              \
	ADD_PTR_TYPES(Cls)


#define ADD_PTR_TYPES_TEMPLATE(Cls, tmplParams, tmplArgs)                         \
	template<tmplParams> using Cls##Ptr = cat::OwningPtr<Cls<tmplArgs>>;          \
	template<tmplParams> using Cls##CPtr = cat::OwningPtr<const Cls<tmplArgs>>;   \
	template<tmplParams> using Cls##WeakPtr = cat::WeakPtr<Cls<tmplArgs>>;        \
	template<tmplParams> using Cls##CWeakPtr = cat::WeakPtr<const Cls<tmplArgs>>;

#define PTRS_FOR_TEMPLATE_STRUCT(Cls, tmplParams, tmplArgs) \
	template<tmplParams> struct Cls;                        \
	ADD_PTR_TYPES_TEMPLATE(Cls, tmplParams, tmplArgs)

#define PTRS_FOR_TEMPLATE_CLASS(Cls, tmplParams, tmplArgs)  \
	template<tmplParams> class Cls;                         \
	ADD_PTR_TYPES_TEMPLATE(Cls, tmplParams, tmplArgs)

}


#endif // CAT_POINTERS_H
