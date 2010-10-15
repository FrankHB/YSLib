
/*
		u8* bitmap = sbit->buf;
		if(!bitmap)
		{
			penX = tx;
			return;
		}

		int maxX = Width - Right;
		int maxY = Height - Bottom;

		PixelType col(color | BITALPHA);
		int t(0);
		for(int y(0); y < sbit->height; ++y)
		{
			for(int x(0); x < sbit->width; ++x)
			{
				if(bitmap[t] >= Alpha_Threshold)
				{
					int sy = penY + y - sbit->top;
					int sx = penX + x + sbit->left;

					if(sx >= Left && sy >= Top && sx < maxX && sy < maxY)
					{
						int s = sy * Width + sx;
						imgAlpha[s] = bitmap[t];
						img[s] = col;
					}
				}
				++t;
			}
		}

//static const u8 SingleColor_Threshold = 31;

/*
inline static PixelType
mpshl(PixelType c, u8 n, PixelType mask)
{
	return ((c & mask) << n) & mask;
}
inline static PixelType
mpshlR(PixelType c, u8 n)
{
	return mpshl(c, n, 0x7C00);
}
inline static PixelType
mpshlG(PixelType c, u8 n)
{
	return mpshl(c, n, 0x03E0);
}
inline static PixelType
mpshlB(PixelType c, u8 n)
{
	return mpshl(c, n, 0x001F);
}
inline static PixelType
mpshr(PixelType c, u8 n, PixelType mask)
{
	return ((c & mask) >> n) & mask;
}
inline static PixelType
mpshrR(PixelType c, u8 n)
{
	return mpshr(c, n, 0x7C00);
}
inline static PixelType
mpshrG(PixelType c, u8 n)
{
	return mpshr(c, n, 0x03E0);
}
inline static PixelType
mpshrB(PixelType c, u8 n)
{
	return mpshr(c, n, 0x001F);
}


//template<typename _tPixel>
struct transPixelShiftLeft
{
	u8 nShift;

	inline explicit
	transPixelShiftLeft(u8 n)
	: nShift(n)
	{}

	inline void
	operator()(BitmapPtr dst)
	{
		PixelType c = *dst;

		*dst = (c & BITALPHA)
			| mpshlR(c, nShift)
			| mpshlG(c, nShift)
			| mpshlB(c, nShift);
	}
};

struct transPixelShiftRight
{
	u8 nShift;

	inline explicit
	transPixelShiftRight(u8 n)
	: nShift(n)
	{}

	inline void
	operator()(BitmapPtr dst)
	{
		PixelType c = *dst;

		*dst = (c & BITALPHA)
			| mpshrR(c, nShift)
			| mpshrG(c, nShift)
			| mpshrB(c, nShift);
	}
};

*/
/*

class Path;

void swap(Path&, Path&);
bool lexicographical_compare(Path::iterator, Path::iterator,
							 Path::iterator, Path::iterator);

bool operator==(const Path&, const Path&);
bool operator!=(const Path&, const Path&);
bool operator<(const Path&, const Path&);
bool operator<=(const Path&, const Path&);
bool operator>(const Path&, const Path&);
bool operator>=(const Path&, const Path&);

path operator/(const Path&, const Path&);

std::ostream& operator<<(std::ostream&, const Path&);
std::wostream& operator<<(std::wostream&, const Path&);
std::istream& operator>>(std::istream&, Path&);
std::wistream& operator>>(std::wistream&, Path&)

class FileSystemError;
class DirectoryEntry;

class HDirectory;
class HRecursiveDirectory;

struct EFileType
{
	enum FileType
	{
		StatusError,
		NotFound,
		Regular,
		Directory,
	//	Symlink,
	//	Block,
		Character,
		FIFO,
	//	Socket,
		UnknownType
	};
};

struct FileStatus;

typedef uintmax_t FileSizeType;

struct SpaceInfo  //空间计算函数返回。
{
	FileSizeType Capacity;
	FileSizeType Free; 
	FileSizeType Available; //非特权进程可用空间。
};

struct ECopyOption
{
	enum //IfExists
	{
		Fail,
		Overwrite
	};
};


//操作。
bool IsDirectory(FileStatus);
bool IsDirectory(const Path&);
bool IsEmpty(const Path&);
bool IsKnownStatus(FileStatus);
bool IsOther(FileStatus);
bool IsOther(const Path&,);
bool IsRegularFile(FileStatus); 
bool IsRegularFile(const Path&);

bool BeEquivalent(const Path&, const Path&);

bool Exists(FileStatus);
bool Exists(const Path&);

Path			GetAbsolutePath(const Path&, const Path& = GetCurrentPath());
std::time_t		GetLastWriteTime(const Path&);
SpaceInfo		GetSpaceInfo(const Path&);
FileStatus		GetStatus(const Path&);
Path			GetCurrentPath();
void			GetCurrentPath(Path&);
FileSizeType	GetFileSize(const Path&);

void SetLastWriteTime(const Path&, const std::time_t);

bool CreateDirectory(const Path&);
bool CreateDirectories(const Path&);

void CopyFile(const Path&, const Path&);
void CopyFile(const Path&, const Path&, ECopyOption);

bool Remove(const Path&);

FileSizeType	RemoveAll(const Path&);

void			Rename(const Path&, const Path&);

Path SystemComplete(const Path&);
//Path		unique_path(const Path& model="%%%%-%%%%-%%%%-%%%%");


typedef char NativePathCharType; //本地路径字符类型，POSIX 为 char ，Windows 为 wchar_t。
*/
/*
struct utf8_codecvt_facet :
	public std::codecvt<wchar_t, char, std::mbstate_t>
{
public:
	explicit utf8_codecvt_facet(std::size_t no_locale_manage = 0)
		: std::codecvt<wchar_t, char, std::mbstate_t>(no_locale_manage)
	{}

protected:
	virtual std::codecvt_base::result do_in(
		std::mbstate_t& state, 
		const char* from,
		const char* from_end, 
		const char*& from_next,
		wchar_t* to, 
		wchar_t* to_end, 
		wchar_t*& to_next
		) const;

	virtual std::codecvt_base::result do_out(
		std::mbstate_t& state, const wchar_t* from,
		const wchar_t* from_end, const wchar_t*& from_next,
		char* to, char* to_end, char *& to_next
		) const;

	bool invalid_continuing_octet(unsigned char octet_1) const
	{
		return octet_1 < 0x80 || 0xbf < octet_1;
	}

	bool invalid_leading_octet(unsigned char octet_1) const
	{
		return (0x7F < octet_1 && octet_1 < 0xC0) || (octet_1 > 0xFD);
	}

	// continuing octets = octets except for the leading octet
	static unsigned int get_cont_octet_count(unsigned char lead_octet)
	{
		return get_octet_count(lead_octet) - 1;
	}

	static unsigned int get_octet_count(unsigned char lead_octet);

	// How many "continuing octets" will be needed for this word
	// ==   total octets - 1.
	int get_cont_octet_out_count(wchar_t word) const;

	virtual bool
	do_always_noconv() const throw()
	{
		return false;
	}

	// UTF-8 isn't really stateful since we rewind on partial conversions
	virtual std::codecvt_base::result do_unshift(
		std::mbstate_t&,
		char * from,
		char * ,//to,
		char * & next
		) const 
	{
		next = from;
		return ok;
	}

	virtual int do_encoding() const throw()
	{
		const int variable_byte_external_encoding=0;
		return variable_byte_external_encoding;
	}

	// How many char objects can I process to get <= max_limit
	// wchar_t objects?
	virtual int do_length(
		const std::mbstate_t&,
		const char* from,
		const char* from_end, 
		std::size_t max_limit
		) const;

	// Largest possible value do_length(state,from,from_end,1) could return.
	virtual int do_max_length() const throw ()
	{
		return 6; // largest UTF-8 encoding of a UCS-4 character
	}
};

class Path
{
public:
	typedef NativePathCharType value_type;
	typedef basic_string<value_type> StringType;
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

private:
	StringType Pathname;

public:
	//编码转换。
	static std::locale imbue(const std::locale&);
	static const codecvt_type& codecvt();

	// constructors and destructor
	Path();
	Path(const Path& p);
//	template<class _tSource>
//	Path(_tSource const&, const codecvt_type& = codecvt());
//	template<class _tInIt>
//	Path(_tInIt, _tInIt, const codecvt_type& = codecvt());
	~Path();

	//赋值。
	Path&
	operator=(const Path& p);
//	template<class _tSource>
//	Path&
//	operator=(const _tSource& source);

//	template<class _tSource>
//	Path&
//	assign(_tSource const& source, const codecvt_type&)
//	template<class _tInIt>
//	Path&
//	assign(_tInIt, _tInIt, const codecvt_type& = codecvt());


	//追加路径。
	Path&
	operator/=(const Path& p);
//	template<class _tSource>
//	Path&
//	operator/=(const _tSource&);

//	template<class _tSource>
//	Path&
//	append(const _tSource&, const codecvt_type&);
//	template<class _tInIt>
//	Path&
//	append(_tInIt begin, _tInIt end, const codecvt_type& = codecvt());

	//modifiers
	void clear();
	void swap(Path& rhs) ythrow();

	Path& MakeAbsolute(const path& base);
	Path& MakePreferred(); // POSIX: no effect. Windows: convert slashes to backslashes
	Path& RemoveFilename();
	Path& ReplaceExtension(const path& new_extension = path());

	const string_type&
	native() const; //本地格式和编码。
	const value_type*
	c_str() const; //本地格式和编码的 C 风格字符串。

	template<class _tString>
	_tString GetString(const codecvt_type& = codecvt()) const; //本地字符串格式。

//	const string    string(const codecvt_type& = codecvt()) const; // native format
//	const wstring   wstring(const codecvt_type& = codecvt()) const; // ditto
//	const u16string u16string() const; // ditto
//	const u32string u32string() const; // ditto

	template<class _tString>
	_tString GetGenericString() const;
//	const string    GetGenericString(const codecvt_type& cvt=codecvt()) const;   // generic format
//	const wstring   GetGenericString(const codecvt_type& cvt=codecvt()) const;  // ditto
//	const u16string GetGenericString() const;                                 // ditto
//	const u32string GetGenericString() const;                                 // ditto

	//查询。
	bool empty() const;
	bool IsAbsolute() const;
	bool IsRelative() const;
	bool HasRootName() const;
	bool HasRootDirectory() const;
	bool HasRootPath() const;
	bool HasRelativePath() const;
	bool HasParentPath() const;
	bool HasFilename() const;
	bool HasStem() const;
	bool HasExtension() const;

	//路径分解。
	Path GetRootName() const;
	Path GetRootDirectory() const;
	Path GetRootPath() const;
	Path GetRelativePath() const;
	Path GetParentPath() const;
	Path GetFilename() const;
	Path GetStem() const;
	Path GetExtension() const;

	//迭代器。
	class iterator;
	typedef iterator const_iterator;

	iterator begin() const;
	iterator end() const;
};
*/
//	lblC.RequestFocus();
//	lblC.ReleaseFocus();

