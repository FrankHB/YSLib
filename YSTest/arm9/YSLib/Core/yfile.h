// YSLib::Core::YFile by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41;
// UTime = 2010-7-25 10:39;
// Version = 0.1489;


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
	typedef u32 SizeType;

protected:
	FILE* fp; //默认文件指针。
	SizeType fsize; //文件大小。

public:
	explicit
	YFile(CPATH); //按指定文件路径初始化对象。
	virtual
	~YFile();

	DefBoolGetter(Valid, fp) //判断文件指针是否有效。
	DefGetter(FILE*, Ptr, fp) //取文件指针。
	DefGetter(SizeType, Length, fsize) //取文件长度。

	void
	Release(); //清除文件指针。

	bool
	Open(CPATH); //打开指定路径的文件。

	long
	ftell() const; //取文件指针的位置。
	void
	rewind() const; //文件指针返回到文件头。
	int
	fseek(SizeType offset, int origin) const; //设置文件指针位置，offset 和 origin 语义同 fseek 函数。
	int
	fread(void *p, int size, int count); //读文件到 p 中。
	int
	feof() const; //检测文件结束符。
};

inline long
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
YFile::fseek(SizeType offset, int origin) const
{
	return std::fseek(fp, offset, origin);
}
inline int
YFile::fread(void *p, int size, int count)
{
	return std::fread(p, size, count, fp);
}
inline int
YFile::feof() const
{
	return std::feof(fp);
}

YSL_END

#endif

