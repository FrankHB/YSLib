// YSLib::Core::YFile_(Text) by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41;
// UTime = 2010-7-25 9:25;
// Version = 0.1474;


#ifndef INCLUDED_YFTEXT_H_
#define INCLUDED_YFTEXT_H_

// YFile_(Text) ：平台无关的文本文件抽象。

#include "yfile.h"

YSL_BEGIN

//文本文件类。
class YTextFile : public YFile
{
public:
	typedef YFile ParentType;

private:
	u8 bl; // BOM 长度。
	Text::CSID cp; //默认编码。

public:
	explicit
	YTextFile(CPATH); //按指定文件路径初始化对象。
	virtual
	~YTextFile();

	DefGetter(u8, BOMLen, bl) //取 BOM 长度。
	DefGetter(Text::CSID, CP, cp) //取默认编码。
	DefGetter(u32, Pos, ftell() - bl) //取文件指针关于文本区段的位置。

	u8
	CheckBOM(Text::CSID&); //检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码，返回 BOM 长度。
	void
	Rewind() const; //文件读位置返回到文本区段头。
	void
	SetPos(u32) const; //设置文件读位置。
	void
	Seek(s32, int origin) const; //设置文件读位置，origin 语义同 fseek 函数（SEEK_SET 起始为文本区段头）。
	u32
	Read(void* s, u32 n); //读文件 n 字节到 s 中。
	u32
	ReadS(uchar_t* s, u32 n) const; //读文件 n 字节（按默认编码转化为 UTF-16LE）到 s 中。
};

YSL_END

#endif

