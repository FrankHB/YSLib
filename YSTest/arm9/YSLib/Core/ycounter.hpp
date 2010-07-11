// YSLib::Core::YCounter by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-2-9 20:43:52;
// UTime = 2010-6-15 20:44;
// Version = 0.1616;


#ifndef INCLUDED_YCOUNTER_H_
#define INCLUDED_YCOUNTER_H_

// YCounter ：对象计数器。

#include "ysdef.h"

YSL_BEGIN

//模板类实例计数器。
template<class T, typename count_t = u32>
class GMCounter
{
private:
	static count_t nCount, nTotal; //实例生成计数和实例计数。
	count_t nID; //实例标识序列号。

public:
	GMCounter()
	: nID(nCount++)
	{
		++nTotal;
	}
	GMCounter(const GMCounter&)
	: nID(nCount++)
	{
		++nTotal;
	}

protected:
	~GMCounter()
	{
		--nTotal;
	}

public:
	static count_t
	GetCount()
	{
		return nCount;
	}
	static count_t
	GetTotal()
	{
		return nTotal;
	}
	count_t
	GetID() const
	{
		return nID;
	}

protected:
	static void
	ResetCount()
	{
		nCount = 0;
	}
	static void
	ResetTotal()
	{
		nTotal = 0;
	}
	void ResetID()
	{
		nID = 0;
	}
};

template<class T, typename count_t>
count_t GMCounter<T, count_t>::nCount(0);

template<class T, typename count_t>
count_t GMCounter<T, count_t>::nTotal(0);

YSL_END

#endif

