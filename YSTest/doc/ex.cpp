
//	lblC.RequestFocus();
//	lblC.ReleaseFocus();

//	lblC.Text = L"测试= =";
//	HWND h(lblC.GetWindowHandle());
//	lblC.Text[0] ^= 1;
//	YString::iterator i(lblC.Text.begin());
//	++*i;
//	tf.SetPos(0);
//	tf.Seek(-128, SEEK_END);
//	sprintf(strtf, "%d,%d,%d,%d,%d", tf.IsValid(), tf.GetBOMLen(), tf.GetFileLen(), tf.GetCP(), tf.GetPos());

/*
struct non_copyable non_inheritable
{
protected:
	non_copyable()
	{}

private:
	non_copyable(const non_copyable&);
};
static void
_testStr(const YString& s)
{
	char* str = sdup(str.c_str(), '?');
	yprintf("%s\n",x,str);
	free(str);
}
*/
/*
static void
RectDarken(BitmapPtr src, const SPoint& l, const SSize& s, HWND hWnd)
{
	YAssert(hWnd, "err: hWnd cannot be a null pointer.");

	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), SRect(l, s), s, transPixelEx, transSeq());
//	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), SRect(l, s), transPixelShiftRight(1), transSeq());


	IWidget* pWgt(dynamic_cast<IWidget*>(hWnd->GetFocusingPtr()));

	if(pWgt)
		DrawWidgetBounds(*pWgt, ARGB16(1, 31, 1, 31));

	YGIC g(hWnd->GetBufferPtr(), hWnd->GetBounds());
//	DrawLineSeg(g, 2, 4, hWnd->GetWidth(), hWnd->GetHeight(), PixelType(ARGB16(1, 9, 4, 31)));
//	DrawLineSeg(g, 2, 10, 12, 65, PixelType(ARGB16(1, 1, 31, 1)));

	DrawWindowBounds(hWnd, ARGB16(1, 1, 1, 31));
//	DrawHLineSeg(g, 1, 10, 65, PixelType(ARGB16(1, 1, 1, 31)));
//	DrawVLineSeg(g, 1, 10, 65, PixelType(ARGB16(1, 1, 1, 31)));
//	DrawPoint(g, 3, 3, PixelType(ARGB16(1, 1, 1, 31)));
//	DrawRect(g, 11, 12, 19, 24, PixelType(ARGB16(1, 4, 31, 31)));
//	DrawWndBounds(hWnd, ARGB16(1, 4, 31, 9));
	DrawLineSeg(g, 2, 3, 22, 9, PixelType(ARGB16(1, 31, 0, 0)));
	DrawLineSeg(g, 22, 3, 42, 28, PixelType(ARGB16(1, 31, 15, 0)));
	DrawLineSeg(g, 52, 29, 62, 3, PixelType(ARGB16(1, 31, 31, 0)));
	DrawLineSeg(g, 72, 11, 82, 3, PixelType(ARGB16(1, 0, 31, 0)));
	DrawLineSeg(g, 102, 5, 82, 13, PixelType(ARGB16(1, 0, 0, 31)));
	DrawLineSeg(g, 122, 3, 102, 35, PixelType(ARGB16(1, 11, 0, 31)));
	DrawLineSeg(g, 122, 9, 142, 1, PixelType(ARGB16(1, 31, 0, 31)));
	DrawLineSeg(g, 132, 35, 152, 3, PixelType(ARGB16(1, 31, 21, 31)));
}
*/
/*
sprintf(strtf, "B%p,%p,%p,%p,%p\n", !!pDefaultFontCache->GetTypefacePtr("FZYaoTi", "Regular"),
!!pDefaultFontCache->GetTypefacePtr("FZYaoTi", "Bold"),
!!pDefaultFontCache->GetTypefacePtr("Microsoft YaHei", "Regular"),
!!pDefaultFontCache->GetTypefacePtr("Microsoft YaHei", "Bold"),
!!pDefaultFontCache->GetTypefacePtr("Microsoft YaHei", "Italic")
);

sprintf(strtf, "A%p,%p,%p\n", pDefaultFontCache->GetFontFamilyPtr("FZYaoTi"),
pDefaultFontCache->GetFontFamilyPtr("YouYuan"),
pDefaultFontCache->GetFontFamilyPtr("Microsoft YaHei")
);
*/
/*
#include "../YSLib/Core/ystring.h"

class YPath
{
public:
	typedef YString PathType;
	typedef PathType& RefType;
	typedef const PathType& CRefType;

protected:
	PathType Directory;
	PathType File;

public:
	YPath(CRefType);

	CRefType GetDirectory() const;
	CRefType GetFile() const;

	void SetPath(CRefType);
};

inline YPath::YPath(YPath::CRefType path)
{
	SetPath(path);
}

inline YPath::CRefType YPath::GetDirectory() const
{
	return Directory;
}
inline YPath::CRefType YPath::GetFile() const
{
	return File;
}

inline void YPath::SetPath(YPath::CRefType path)
{
	SplitPath(path, Directory, File);
}


YClass YFileItem
{
private:
	DIR_ITER* dir;
	stat st;
	PATHSTR fileName;

public:
	~YFileItem();

	bool IsDirectory() const;
	bool IsValid() const;

	void Open(CPATH path);
	void Reset();
	void Close();
	int ToNext();
};

inline YFileItem::~YFileItem()
{
	Close();
}

inline bool YFileItem::IsDirectory() const
{
	return st.st_mode & S_IFDIR;
}
inline bool YFileItem::IsValid() const
{
	return dir != NULL;
}

void YFileItem::Open(CPATH path)
{
	dir = diropen(path);
}
void YFileItem::Reset()
{
	dirreset(dir);
}
void YFileItem::Close()
{
	if(dir)7
		dirclose(dir);
}
int YFileItem::ToNext()
{
	return dirnext(dir, fileName, &st);
}*/