//	lblC.Text = L"测试= =";
//	HWND h(lblC.GetWindowHandle());
//	lblC.Text[0] ^= 1;
//	String::iterator i(lblC.Text.begin());
//	++*i;
//	tf.SetPos(0);
//	tf.Seek(-128, SEEK_END);
//	sprintf(strtf, "%d,%d,%d,%d,%d", tf.IsValid(), tf.GetBOMLen(), tf.GetLen(), tf.GetCP(), tf.GetPos());

/*
struct non_inheritable
{
};
static void
_testStr(const String& s)
{
	char* str = sdup(str.c_str(), '?');
	yprintf("%s\n",x,str);
	free(str);
}
*/
/*
static void
RectDarken(BitmapPtr src, const Point& l, const Size& s, HWND hWnd)
{
	YAssert(hWnd, "err: hWnd is null.");

	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), Rect(l, s), s, transPixelEx, transSeq());
//	transRect()(hWnd->GetBufferPtr(), hWnd->GetSize(), Rect(l, s), transPixelShiftRight(1), transSeq());


	IWidget* pWgt(dynamic_cast<IWidget*>(hWnd->GetFocusingPtr()));

	if(pWgt)
		DrawWidgetBounds(*pWgt, Color(255, 1, 255));

	GraphicInterfaceContext g(hWnd->GetBufferPtr(), hWnd->GetBounds());
//	DrawLineSeg(g, 2, 4, hWnd->GetWidth(), hWnd->GetHeight(), Color(72, 32, 255));
//	DrawLineSeg(g, 2, 10, 12, 65, Color(8, 255, 8));

	DrawWindowBounds(hWnd, Color(8, 8, 255);
//	DrawHLineSeg(g, 1, 10, 65, Color(8, 8, 255));
//	DrawVLineSeg(g, 1, 10, 65, Color(88, 8, 255));
//	DrawPoint(g, 3, 3, Color(88, 8, 255));
//	DrawRect(g, 11, 12, 19, 24, Color(32, 255, 255));
//	DrawWndBounds(hWnd, Color(32, 255, 72));
	DrawLineSeg(g, 2, 3, 22, 9, Color(255, 0, 0));
	DrawLineSeg(g, 22, 3, 42, 28, Color(255, 120, 0));
	DrawLineSeg(g, 52, 29, 62, 3, Color(255, 255, 0));
	DrawLineSeg(g, 72, 11, 82, 3, Color(0, 255, 0));
	DrawLineSeg(g, 102, 5, 82, 13, Color(0, 0, 255));
	DrawLineSeg(g, 122, 3, 102, 35, Color(88, 0, 255));
	DrawLineSeg(g, 122, 9, 142, 1, Color(255, 0, 255));
	DrawLineSeg(g, 132, 35, 152, 3, Color(255, 168, 255));
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

class Path
{
public:
	typedef String PathType;
	typedef PathType& RefType;
	typedef const PathType& CRefType;

protected:
	PathType Directory;
	PathType File;

public:
	Path(CRefType);

	CRefType GetDirectory() const;
	CRefType GetFile() const;

	void SetPath(CRefType);
};

inline Path::Path(Path::CRefType path)
{
	SetPath(path);
}

inline Path::CRefType Path::GetDirectory() const
{
	return Directory;
}
inline Path::CRefType Path::GetFile() const
{
	return File;
}

inline void Path::SetPath(Path::CRefType path)
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
	Path alloc;

public:
	YListControl();
};

YFileListControl(CPATH path)
{}

*/

