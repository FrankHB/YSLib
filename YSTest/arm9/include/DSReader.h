// YReader -> DSReader by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 14:03:47;
// UTime = 2010-8-13 19:29;
// Version = 0.2060;


#ifndef _DSREADER_H_
#define _DSREADER_H_

// DSReader ：适用于 NDS 的双屏阅读器实现。

#include "../YSLib/Helper/shlds.h"

#include "DSRMsg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

class MDualScreenReader
{
private:
	YTextFile& tf; //文本文件对象。
	Text::MTextFileBuffer Blocks; //文本缓存映射。
	YFontCache& fc; //字体缓存。
	u16 left, top_up, top_down; // left ：上下字符区域距离屏幕左边距离； top_up ：上字符区域距离上屏顶端距离； top_down ：下字符区域距离下屏顶端距离。
	PixelType *pBgUp, *pBgDn; //上下屏幕背景层显存地址。
	MTextRegion &trUp, &trDn; //上下屏幕对应字符区域。
	ROT rot; //屏幕指向。
	Text::MTextFileBuffer::TextIterator itUp, itDn; //字符区域读取文本缓存迭代器。
	u8 lnHeight; //行高。

	DefGetter(u16, ColorUp, trUp.Color) //取上字符区域的字体颜色。
	DefGetter(u16, ColorDn, trDn.Color) //取下字符区域的字体颜色。
	DefGetter(u8, LnGapUp, trUp.GetLineGap()) //取上字符区域的行距。
	DefGetter(u8, LnGapDn, trDn.GetLineGap()) //取下字符区域的行距。

	DefSetterDe(PixelType, ColorUp, trUp.Color, 0) //设置上字符区域的字体颜色。
	DefSetterDe(PixelType, ColorDn, trDn.Color, 0) //设置下字符区域的字体颜色。
	PDefHead(void, SetLnGapUp, u16 g = 0) //设置上字符区域的行距。
		ImplBodyMemberVoid(trUp, SetLineGap, g)
	PDefHead(void, SetLnGapDn, u16 g = 0) //设置下字符区域的行距。
		ImplBodyMemberVoid(trDn, SetLineGap, g)

	//清除字符区域缓冲区。
	void Clear()
	{
		trUp.ClearImage();
		trDn.ClearImage();
	}
	//复位缓存区域写入位置。
	void ResetPen()
	{
		trUp.SetPen();
		trDn.SetPen();
	}
/*
	void ReloadText(YTextFile& f)
	{
		YTextFile& f,
	}
*/
	void FillText(); //文本填充：输出文本缓冲区字符串，并返回填充字符数。

public:
	/*
	//构造函数。
	tf_ ：文本文件。
	l ：距离屏幕左边距离； w ：字符区域宽。
	t_up ：上字符区域距离上屏顶端距离； h_up：上字符区域高。
	t_down ：下字符区域距离下屏顶端距离； h_down：下字符区域高。
	fc_ ：字体缓存对象引用。
	*/
	MDualScreenReader(YTextFile& tf_, u16 l = 0, u16 w = SCRW,
		u16 t_up = 0, u16 h_up = SCRH, u16 t_down = 0, u16 h_down = SCRH, YFontCache& fc_ = *pDefaultFontCache);
	~MDualScreenReader(); //析构函数。

	bool IsTextTop(); //判断输出位置是否到文本顶端。	
	bool IsTextBottom(); //判断输出位置是否到文本底端。

	DefGetter(u8, FontSize, fc.GetFontSize()) //取字符区域的字体大小。
	DefGetter(MTextRegion&, Up, trUp) //取上字符区域的引用。
	DefGetter(MTextRegion&, Dn, trDn) //取下字符区域的引用。
	DefGetter(u16, Color, GetColorUp()) //取字符区域的字体颜色。
	DefGetter(u8, LineGap, GetLnGapUp()) //取字符区域的行距。

	DefSetterDe(SDST, Left, left, 0) //设置字符区域距离屏幕左边距离。
	void
	SetFontSize(MFont::SizeType = MFont::DefSize); //设置字符区域字体大小。
	void
	SetColor(PixelType = 0); //设置字符颜色。
	void
	SetLineGap(u8 = 0); //设置行距。

	//设置笔的行位置。
	//void
	//SetLnNNow(u8);

	//复位输出状态。
	void
	Reset();

	//绘制文本。
	void
	PrintText();

	//刷新到屏幕。
	void
	Refresh();

	//初始化载入文本。
	void
	InitText();

	//更新缓冲区文本。
	void
	Update();

	//上移一行。
	bool
	LineUp();

	//下移一行。
	bool
	LineDown();

	//上移一屏。
	bool
	ScreenUp();

	//下移一屏。
	bool
	ScreenDown();

	//自动滚屏。 pCheck ：输入检测函数指针。
	void
	Scroll(PFVOID pCheck);
};


class YWndDSReader : public YSLib::Components::YComponent, public YSLib::Components::Forms::AWindow
{
public:
	virtual void
	DrawForeground();
};

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

#endif

