// YSLib::Core::YCounter by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-09 20:43:52 + 08:00;
// UTime = 2010-10-24 13:37 + 08:00;
// Version = 0.1636;


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
	//********************************
	//名称:		GMCounter
	//全名:		YSLib::GMCounter::GMCounter
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	构造。
	//备注:		实例生成计数和实例计数自增。
	//********************************
	GMCounter()
	: nID(nCount++)
	{
		++nTotal;
	}
	//********************************
	//名称:		GMCounter
	//全名:		YSLib::GMCounter::GMCounter
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const GMCounter &
	//功能概要:	复制构造。
	//备注:		实例生成计数和实例计数自增。
	//********************************
	GMCounter(const GMCounter&)
	: nID(nCount++)
	{
		++nTotal;
	}

protected:
	//********************************
	//名称:		~GMCounter
	//全名:		YSLib::GMCounter::~GMCounter
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		实例计数自减。
	//********************************
	~GMCounter()
	{
		--nTotal;
	}

public:
	DefStaticGetter(count_t, Count, nCount)
	DefStaticGetter(count_t, Total, nTotal)
	DefGetter(count_t, ID, nID)

protected:
	//********************************
	//名称:		ResetCount
	//全名:		YSLib::GMCounter::ResetCount
	//可访问性:	protected static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位实例生成计数。
	//备注:		
	//********************************
	static void
	ResetCount()
	{
		nCount = 0;
	}

	//********************************
	//名称:		ResetTotal
	//全名:		YSLib::GMCounter::ResetTotal
	//可访问性:	protected static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位实例计数。
	//备注:		
	//********************************
	static void
	ResetTotal()
	{
		nTotal = 0;
	}

	//********************************
	//名称:		ResetID
	//全名:		YSLib::GMCounter::ResetID
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位实例标识序列号。
	//备注:		
	//********************************
	void
	ResetID()
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