/*
class YListControl : public YObject
{
protected:
	YPath alloc;

public:
	YListControl();
};

YFileListControl(CPATH path)
{
}

*/

//unverified:

/*


class YLabeledWidget : public GMCounter<LabeledWidget>, public YWidget
{
protected:
	YLabel& label;
	YWidget& widget;

public:
	template<class _CharT>
		YLabeledWidget(HWND hWnd, const _CharT*, YWidget*, const YFont::SizeType = YFont::DefSize);
	virtual ~LabeledWidget();

	virtual void SetBounds(s16 x, s16 y, u16 w, u16 h);
	virtual void SetBounds(SRect r);

};
void YLabeledWidget::SetBounds(SRect& r)
{
	YWidget::SetBounds(r);

	label.SetBounds(r.x, r.y, r.w >> 1, r.h);
	widget.SetBounds(r.x + (r.w>>1), r.y, r.w - (r.w >> 1), r.h);
}
*/

/*
//	(prTextRegion->*SelectRenderer(Transparent))
class YCommandButton : public GMCounter<YCommandButton>, public YWidget //命令按钮。
{
public:
	typedef YWidget ParentType;
protected:
	YLabel* label;
	YImage* image;

public:
	YString& Caption;
	u8& FontSize;

	YCommandButton();
};


*/
	/*
	//设置主屏显示模式为帧缓冲，副屏不使用
	videoSetMode(MODE_FB0);
	videoSetModeSub(0);
	vramSetBankA(VRAM_A_LCD);
	for(u16 y = 60; y < SCRH - 60; ++y)
		for(u16 x = 60; x < SCRW - 60; ++x)
			VRAM_A[y * SCRW + x] = RGB15((x+5)%31,y%31,(x&y)&31);
	PixelType color = RGB15(31,0,0)|BITALPHA;
	*/

	//对象测试段。
	//YImage* pyi = new YImage;
	//sprintf(strtbuf + strlen(strtbuf), "a.addr>%p,pyi>size%d,addr:0x%x;\n",&theApp,&a,sizeof(*pyi),pyi);

	/*

	*/
	/*
	shltxt.PrintString("CaoNiMaYahoo爷们万碎！！！wwwwwwwww");
	uchar_t ustr[256];
	wchar_t wstr[512];
	u32 ucl = MBCSToUTF16LE(ustr, str, GBK);
	w32 wcl = MBCSToUCS(wstr, str, GBK);
	shltxt.PutString(ustr);
	shltxt.PutString(wstr);
	shltxt.PutString(L"Innovation in China.!?<%^&*>");
	*/
	//shltxt.CopyToBuffer(pDesktopUp->GetPtr());
	//shltxt.BlitToBuffer(pDesktopUp->GetPtr());
