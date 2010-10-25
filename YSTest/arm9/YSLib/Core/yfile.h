// YSLib::Core::YFile by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41 + 08:00;
// UTime = 2010-10-24 19:56 + 08:00;
// Version = 0.1677;


#ifndef INCLUDED_YFILE_H_
#define INCLUDED_YFILE_H_

// YFile ：平台无关的文件抽象。

#include "ystring.h"

YSL_BEGIN

//文件基类。
class YFile : public YObject
{
public:
	typedef YObject ParentType;
	typedef std::size_t SizeType; //大小类型。
	typedef std::ptrdiff_t OffType; //偏移量类型。

protected:
	FILE* fp; //默认文件指针。
	SizeType fsize; //文件大小。

public:
	//********************************
	//名称:		YFile
	//全名:		YSLib::YFile::YFile
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	构造：使用指定文件路径初始化对象。
	//备注:		
	//********************************
	explicit
	YFile(CPATH);
	//********************************
	//名称:		~YFile
	//全名:		YSLib::YFile::~YFile
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		自动关闭文件。
	//********************************
	virtual
	~YFile();

	DefPredicate(Valid, fp) //判断文件指针是否有效。
	DefGetter(FILE*, Ptr, fp) //取文件指针。
	DefGetter(SizeType, Size, fsize) //取文件大小。

	//********************************
	//名称:		Release
	//全名:		YSLib::YFile::Release
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除文件指针。
	//备注:		关闭文件。
	//********************************
	void
	Release();

	//********************************
	//名称:		Open
	//全名:		YSLib::YFile::Open
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	打开指定路径的文件。
	//备注:		
	//********************************
	bool
	Open(CPATH);

	//********************************
	//名称:		ftell
	//全名:		YSLib::YFile::ftell
	//可访问性:	public 
	//返回类型:	YSLib::YFile::OffType
	//修饰符:	const
	//功能概要:	取文件指针的位置，返回值语义同 std::ftell 。
	//备注:		
	//********************************
	OffType
	ftell() const;

	//********************************
	//名称:		rewind
	//全名:		YSLib::YFile::rewind
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//功能概要:	文件指针返回到文件头，同 std:: rewind 。
	//备注:		
	//********************************
	void
	rewind() const;

	//********************************
	//名称:		fseek
	//全名:		YSLib::YFile::fseek
	//可访问性:	public 
	//返回类型:	int
	//修饰符:	const
	//形式参数:	OffType
	//形式参数:	int
	//功能概要:	设置文件指针位置。
	//备注:		参数和返回值语义同 std::fseek 。
	//********************************
	int
	fseek(OffType, int) const;

	//********************************
	//名称:		fread
	//全名:		YSLib::YFile::fread
	//可访问性:	public 
	//返回类型:	YSLib::YFile::SizeType
	//修饰符:	const
	//形式参数:	void * ptr
	//形式参数:	SizeType size
	//形式参数:	SizeType nmemb
	//功能概要:	连续读 nmemb 个大小为 size 文件块到 ptr 中，
	//			返回成功读取的文件块数。
	//备注:		
	//********************************
	SizeType
	fread(void* ptr, SizeType size, SizeType nmemb) const;

	//********************************
	//名称:		feof
	//全名:		YSLib::YFile::feof
	//可访问性:	public 
	//返回类型:	int
	//修饰符:	const
	//功能概要:	检测文件结束符。
	//备注:		参数和返回值语义同 std::feof() 。
	//********************************
	int
	feof() const;
};

inline YFile::OffType
YFile::ftell() const
{
	return std::ftell(fp);
}

inline void
YFile::rewind() const
{
	std::rewind(fp);
}

inline int
YFile::fseek(YFile::OffType offset, int whence) const
{
	return std::fseek(fp, offset, whence);
}

inline std::size_t
YFile::fread(void* ptr, std::size_t size, std::size_t nmemb) const
{
	return std::fread(ptr, size, nmemb, fp);
}

inline int
YFile::feof() const
{
	return std::feof(fp);
}

YSL_END

#endif

