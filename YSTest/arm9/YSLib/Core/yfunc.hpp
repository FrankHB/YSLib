// YSLib::Core::YFunc by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-2-14 18:48:44;
// UTime = 2010-6-21 22:04;
// Version = 0.1602;


#ifndef INCLUDED_YFUNC_HPP_
#define INCLUDED_YFUNC_HPP_

// YFunc ：函数对象封装模块。

#include "ysdef.h"

YSL_BEGIN

//函数对象基类。
template<typename _ptrType>
class GHBase
{
protected:
	mutable _ptrType _ptr; //对象指针。

	inline explicit
	GHBase(_ptrType pf = NULL) : _ptr(pf)
	{}
	template<typename P>
	inline explicit
	GHBase(GHBase<P> pf) : _ptr(reinterpret_cast<_ptrType>(P(pf)))
	{}
	inline
	~GHBase()
	{}

public:
	inline
	operator _ptrType() const
	{
		return _ptr;
	}

	inline bool
	empty() const
	{
		return !_ptr;
	}
	inline _ptrType
	GetPtr() const
	{
		return _ptr;
	}

//	bool contains(const Functor&) const;
//	const std::type_info& target_type() const;

};

/*
template<typename Functor>
class YFunc
{

};
*/

/*
//通用句柄。
struct HGerneral : protected GHBase<void*>
{
public:
	typedef void(YObject::*VoidMemType)(); //默认成员函数类型。

private:
	mutable VoidMemType _ptrMem; //成员函数指针。

public:
	inline explicit
	HGerneral(void* p = NULL, VoidMemType pm = NULL)
	: GHBase<void*>(p), _ptrMem(pm)
	{}
	inline explicit
	HGerneral(VoidMemType pm)
	: GHBase<void*>(), _ptrMem(pm)
	{}
	inline
	~HGerneral()
	{}

	DefGetter(void*, Pointer, _ptr)
	DefGetter(VoidMemType, Pointer, _ptrMem)

protected:
	DefSetter(void*, Pointer, _ptr)
	DefSetter(VoidMemType, Pointer, _ptrMem)
};
*/

YSL_END

#endif