//unverified:

/*


class YLabeledWidget : public GMCounter<LabeledWidget>, public YWidget
{
protected:
	YButton& label;
	YWidget& widget;

public:
	template<class _CharT>
		YLabeledWidget(HWND hWnd, const _CharT*, YWidget*, const Font::SizeType = Font::DefSize);
	virtual ~LabeledWidget();

	virtual void SetBounds(s16 x, s16 y, u16 w, u16 h);
	virtual void SetBounds(Rect r);

};
void YLabeledWidget::SetBounds(Rect& r)
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
	YButton* label;
	YImage* image;

public:
	String& Caption;
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
	TextRegion& shltxt = dsr.GetUp();
	sprintf(strtbuf + strlen(strtbuf), "buffer addr : 0x%x", (int)shltxt.GetBufferPtr());
	sprintf(strtmargin, "%d, %d, %d, %d;", shltxt.Margin.GetLeft(), shltxt.Margin.GetRight(), shltxt.Margin.GetTop(), shltxt.Margin.GetBottom());
	sprintf(strtf,"xxx:%u,%u;%u,%u\n",shltxt.Margin.GetBottom(),shltxt.ResizeMargin(),shltxt.GetPenX(),shltxt.GetPenY());
	sprintf(strtpen, "penX = %d, penY = %d;", shltxt.GetPenX(), shltxt.GetPenY());
	*/

//AGG Drawing Test

/*
static u8* frameBuffer = new u8[sizeof(ScreenBufferType)];
static agg::rendering_buffer& grbuf = *new agg::rendering_buffer(frameBuffer, SCRW, SCRH, SCRW * sizeof(PixelType));

typedef agg::renderer_base<agg::pixfmt_argb16> rend_type;
static rend_type rend;//(grbuf);
static agg::rasterizer_outline<rend_type> rast(rend);*/

////

/*
static void
ad_rect(const Rect& r, const agg::rgba8* c)
{
	rast.move_to(r.x, r.y);
	rast.line_to(r.x + r.w, r.y);
	rast.line_to(r.x + r.w, r.y + r.h);
	rast.line_to(r.x, r.y + r.h);
	rast.line_to(r.x, r.y);
	if(c)
		rast.render(rend, *c);
}
typedef vector<Point> poly; //多边形。
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
Color(255, 255, 255);

static void dfrandom(BitmapPtr buf, u16 x, u16 y)
{
	buf[y * SCRW + x] = rand() & 0xffff;
}

m1
	RGB15(((x * 3 + y * 5) | (x * 5 + y * 7)) & 1, (x + 5) % 31, y % 31, (x & y) & 31);

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
