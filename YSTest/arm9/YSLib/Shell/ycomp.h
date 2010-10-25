// YSLib::Shell::YComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-19 20:05:08 + 08:00;
// UTime = 2010-10-24 19:57 + 08:00;
// Version = 0.2766;


#ifndef INCLUDED_YCOMPONENT_H_
#define INCLUDED_YCOMPONENT_H_

// YComponent ：平台无关的图形用户界面组件实现。

#include "../Core/yapp.h"
#include "../Core/yevt.hpp"
#include "../Core/yevtarg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//基本组件接口。
DeclInterface(IComponent)
EndDecl


//基本组件。
class YComponent : public GMCounter<YComponent>, public YCountableObject,
	implements IComponent
{
public:
	typedef YCountableObject ParentType;
};


//通用对象组类模板。
template<class _type, class _tContainer = set<_type*> >
class GMContainer : public _tContainer,
	implements GIContainer<_type>
{
public:
	typedef _tContainer ContainerType; //对象组类型。

	virtual DefEmptyDtor(GMContainer)

	//********************************
	//名称:		GetContainer
	//全名:		YSLib::Components::GMContainer<_type, _tContainer>
	//				::GetContainer
	//可访问性:	public 
	//返回类型:	ContainerType&
	//修饰符:	
	//功能概要:	取容器引用。
	//备注:		
	//********************************
	ContainerType&
	GetContainer()
	{
		return *this;
	}
	inline DefGetter(const ContainerType&, Container, *this)

	//********************************
	//名称:		operator+=
	//全名:		YSLib::Components::GMContainer<_type, _tContainer>::operator+=
	//可访问性:	ImplI(GIContainer<_type>) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_type & w
	//功能概要:	向对象组添加对象。
	//备注:		
	//********************************
	ImplI(GIContainer<_type>) void
	operator+=(_type& w) 
	{
		insert(&w);
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Components::GMContainer<_type, _tContainer>::operator-=
	//可访问性:	ImplI(GIContainer<_type>) public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	_type & w
	//功能概要:	从对象组移除对象。
	//备注:		
	//********************************
	ImplI(GIContainer<_type>) bool
	operator-=(_type& w)
	{
		return erase(&w);
	}
};


class AFocusRequester;


//焦点响应器模板。
template<class _type = AFocusRequester>
class GMFocusResponser// : implements GIContainer<_type>
{
	friend class AFocusRequester;

protected:
	_type* pFocusing; //焦点对象指针。
	Components::GMContainer<_type> sFOs; //焦点对象组。

	typedef typename Components::GMContainer<_type>::ContainerType FOs; \
		//焦点对象组类型。

	//********************************
	//名称:		GMFocusResponser
	//全名:		YSLib::Components::GMFocusResponser<_type>::GMFocusResponser
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		
	//********************************
	inline
	GMFocusResponser()
	: pFocusing(NULL), sFOs()
	{}

public:
	//判断指定指针是否和焦点对象指针相等。
	inline PDefH(bool, IsFocusing, _type* p) const
		ImplRet(pFocusing == p)

	//取焦点对象指针。
	inline DefGetter(_type*, FocusingPtr, pFocusing)
	//取焦点对象组（只读）。
	inline DefGetter(const FOs&, FocusingSet, sFOs)

protected:
	//********************************
	//名称:		SetFocusingPtr
	//全名:		YSLib::Components::GMFocusResponser<_type>::SetFocusingPtr
	//可访问性:	protected 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	_type * p
	//功能概要:	设置焦点对象指针。
	//备注:		
	//********************************
	bool
	SetFocusingPtr(_type* p)
	{
		if(p && sFOs.find(p) == sFOs.end())
			return false;
		if(pFocusing != p)
		{
			if(pFocusing && pFocusing->IsFocused())
				pFocusing->ReleaseFocus(GetZeroElement<MEventArgs>());
			pFocusing = p;
		}
		return pFocusing;
	}

	//向焦点对象组添加焦点对象。
	inline PDefHOperator(void, +=, _type& c)
		ImplExpr(sFOs += c)
	//从焦点对象组移除焦点对象。
	inline PDefHOperator(bool, -=, _type& c)
		ImplRet(sFOs -= c)

public:
	//清空焦点指针。
	inline PDefH(bool, ClearFocusingPtr)
		ImplRet(SetFocusingPtr(NULL))
};


//焦点申请器接口模板。
template<class _type = AFocusRequester>
DeclInterface(GIFocusRequester)
	DeclIEntry(bool IsFocused() const)
	DeclIEntry(bool IsFocusOfContainer(GMFocusResponser<_type>&) const)

	DeclIEntry(bool CheckRemoval(GMFocusResponser<_type>&) const)

	DeclIEntry(void ReleaseFocus(const MEventArgs&))
EndDecl


//焦点申请器。
class AFocusRequester : implements GIFocusRequester<AFocusRequester>
{
protected:
	bool bFocused; //是否为所在容器的焦点。

public:
	//********************************
	//名称:		AFocusRequester
	//全名:		YSLib::Components::AFocusRequester::AFocusRequester
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		
	//********************************
	AFocusRequester();
	virtual DefEmptyDtor(AFocusRequester)

	//********************************
	//名称:		IsFocused
	//全名:		YSLib::Components::AFocusRequester::IsFocused
	//可访问性:	ImplI(GIFocusRequester<AFocusRequester>) public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否为获得焦点状态。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<AFocusRequester>) bool
	IsFocused() const;
	//********************************
	//名称:		IsFocusOfContainer
	//全名:		YSLib::Components::AFocusRequester::IsFocusOfContainer
	//可访问性:	ImplI(GIFocusRequester<AFocusRequester>) public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	判断是否已在指定响应器中获得焦点。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<AFocusRequester>) bool
	IsFocusOfContainer(GMFocusResponser<AFocusRequester>&) const;
	//********************************
	//名称:		IsFocusOfContainer
	//全名:		YSLib::Components::AFocusRequester::IsFocusOfContainer<_type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	GMFocusResponser<_type> &
	//功能概要:	判断是否已在指定响应器中获得焦点。
	//备注:		
	//********************************
	template<class _type>
	bool
	IsFocusOfContainer(GMFocusResponser<_type>&) const;

	//********************************
	//名称:		CheckRemoval
	//全名:		YSLib::Components::AFocusRequester::CheckRemoval
	//可访问性:	ImplI(GIFocusRequester<AFocusRequester>) public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	判断是否已在指定响应器中获得焦点，若是则释放焦点。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<AFocusRequester>) bool
	CheckRemoval(GMFocusResponser<AFocusRequester>&) const;
	//********************************
	//名称:		CheckRemoval
	//全名:		YSLib::Components::AFocusRequester::CheckRemoval<_type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	GMFocusResponser<_type> &
	//功能概要:	判断是否已在指定响应器中获得焦点，若是则释放焦点。
	//备注:		
	//********************************
	template<class _type>
	bool
	CheckRemoval(GMFocusResponser<_type>&) const;

	//********************************
	//名称:		RequestFocus
	//全名:		YSLib::Components::AFocusRequester::RequestFocus
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	向指定响应器对应的容器申请获得焦点。
	//备注:		
	//********************************
	bool
	RequestFocus(GMFocusResponser<AFocusRequester>&);
	//********************************
	//名称:		RequestFocus
	//全名:		YSLib::Components::AFocusRequester::RequestFocus<_type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	GMFocusResponser<_type> &
	//功能概要:	向指定响应器对应的容器申请获得焦点。
	//备注:		
	//********************************
	template<class _type>
	bool
	RequestFocus(GMFocusResponser<_type>&);

	//********************************
	//名称:		ReleaseFocus
	//全名:		YSLib::Components::AFocusRequester::ReleaseFocus
	//可访问性:	ImplI(GIFocusRequester<AFocusRequester>) public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	释放焦点。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<AFocusRequester>) bool
	ReleaseFocus(GMFocusResponser<AFocusRequester>&);
	//********************************
	//名称:		ReleaseFocus
	//全名:		YSLib::Components::AFocusRequester::ReleaseFocus<_type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	GMFocusResponser<_type> &
	//功能概要:	释放焦点。
	//备注:		
	//********************************
	template<class _type>
	bool
	ReleaseFocus(GMFocusResponser<_type>&);

	ImplA(GIFocusRequester<AFocusRequester>)
	DeclIEntry(void ReleaseFocus(const MEventArgs&))
};

inline
AFocusRequester::AFocusRequester()
	: bFocused(false)
{}

inline bool
AFocusRequester::IsFocused() const
{
	return bFocused;
}

inline bool
AFocusRequester::IsFocusOfContainer(GMFocusResponser<AFocusRequester>& c) const
{
	return c.GetFocusingPtr() == this;
}
template<class _type>
inline bool
AFocusRequester::IsFocusOfContainer(GMFocusResponser<_type>& c) const
{
	return c.GetFocusingPtr() == dynamic_cast<const _type*>(this);
}

template<class _type>
bool
AFocusRequester::CheckRemoval(GMFocusResponser<_type>& c) const
{
	if(IsFocusOfContainer(c))
	{
		c.ClearFocusingPtr();
		return true;
	}
	return false;
}

template<class _type>
bool
AFocusRequester::RequestFocus(GMFocusResponser<_type>& c)
{
	return !(bFocused && IsFocusOfContainer(c)) && (bFocused = c.SetFocusingPtr(dynamic_cast<_type*>(this)));
}

template<class _type>
bool
AFocusRequester::ReleaseFocus(GMFocusResponser<_type>& c)
{
	return bFocused && IsFocusOfContainer(c) && (bFocused = NULL, !(c.ClearFocusingPtr()));
}


//序列视图类模板。
template<class _tContainer>
class GSequenceViewer
{
public:
	typedef typename _tContainer::size_type SizeType; //项目下标类型。
	typedef std::ptrdiff_t IndexType; //项目索引类型。

private:
	_tContainer& c; //序列容器引用。
	IndexType nIndex, //项目索引：视图中首个项目下标，若不存在则为 -1 。
		nSelected; //选中项目下标，大于等于 GetTotal() 时无效。
	SizeType nLength; //视图长度。
	bool bSelected; //选中状态。

public:
	//********************************
	//名称:		GSequenceViewer
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>
	//				::GSequenceViewer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tContainer & c_
	//功能概要:	构造：使用指定容器。
	//备注:		
	//********************************
	explicit
	GSequenceViewer(_tContainer& c_)
	: c(c_), nIndex(0), nSelected(0), nLength(0), bSelected(false)
	{}

	inline PDefHOperator(GSequenceViewer&, ++) //选中项目下标自增。
		ImplRet(*this += 1)
	inline PDefHOperator(GSequenceViewer&, --) //选中项目下标自减。
		ImplRet(*this -= 1)
	inline PDefHOperator(GSequenceViewer&, ++, int) //视图中首个项目下标自增。
		ImplRet(*this >> 1)
	inline PDefHOperator(GSequenceViewer&, --, int) //视图中首个项目下标自减。
		ImplRet(*this >> -1)
	//********************************
	//名称:		operator>>
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::operator>>
	//可访问性:	public 
	//返回类型:	GSequenceViewer&
	//修饰符:	
	//形式参数:	IndexType d
	//功能概要:	视图中首个项目下标增加 d 。
	//备注:		
	//********************************
	inline GSequenceViewer&
	operator>>(IndexType d)
	{
		SetIndex(nIndex + d);
		return *this;
	}
	inline PDefHOperator(GSequenceViewer&, <<, IndexType d) //视图中首个项目下标减少 d 。
		ImplRet(*this >> -d)
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::operator+=
	//可访问性:	public 
	//返回类型:	GSequenceViewer&
	//修饰符:	
	//形式参数:	IndexType d
	//功能概要:	选中项目下标增加 d 。
	//备注:		
	//********************************
	inline GSequenceViewer&
	operator+=(IndexType d)
	{
		SetSelected(nSelected + d);
		return *this;
	}
	inline PDefHOperator(GSequenceViewer&, -=, IndexType d) //选中项目下标减少 d 。
		ImplRet(*this += -d)

	DefPredicate(Selected, bSelected) //判断是否为选中状态。

	DefGetter(SizeType, Total, c.size()) //取容器中项目个数。
	DefGetter(SizeType, Length, nLength)
	DefGetter(IndexType, Index, nIndex)
	DefGetter(IndexType, Selected, nSelected)
	DefGetter(SizeType, Valid, vmin(GetTotal() - GetIndex(), GetLength())) //取当前视图中有效项目个数。

	//********************************
	//名称:		SetIndex
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::SetIndex
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	IndexType t
	//功能概要:	设置项目索引。
	//备注:		
	//********************************
	bool
	SetIndex(IndexType t)
	{
		if(GetTotal() && isInIntervalRegular<IndexType>(t, GetTotal()) && t != nIndex)
		{
			if(!t)
				MoveViewerToBegin();
			else if(nLength + t > GetTotal())
				MoveViewerToEnd();
			else
				nIndex = t;
			RestrictSelected();
			return true;
		}
		return false;
	}
	//********************************
	//名称:		SetLength
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::SetLength
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	SizeType l
	//功能概要:	设置长度。
	//备注:		
	//********************************
	bool
	SetLength(SizeType l)
	{
		if(l != nLength)
		{
			if(l < 0)
			{
				nLength = 0;
				nSelected = 0;
			}
			else if(l < nLength && nSelected >= static_cast<IndexType>(l))
				nSelected = l - 1;
			else
				nLength = l;
			RestrictSelected();
			return true;
		}
		return false;
	}
	//********************************
	//名称:		SetSelected
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::SetSelected
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	IndexType t
	//功能概要:	设置选中项目下标。
	//备注:		
	//********************************
	bool
	SetSelected(IndexType t)
	{
		if(GetTotal() && isInIntervalRegular<IndexType>(t, GetTotal()) && !(t == nSelected && bSelected))
		{
			nSelected = t;
			RestrictViewer();
			bSelected = true;
			return true;
		}
		return false;
	}

	//********************************
	//名称:		ClearSelected
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::ClearSelected
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	取消选中状态。
	//备注:		
	//********************************
	bool
	ClearSelected()
	{
		if(IsSelected())
		{
			bSelected = false;
			return true;
		}
		return false;
	}

	//********************************
	//名称:		RestrictSelected
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::RestrictSelected
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	约束被选中的元素在视图内。
	//备注:		
	//********************************
	bool
	RestrictSelected()
	{
		if(nIndex < 0)
			return false;
		if(nSelected < nIndex)
			nSelected = nIndex;
		else if(nSelected >= nIndex + static_cast<IndexType>(nLength))
			nSelected = nIndex + nLength - 1;
		else
			return false;
		return true;
	}

	//********************************
	//名称:		RestrictViewer
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::RestrictViewer
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	约束视图包含被选中的元素。
	//备注:		
	//********************************
	bool
	RestrictViewer()
	{
		if(nIndex < 0)
			return false;
		if(nSelected < nIndex)
			nIndex = nSelected;
		else if(nSelected >= nIndex + static_cast<IndexType>(nLength))
			nIndex = nSelected + 1 - nLength;
		else
			return false;
		return true;
	}

	//********************************
	//名称:		MoveViewerToBegin
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::MoveViewerToBegin
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	移动视图至序列起始。
	//备注:		
	//********************************
	bool
	MoveViewerToBegin()
	{
		if(nIndex)
		{
			nIndex = 0;
			return true;
		}
		return false;
	}

	//********************************
	//名称:		MoveViewerToEnd
	//全名:		YSLib::Components::GSequenceViewer<_tContainer>::MoveViewerToEnd
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	移图至序列结动视尾。
	//备注:		
	//********************************
	bool
	MoveViewerToEnd()
	{
		if(GetTotal() >= nLength)
		{
			nIndex = GetTotal() - nLength;
			return true;
		}
		return false;
	}
};


//控制台。
class YConsole : public YComponent
{
public:
	YScreen& Screen;

	//********************************
	//名称:		YConsole
	//全名:		YSLib::Components::YConsole::YConsole
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YScreen &
	//形式参数:	bool
	//形式参数:	Drawing::Color
	//形式参数:	Drawing::Color
	//功能概要:	构造：使用指定屏幕、有效性、前景色和背景色。
	//备注:		
	//********************************
	explicit
	YConsole(YScreen& = *pDefaultScreen, bool = true, Drawing::Color = Drawing::ColorSpace::White, Drawing::Color = Drawing::ColorSpace::Black);
	//********************************
	//名称:		~YConsole
	//全名:		YSLib::Components::YConsole::~YConsole
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		自动停用。
	//********************************
	virtual
	~YConsole();

	//********************************
	//名称:		Activate
	//全名:		YSLib::Components::YConsole::Activate
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Drawing::Color
	//形式参数:	Drawing::Color
	//功能概要:	激活：使用指定屏幕、有效性、前景色和背景色。
	//备注:		
	//********************************
	void
	Activate(Drawing::Color = Drawing::ColorSpace::White, Drawing::Color = Drawing::ColorSpace::Black);

	//********************************
	//名称:		Deactivate
	//全名:		YSLib::Components::YConsole::Deactivate
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	停用。
	//备注:		
	//********************************
	void
	Deactivate();

	void
	Pause();
};

inline
YConsole::YConsole(YScreen& scr, bool a, Drawing::Color fc, Drawing::Color bc)
	: YComponent(),
	Screen(scr)
{
	if(a)
		Activate(fc, bc);
}
inline
YConsole::~YConsole()
{
	Deactivate();
}

inline void
YConsole::Activate(Drawing::Color fc, Drawing::Color bc)
{
	Def::InitConsole(Screen, fc, bc);
}

inline void
YConsole::Deactivate()
{
	Def::InitVideo();
}

inline void
YConsole::Pause()
{
	Def::WaitForInput();
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