//	for(int i = 0;i < 128; ++i)
	/*
	sprintf(strttxt + strlen(strttxt), "%x,", shltxt.GetBufferPtr()[i]);
	MDualScreenReader& dsr = *pdsr;
	YTextRegion& shltxt = dsr.GetUp();
	sprintf(strtbuf + strlen(strtbuf), "buffer addr : 0x%x", (int)shltxt.GetBufferPtr());
	sprintf(strtmargin, "%d, %d, %d, %d;", shltxt.Margin.GetLeft(), shltxt.Margin.GetRight(), shltxt.Margin.GetTop(), shltxt.Margin.GetBottom());
	sprintf(strtf,"xxx:%u,%u;%u,%u\n",shltxt.Margin.GetBottom(),shltxt.ResizeMargin(),shltxt.GetPenX(),shltxt.GetPenY());
	sprintf(strtpen, "penX = %d, penY = %d;", shltxt.GetPenX(), shltxt.GetPenY());
	*/

//AGG Drawing Test

/*
static u8* frameBuffer = new u8[sizeof(SCRBUF)];
static agg::rendering_buffer& grbuf = *new agg::rendering_buffer(frameBuffer, SCRW, SCRH, SCRW * sizeof(PixelType));

typedef agg::renderer_base<agg::pixfmt_argb16> rend_type;
static rend_type rend;//(grbuf);
static agg::rasterizer_outline<rend_type> rast(rend);*/

////

