// YSLib::Core::YFunc by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-14 18:48:44 + 08:00;
// UTime = 2010-10-16 14:00 + 08:00;
// Version = 0.1640;


#ifndef INCLUDED_YFUNC_HPP_
#define INCLUDED_YFUNC_HPP_

// YFunc ：函数对象封装模块。

#include "ysdef.h"

YSL_BEGIN

//函数对象基类。
template<typename _tPointer>
class GHBase
{
private:
	mutable _tPointer _ptr; //指针。

protected:
	inline explicit
	GHBase(_tPointer pf = NULL) : _ptr(pf)
	{}
	template<typename P>
	inline explicit
	GHBase(GHBase<P> pf) : _ptr(reinterpret_cast<_tPointer>(P(pf)))
	{}
	inline
	~GHBase()
	{}

public:
	inline
	operator _tPointer() const
	{
		return _ptr;
	}

	inline bool
	empty() const
	{
		return !_ptr;
	}
	inline _tPointer
	GetPtr() const
	{
		return _ptr;
	}
};

YSL_END

#endif

