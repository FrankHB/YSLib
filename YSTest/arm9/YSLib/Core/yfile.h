// YSLib::Core::YFile by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41 + 08:00;
// UTime = 2010-10-22 13:01 + 08:00;
// Version = 0.1545;


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
	explicit
	YFile(CPATH); //按指定文件路径初始化对象。
	virtual
	~YFile();

	DefPredicate(Valid, fp) //判断文件指针是否有效。
	DefGetter(FILE*, Ptr, fp) //取文件指针。
	DefGetter(SizeType, Size, fsize) //取文件大小。

	//清除文件指针。
	void
	Release();

	//打开指定路径的文件。
	bool
	Open(CPATH);

	//取文件指针的位置，返回值语义同 std::ftell 。
	OffType
	ftell() const;

	//文件指针返回到文件头，同 std:: rewind 。
	void
	rewind() const;

	//设置文件指针位置，参数和返回值语义同 std::fseek 。
	int
	fseek(OffType, int) const;

	//连续读 nmemb 个大小为 size 文件块到 ptr 中，返回成功读取的文件块数。
	SizeType
	fread(void* ptr, SizeType size, SizeType nmemb) const;

	//检测文件结束符。
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