/*
static void
ad_rect(const SRect& r, const agg::rgba8* c)
{
	rast.move_to(r.x, r.y);
	rast.line_to(r.x + r.w, r.y);
	rast.line_to(r.x + r.w, r.y + r.h);
	rast.line_to(r.x, r.y + r.h);
	rast.line_to(r.x, r.y);
	if(c)
		rast.render(rend, *c);
}
typedef std::vector<SPoint> poly; //多边形。
static void
ad_v(const poly& p, const agg::rgba8* c)
{
	poly::const_iterator i = p.begin();

	rast.move_to(i->x, i->y);
	while(++i != p.end())
		rast.line_to(i->x, i->y);
	if(c)
		rast.render(rend, *c);
}
*/
/*
static void on_draw()
{
	agg::rendering_buffer &rbuf = grbuf;
	agg::pixfmt_rgb555 pixf(rbuf);

	typedef agg::renderer_base<agg::pixfmt_rgb555> renderer_base_type;
	renderer_base_type renb(pixf);

	typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
	renderer_scanline_type rensl(renb);

	// Vertex Source
	agg::ellipse ell(100,100,50,50); //圆心在中间
	// Coordinate conversion pipeline

	agg::trans_affine mtx;
	mtx.scale(0.5,1); // x轴缩小到原来的一半
	mtx.rotate(agg::deg2rad(30)); // 旋转30度
	mtx.translate(100,100); // 平移100,100
	typedef agg::conv_transform<agg::ellipse> ell_ct_type;
	ell_ct_type ctell(ell, mtx); // 矩阵变换
	typedef agg::conv_contour<ell_ct_type> ell_cc_type;
	ell_cc_type ccell(ctell); // 轮廓变换

	typedef agg::conv_contour<agg::ellipse> ell_cc_type;
	ell_cc_type ccell(ell);
	typedef agg::conv_stroke<ell_cc_type> ell_cc_cs_type;
	ell_cc_cs_type csccell(ccell); // 转换成多义线

	agg::rasterizer_scanline_aa<> ras;
	agg::scanline_u8 sl;

	renb.clear(rgb8(255, 255, 255));

//	renb.clip_box(30, 30, 160, 160); // 设置可写区域


	for(int i = 0; i < 5; ++i)
	{
		ccell.width(i * 20);
		ras.add_path(csccell);
		rensl.color(rgb8(0, 0, i * 50));
		agg::render_scanlines(ras,sl,rensl);
	}
	agg::arrowhead ah;
	ah.head(0, 10, 5, 5);
	ah.tail(10, 10, 5, 5);
	// 用path_storage生成一条直线
	agg::path_storage ps;
	ps.move_to(160, 60);
	ps.line_to(100, 100);
	// 转换
	agg::conv_stroke<agg::path_storage, agg::vcgen_markers_term> csps(ps);
	agg::conv_marker<agg::vcgen_markers_term, agg::arrowhead>
		arrow(csps.markers(), ah);
	// 画线
	ras.add_path(csps);
	agg::render_scanlines_aa_solid(ras, sl, renb, rgb8(0, 0, 0));
	// 画箭头
	ras.add_path(arrow);
	agg::render_scanlines_aa_solid(ras, sl, renb, argb16(1, 31, 0, 0));

	//从50,20开始，画20条长度为100的坚线，颜色从黑渐变到红，覆盖率为128(半透明)
	for(int i = 0; i < 31; ++i)
		pixf.blend_vline(50 + i*2, 20, 100, rgb5(i, 0, 0), 128);


	agg::int8u* p = rbuf.row_ptr(20); //得到第20行指针
	memset(p, 0, rbuf.stride_abs()); //整行以0 填充


}
*/
/*
static inline void aggcopy(BitmapPtr dst, agg::rendering_buffer& src)
{
	scrCopy(dst, (ConstBitmapPtr)src.buf());
}
static void aggtest()
{
	on_draw();
	aggcopy(pDesktopUp->GetPtr(), grbuf);
}
*/

