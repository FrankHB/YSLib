// YSLib::Core::YFile_(Text) by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41 + 08:00;
// UTime = 2010-10-24 19:56 + 08:00;
// Version = 0.1567;


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
	//********************************
	//名称:		YTextFile
	//全名:		YSLib::YTextFile::YTextFile
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	构造：使用指定文件路径初始化对象。
	//备注:		
	//********************************
	explicit
	YTextFile(CPATH);
	virtual DefEmptyDtor(YTextFile)

	DefGetter(u8, BOMSize, bl) //取 BOM 大小。
	DefGetter(Text::CSID, CP, cp) //取编码。
	DefGetter(SizeType, TextSize, GetSize() - GetBOMSize()) //取文本区段大小。
	DefGetter(SizeType, Pos, ftell() - bl) //取文件指针关于文本区段的位置。

	//
	//********************************
	//名称:		CheckBOM
	//全名:		YSLib::YTextFile::CheckBOM
	//可访问性:	public 
	//返回类型:	u8
	//修饰符:	
	//形式参数:	Text::CSID &
	//功能概要:	检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码。
	//			返回 BOM 的长度。
	//备注:		
	//********************************
	u8
	CheckBOM(Text::CSID&);

	//********************************
	//名称:		Rewind
	//全名:		YSLib::YTextFile::Rewind
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//功能概要:	设置文件读位置为文本区段头。
	//备注:		
	//********************************
	void
	Rewind() const;

	//********************************
	//名称:		SetPos
	//全名:		YSLib::YTextFile::SetPos
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	u32
	//功能概要:	设置文件读位置。
	//备注:		
	//********************************
	void
	SetPos(u32) const;

	//********************************
	//名称:		Seek
	//全名:		YSLib::YTextFile::Seek
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	long
	//形式参数:	int whence
	//功能概要:	设置文件读位置。
	//备注:		whence 语义同 fseek 函数（除 SEEK_SET 表示起始为文本区段头）。
	//********************************
	void
	Seek(long, int whence) const;

	//********************************
	//名称:		Read
	//全名:		YSLib::YTextFile::Read
	//可访问性:	public 
	//返回类型:	SizeType
	//修饰符:	const
	//形式参数:	void * s
	//形式参数:	SizeType n
	//功能概要:	从文件读 n 字节到 s 中。
	//备注:		
	//********************************
	SizeType
	Read(void* s, SizeType n) const;

	//********************************
	//名称:		ReadS
	//全名:		YSLib::YTextFile::ReadS
	//可访问性:	public 
	//返回类型:	SizeType
	//修饰符:	const
	//形式参数:	uchar_t * s
	//形式参数:	SizeType n
	//功能概要:	从文件读 n 字节到 s 中。
	//备注:		按默认编码转化为 UTF-16LE 。
	//********************************
	SizeType
	ReadS(uchar_t* s, SizeType n) const;
};

YSL_END

#endif

