// YSLib::Shell::YDesktop by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-05-02 12:00:08 + 08:00;
// UTime = 2010-11-03 19:59 + 08:00;
// Version = 0.2028;


#include "ydesktop.h"

YSL_BEGIN

using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

using namespace Controls;

YDesktop::YDesktop(YScreen& s, Color c, GHResource<YImage> i)
	: YFrameWindow(Rect::FullScreen, i, NULL, hShellMain),
	Screen(s), sDOs()
{
	BackColor = c;
}

void
YDesktop::operator+=(IVisualControl& w)
{
	if(std::find(sDOs.begin(), sDOs.end(), &w) == sDOs.end())
	{
		sDOs.push_back(&w);
		GMFocusResponser<IVisualControl>::operator+=(w);
		bRefresh = true;
	}
}
bool
YDesktop::operator-=(IVisualControl& w)
{
	DOs::iterator i(std::find(sDOs.begin(), sDOs.end(), &w));

	if(i == sDOs.end())
		return false;
	w.CheckRemoval(*this);
	sDOs.erase(i);
	bRefresh = true;
	return true;
}
YDesktop::DOs::size_type
YDesktop::RemoveAll(IVisualControl& w)
{
	DOs::size_type n(0);
	DOs::iterator i;

	while((i = std::find(sDOs.begin(), sDOs.end(), &w)) != sDOs.end())
	{
		w.CheckRemoval(*this);
		sDOs.erase(i);
		++n;
	}
	bRefresh = true;
	return n;
}

bool
YDesktop::MoveToTop(IVisualControl& w)
{
	DOs::iterator i(std::find(sDOs.begin(), sDOs.end(), &w));

	if(i != sDOs.end())
	{
		std::swap(*i, sDOs.back());
		bRefresh = true;
		return true;
	}
	return false;
}

void
YDesktop::RemoveTopDesktopObject()
{
	if(!sDOs.empty())
	{
		sDOs.back()->CheckRemoval(*this);
		sDOs.pop_back();
		bRefresh = true;
	}
}

void
YDesktop::ClearDesktopObjects()
{
	ClearFocusingPtr();
	sDOs.clear();
	bRefresh = true;
}

IVisualControl*
YDesktop::GetFirstDesktopObjectPtr() const
{
	return sDOs.empty() ? NULL : sDOs.front();
}
IVisualControl*
YDesktop::GetTopDesktopObjectPtr() const
{
	return sDOs.empty() ? NULL : sDOs.back();
}
IVisualControl*
YDesktop::GetTopDesktopObjectPtr(const Point& pt) const
{
	for(DOs::const_reverse_iterator i(sDOs.rbegin()); i != sDOs.rend(); ++i)
	{
		try
		{
			if(Contains(**i, pt))
				return *i;
		}
		catch(std::bad_cast&)
		{}
	}
	return NULL;
}

void
YDesktop::DrawBackground()
{
	YWindowAssert(this, YDesktop, DrawBackground);

	if(prBackImage && GetBufferPtr())
	{
		const PixelType* imgBg(prBackImage->GetImagePtr());

		if(imgBg)
		{
			std::memcpy(GetBufferPtr(), imgBg, Buffer.GetSizeOfBuffer());
			return;
		}
	}
	Buffer.BeFilledWith(BackColor);
}

void
YDesktop::DrawDesktopObjects()
{
	YWindowAssert(this, YDesktop, DrawDesktopObjects);

	for(DOs::iterator i(sDOs.begin()); i != sDOs.end(); ++i)
	{
		try
		{
			IWindow& w(dynamic_cast<IWindow&>(**i));
			w.Refresh();
			w.SetUpdate(true);
			w.Update();
		}
		catch(std::bad_cast&)
		{}
	}
}

void
YDesktop::Draw()
{
	DrawBackground();
	DrawDesktopObjects();
	bUpdate = true;
}

void
YDesktop::Refresh()
{
	if(bRefresh)
	{
		bRefresh = false;
		Draw();
	}
}

void
YDesktop::Update()
{
	if(bUpdate)
	{
		bUpdate = false;
		Screen.Update(GetBufferPtr());
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

