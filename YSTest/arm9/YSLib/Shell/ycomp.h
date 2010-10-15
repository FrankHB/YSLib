// YSLib::Shell::YComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-19 20:05:08 + 08:00;
// UTime = 2010-10-14 08:36 + 08:00;
// Version = 0.2549;


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

	GMContainer()
	: ContainerType()
	{}
	virtual DefEmptyDtor(GMContainer)

	ContainerType&
	GetContainer()
	{
		return *this;
	}
	const ContainerType&
	GetContainer() const
	{
		return *this;
	}

	void
	operator+=(_type& w) //向对象组添加对象。
	{
		insert(&w);
	}
	bool
	operator-=(_type& w) //从对象组移除对象。
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

	typedef typename Components::GMContainer<_type>::ContainerType FOs; //焦点对象组类型。

	inline
	GMFocusResponser()
	: pFocusing(NULL), sFOs()
	{}
	inline
	~GMFocusResponser()
	{}

public:
	//判断给定指针是否和焦点对象指针相等。
	inline bool
	IsFocusing(_type* p) const
	{
		return pFocusing == p;
	}

	//取焦点对象指针。
	inline _type*
	GetFocusingPtr() const
	{
		return pFocusing;
	}
	//取焦点对象组（只读）。
	inline const FOs&
	GetFocusingSet() const
	{
		return sFOs;
	}

protected:
	//设置焦点对象指针。
	bool
	SetFocusingPtr(_type* p)
	{
		if(p && sFOs.find(p) == sFOs.end())
			return false;
		if(pFocusing != p)
		{
			if(pFocusing && pFocusing->IsFocused())
				pFocusing->ReleaseFocus();
			pFocusing = p;
		}
		return pFocusing;
	}

	//向焦点对象组添加焦点对象。
	inline void
	operator+=(_type& c)
	{
		sFOs += c;
	}
	//从焦点对象组移除焦点对象。
	inline bool
	operator-=(_type& c)
	{
		return sFOs -= c;
	}

public:
	//清空焦点指针。
	inline bool
	ClearFocusingPtr()
	{
		return SetFocusingPtr(NULL);
	}
};


//焦点申请器接口模板。
template<class _type = AFocusRequester>
DeclInterface(GIFocusRequester)
	DeclIEntry(bool IsFocused() const)
	DeclIEntry(bool IsFocusOfContainer(GMFocusResponser<_type>&) const)

	DeclIEntry(bool CheckRemoval(GMFocusResponser<_type>&) const)

	DeclIEntry(void ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
EndDecl


//焦点申请器。
class AFocusRequester : implements GIFocusRequester<AFocusRequester>
{
protected:
	bool bFocused; //是否为所在容器的焦点。

public:
	AFocusRequester();
	virtual
	~AFocusRequester()
	{}

	//判断是否为获得焦点状态。
	bool
	IsFocused() const;
	//判断是否已在指定响应器中获得焦点。
	bool
	IsFocusOfContainer(GMFocusResponser<AFocusRequester>&) const;
	template<class _type>
	bool
	IsFocusOfContainer(GMFocusResponser<_type>&) const;

	//判断是否已在指定响应器中获得焦点，若是则释放焦点。
	bool
	CheckRemoval(GMFocusResponser<AFocusRequester>&) const;
	template<class _type>
	bool
	CheckRemoval(GMFocusResponser<_type>&) const;

	//向指定响应器对应的容器申请获得焦点。
	bool
	RequestFocus(GMFocusResponser<AFocusRequester>&);
	template<class _type>
	bool
	RequestFocus(GMFocusResponser<_type>&);
	//释放焦点。
	bool
	ReleaseFocus(GMFocusResponser<AFocusRequester>&);
	template<class _type>
	bool
	ReleaseFocus(GMFocusResponser<_type>&);

	DeclIEntry(void ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
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
	explicit
	GSequenceViewer(_tContainer& c_)
	: c(c_), nIndex(0), nSelected(0), nLength(0), bSelected(false)
	{}

	inline GSequenceViewer&
	operator++() //选中项目下标自增。
	{
		return *this += 1;
	}
	inline GSequenceViewer&
	operator--() //选中项目下标自减。
	{
		return *this += -1;
	}
	inline GSequenceViewer&
	operator++(int) //视图中首个项目下标自增。
	{
		return *this >> 1;
	}
	inline GSequenceViewer&
	operator--(int) //视图中首个项目下标自减。
	{
		return *this >> -1;
	}
	inline GSequenceViewer&
	operator>>(IndexType d) //视图中首个项目下标增加 d 。
	{
		SetIndex(nIndex + d);
		return *this;
	}
	inline GSequenceViewer&
	operator<<(IndexType d) //视图中首个项目下标减少 d 。
	{
		return *this >> -d;
	}
	inline GSequenceViewer&
	operator+=(IndexType d) //选中项目下标增加 d 。
	{
		SetSelected(nSelected + d);
		return *this;
	}
	inline GSequenceViewer&
	operator-=(IndexType d) //选中项目下标减少 d 。
	{
		return *this += -d;
	}

	DefPredicate(Selected, bSelected) //判断是否为选中状态。

	DefGetter(SizeType, Total, c.size()) //取容器中项目个数。
	DefGetter(SizeType, Length, nLength)
	DefGetter(IndexType, Index, nIndex)
	DefGetter(IndexType, Selected, nSelected)
	DefGetter(SizeType, Valid, vmin(GetTotal() - GetIndex(), GetLength())) //取当前视图中有效项目个数。

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

	bool
	ClearSelected() //取消选中状态。
	{
		if(IsSelected())
		{
			bSelected = false;
			return true;
		}
		return false;
	}

	bool
	RestrictSelected() //约束被选中的元素在视图内。
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
	bool
	RestrictViewer() //约束视图包含被选中的元素。
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

	bool
	MoveViewerToBegin() //移动视图至序列起始。
	{
		if(nIndex)
		{
			nIndex = 0;
			return true;
		}
		return false;
	}
	bool
	MoveViewerToEnd() //移动视图至序列结尾。
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

	explicit
	YConsole(YScreen& = *pDefaultScreen, bool = true, Drawing::Color = Drawing::Color::White, Drawing::Color = Drawing::Color::Black);
	virtual
	~YConsole();

	void
	Activate(Drawing::Color = Drawing::Color::White, Drawing::Color = Drawing::Color::Black);

	void
	Deactivate();
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

YSL_END_NAMESPACE(Components)

YSL_END

#endif