/*
//基类同名函数映射。

#ifdef __GNUC__


#	define DefineFuncToBase(_name, _base, _type, _paralist...) _type _name(__MakeParaList(##_paralist)) { return _base::_name(__MakeArgList(##_paralist)); }
#	define DefineFuncToBaseVoid(_name, _base, _paralist...) void _name(__MakeParaList(##_paralist)) { _base::_name(__MakeArgList(##_paralist)); }
#	define DefineFuncToBaseConst(_name, _base, _type, _paralist...) _type _name(__MakeParaList(##_paralist)) const { return _base::_name(__MakeArgList(##_paralist)); }
#	define DefineFuncToBaseVoidConst(_name, _base, _paralist...) void _name(__MakeParaList(##_paralist)) const { _base::_name(__MakeArgList(##_paralist)); }
#	define __MakeParaList(_type, _name, _other...) _type _name, __MakeParaList(##_other)
#	define __MakeArgList(_type, _name, _other...) _name, __MakeArgList(##_other)
#	define __MakeParaList()
#	define __MakeArgList()

#else

#	define DefineFuncToBase(_name, _base, _type, _para, ...) _type _name {} { return _base::_name(_para, ##__VA_ARGS__); }
#	define DefineFuncToBaseVoid(_name, _base, _para, ...) void _name {} { _base::_name(_para, ##__VA_ARGS__); }
#	define DefineFuncToBaseConst(_name, _base, _type, _para, ...) _type _name {} const { return _base::_name(_para, ##__VA_ARGS__); }
#	define DefineFuncToBaseVoidConst(_name, _base, _para, ...) void _name {} const { _base::_name(_para, ##__VA_ARGS__); }

#endif
*/
/*
fullw
ARGB16(1,31,31,31);

static void dfrandom(BitmapPtr buf, u16 x, u16 y)
{
	buf[y * SCRW + x] = rand() & 0xffff;
}

m1
	ARGB16(((x*3+y*5)|(x*5+y*7))&1,(x+5)%31,y%31,(x&y)&31);

s1
	ARGB16(1, (x + 5) & 31, y & 31, (x & y) & 31);
t1
	ARGB16(1, (x + 2) % 14 + 13, y % 7 + 19, (x >> (y & 1)) % 11 + 6);
t2
	ARGB16(1, (x + 1) % 24 + 13, y % 17 + 9, (x >> (y & 1)) % 21 + 6);
b1
	ARGB16(1, (x + 5) % 23 + 4, y % 21 + 3, (x ^ y) % 22 + 7);
c1
	ARGB16(1, (x + 5) % 21 + 2, y % 23 + 4, (x & y) % 22 + 7);
ry1
	ARGB16(1, rand() & 31, rand() & 31, rand() & 15 & (rand() & 1) << 4);
ar1
	ARGB16(1, ((x + 5) % 83 + 4) & 31, (y % 71 + 3) & 31, (x ^ ~y) % 22 + 9);
ar2
	ARGB16(1, ((x + 5) % 89 + 4) & 31, (y % 23 + 3) & 31, (x ^ ~y) % 37 - 9);
u1
	ARGB16(1, ((x + 27) % 53 + 4) & 31, (~y % 233 + 3) & 31, (x * y) & 31);
v1
	ARGB16(1, ((x + y) % ((y - 2) & 1) + (x << 2)) & 31, (~x % 101 + y) & 31, ((x << 4) / (y & 1)) & 31);
yu1
	ARGB16(1, (y % x) & 31, (x % y) & 31, (x * y) & 31);
arz1
	ARGB16(1, ((x | y) % (y + 2)) & 31, ((~y | x) % 27 + 3) & 31, ((x << 6) / (y | 1)) & 31);
f1
	ARGB16(1, (x + y) & 31, (x - y) & 31, (x | y) & (x ^ y) & 31);
fl1
	ARGB16(1, (x + y * y) & 31, (x * x + y) & 31, ((x & y) ^ (x | y)) & 31);
fb1
	ARGB16(1, ((x + y) | y) & 31, ((x + y) | x) & 31, ((x ^ y) & (x ^ y)) & 31);
fb2
	ARGB16(1, ((x + y) & y) & 31, ((x + y) & x) & 31, ((x & y) ^ (x | y)) & 31);
rs1
	ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, ((x ^ y) + (x ^ y)) & 31);
rs2
	ARGB16(1, ((x * y) & y) & 31, ((x * y) & x) & 31, ((x & y) * (x | y)) & 31);
rs3
	ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, ((x ^ y) * (x ^ y)) & 31);
rs4
	ARGB16(1, ((x * y) & y) & 31, ((x * y) & x) & 31, ((x & y) + (x | y)) & 31);
rs5
	ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, (x * y) & 31);
rs6
	ARGB16(1, ((x * y) & y) & 31, ((x * y) & x) & 31, (x + y) & 31);
rs7
	ARGB16(1, ((x * y) | ~y) & 31, ((x * y) | ~x) & 31, ~(x * y) & 31);
rs8
	ARGB16(1, ((x * y) & ~y) & 31, ((x * y) & ~x) & 31, ~(x + y) & 31);
rs9
	ARGB16(1, (x + y) & 31, (x * y) & 31, (x - y) & 31);
ra1
	ARGB16(1, x & 31, y & 31, ~(x * y) & 31);
ra2
	ARGB16(1, ~y & 31, ~x & 31, (~x * ~y) & 31);
raz1
	ARGB16(1, (x << 2) + 15 & 31, (y << 2) + 15 & 31, (~(x * y) << 2) + 15 & 31);
raz2
	ARGB16(1, ((x >> 2) + 15) & 31, ((y >> 2) + 15) & 31, ((~(x * y) >> 2) + 15) & 31);
bza1
	ARGB16(1, ((x | y << 1) % (y + 2)) & 31, ((~y | x << 1) % 27 + 3) & 31, ((x << 4) / (y | 1)) & 31);
*/
