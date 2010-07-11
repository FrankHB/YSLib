// YSLib::Core::YFile by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:41;
// UTime = 2010-6-24 0:42;
// Version = 0.1250;


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

protected:
	FILE* fp; //默认文件指针。
	u32 fsize; //文件大小。

public:
	explicit
	YFile(CPATH); //按指定文件路径初始化对象。
	virtual
	~YFile();

	bool
	IsValid() const; //判断文件指针是否有效。
	FILE*
	GetFilePtr() const; //取文件指针。
	u32
	GetFileLen() const; //取文件长度。

	bool
	OpenFile(CPATH); //打开指定路径的文件。
	long
	ftell() const; //取文件指针的位置。
	void
	rewind() const; //文件指针返回到文件头。
	int
	fseek(s32 offset, int origin) const; //设置文件指针位置，offset 和 origin 语义同 fseek 函数。
	int
	fread(void *p, int size, int count); //读文件到 p 中。
	int
	fEOF(); //检测文件结束符。
};

inline bool
YFile::IsValid() const
{
	return fp;
}
inline FILE*
YFile::GetFilePtr() const
{
	return fp;
}
inline u32
YFile::GetFileLen() const
{
	return fsize;
}

inline long
YFile::ftell() const
{
	return ::ftell(fp);
}
inline void
YFile::rewind() const
{
	::rewind(fp);
}
inline int
YFile::fseek(s32 offset, int origin) const
{
	return ::fseek(fp, offset, origin);
}
inline int
YFile::fread(void *p, int size, int count)
{
	return ::fread(p, size, count, fp);
}
inline int
YFile::fEOF()
{
#ifdef feof
	return feof(fp);
#else
	return ::feof(fp);
#endif
}

YSL_END

#endif

