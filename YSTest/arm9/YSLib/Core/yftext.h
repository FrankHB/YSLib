// YSLib::Core::YFile_(Text) by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41 + 08:00;
// UTime = 2010-10-13 15:50 + 08:00;
// Version = 0.1523;


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
	SizeType bl; // BOM 大小。
	Text::CSID cp; //编码。

public:
	explicit
	YTextFile(CPATH); //按指定文件路径初始化对象。
	virtual DefEmptyDtor(YTextFile)

	DefGetter(u8, BOMSize, bl) //取 BOM 大小。
	DefGetter(Text::CSID, CP, cp) //取编码。
	DefGetter(SizeType, TextSize, GetSize() - GetBOMSize()) //取文本区段大小。
	DefGetter(SizeType, Pos, ftell() - bl) //取文件指针关于文本区段的位置。

	u8
	CheckBOM(Text::CSID&); //检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码，返回 BOM 长度。
	void
	Rewind() const; //文件读位置返回到文本区段头。
	void
	SetPos(u32) const; //设置文件读位置。
	void
	Seek(long, int whence) const; //设置文件读位置，whence 语义同 fseek 函数（SEEK_SET 起始为文本区段头）。
	SizeType
	Read(void* s, SizeType n) const; //从文件读 n 字节到 s 中。
	SizeType
	ReadS(uchar_t* s, SizeType n) const; //从文件读 n 字节（按默认编码转化为 UTF-16LE）到 s 中。
};

YSL_END

#endif

