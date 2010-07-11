// YReader -> DSReader by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 14:03:47;
// UTime = 2010-7-9 9:55;
// Version = 0.1965;


#ifndef _DSREADER_H_
#define _DSREADER_H_

// DSReader ：适用于 NDS 的双屏阅读器实现。

#include "../YSLib/Helper/shlds.h"

#include "DSRMsg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

class MDualScreenReader : public Text::MTextBuffer
{
private:
	YTextFile& tf; //文本文件对象。
	YFontCache* pfc; //字体缓存。
	u16 left, top_up, top_down; // left ：上下字符区域距离屏幕左边距离； top_up ：上字符区域距离上屏顶端距离； top_down ：下字符区域距离下屏顶端距离。
	PixelType *pBgUp, *pBgDn; //上下屏幕背景层显存地址。
	YTextRegion &trUp, &trDn; //上下屏幕对应字符区域。
	ROT rot; //屏幕指向。
	u32 nLoad; //从文本文件读取的字符数。
	s32 offUp, offDn; //字符区域读取文本文件的输出位置（偏移字符量）。
	u8 lnHeight; //行高。

	DefGetter(u16, ColorUp, trUp.GetColor()) //取上字符区域的字体颜色。
	DefGetter(u16, ColorDn, trDn.GetColor()) //取下字符区域的字体颜色。
	DefGetter(u8, LnGapUp, trUp.GetLineGap()) //取上字符区域的行距。
	DefGetter(u8, LnGapDn, trDn.GetLineGap()) //取下字符区域的行距。

	PDefHead(void, SetColorUp, u16 c = ARGB16(1, 0, 0, 0)) //设置上字符区域的字体颜色。
		ImplBodyMemberVoid(trUp, SetColor, c)
	PDefHead(void, SetColorDn, u16 c = ARGB16(1, 0, 0, 0)) //设置下字符区域的字体颜色。
		ImplBodyMemberVoid(trDn, SetColor, c)
	PDefHead(void, SetLnGapUp, u16 g = 0) //设置上字符区域的行距。
		ImplBodyMemberVoid(trUp, SetLineGap, g)
	PDefHead(void, SetLnGapDn, u16 g = 0) //设置下字符区域的行距。
		ImplBodyMemberVoid(trDn, SetLineGap, g)

	void Clear()
	//清除字符区域缓冲区。
	{
		trUp.ClearImage();
		trDn.ClearImage();
	}
	void ResetPen()
	//复位缓存区域写入位置。
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
	u32 TextFill(u32);
	//文本填充：输出文本缓冲区指定偏移量的字符串，并返回填充字符数。

public:
	/*
	//构造函数。
	l ：距离屏幕左边距离； w ：字符区域宽。
	t_up ：上字符区域距离上屏顶端距离； h_up：上字符区域高。
	t_down ：下字符区域距离下屏顶端距离； h_down：下字符区域高。
	fc ：字体缓存对象指针。
	*/
	MDualScreenReader(YTextFile& srcf, u16 l = 0, u16 w = SCRW,
		u16 t_up = 0, u16 h_up = SCRH, u16 t_down = 0, u16 h_down = SCRH, YFontCache* fc = pDefaultFontCache);
	~MDualScreenReader(); //析构函数。

	bool IsTextTop(); //判断输出位置是否到文本顶端。	
	bool IsTextBottom(); //判断输出位置是否到文本底端。

	DefGetter(u8, FontSize, pfc->GetFontSize()) //取字符区域的字体大小。
	DefGetter(YTextRegion&, Up, trUp) //取上字符区域的引用。
	DefGetter(YTextRegion&, Dn, trDn) //取下字符区域的引用。
	DefGetter(u16, Color, GetColorUp()) //取字符区域的字体颜色。
	DefGetter(u8, LineGap, GetLnGapUp()) //取字符区域的行距。

	DefSetterDef(u16, Left, left, 0) //设置字符区域距离屏幕左边距离。
	//设置字符区域字体大小。
	void
	SetFontSize(u16 = 16);

	//设置字符颜色。
	void
	SetColor(u16 = ARGB16(1, 0, 0, 0));
	//设置行距。
	void
	SetLineGap(u8 = 0);

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
	TextInit();
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

