/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GBKEX.cpp
\ingroup YReader
\brief 测试文件。
\version 0.3055; *Build 190 r17;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11;
\par 修改时间:
	2011-02-05 23:47 + 08:00;
\par 字符集:
	ANSI / GBK;
\par 模块名称:
	YReader::GBKEX;
\deprecated 临时测试用途。
*/


#include <ydef.h>
#include <Shells.h>

// GBK coded definitions:

namespace YSLib
{
	CPATH DEF_DIRECTORY = "/Data/";
	//const char* DEF_FONT_NAME = "方正姚体";
	//CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_DIRECTORY = "/Font/";
	/*
	using namespace Components;
	using namespace Widgets;
	using namespace Controls;
	*/
	using namespace Drawing;

	namespace
	{
		class TestObj
		{
		public:
			GHHandle<YDesktop> h;
			Color c;
			Point l;
			Size s;

			TestObj()
				: h(hDesktopDown), c(ColorSpace::White),
				l(20, 32), s(120, 90)
			{}

			void
			Fill()
			{
				FillRect(h->GetContext(), l, s, c);
			}

			void
			Pause()
			{
				h->Refresh();
				h->Update();
				WaitForInput();
			}

			void
			Blit(const TextRegion& tr)
			{
				tr.BlitTo(h->GetContext().GetBufferPtr(),
					h->GetContext().GetSize(), Point::Zero, l, tr.GetSize());
			}
		};
	}

	void
	ShlSetting::TFormC::btnD_Click(TouchEventArgs& e)
	{
	//	static int test_state = 0;

		YDebugSetStatus();

		{
			uchar_t* tstr(Text::ucsdup("Abc测试", Text::CS_Local));
			String str(tstr);

			std::free(tstr);

			TestObj t;
		//	const Graphics& g(t.h->GetContext());

			TextRegion tr;
			tr.SetSize(t.s.Width, t.s.Height);
			TextRegion& tr2(*GetGlobalResource<TextRegion>());

			switch(e.X * 4 / btnD.GetWidth())
			{
			case 0:
				t.Fill();
				t.Pause();

			//	tr.BeFilledWith(ColorSpace::Black);
				PutLine(tr, str);
				t.Blit(tr);
				t.Pause();

				t.Fill();				
				tr.ClearImage();
				tr.ResetPen();
				tr.Color = ColorSpace::Black;
				PutLine(tr, str);
				t.Blit(tr);
				t.Pause();

				t.Fill();
				tr.ClearImage();
				tr.ResetPen();
				tr.Color = ColorSpace::Blue;
				PutLine(tr, str);
				t.Pause();

				t.Blit(tr);
				t.Pause();

			case 1:
				tr2.SetSize(t.s.Width, t.s.Height);

				t.Fill();
				t.Pause();

				//	tr2.BeFilledWith(ColorSpace::Black);
				tr2.ClearImage();
				tr2.ResetPen();
				tr2.Color = ColorSpace::White;
				PutLine(tr2, str);
				t.Blit(tr2);
				t.Pause();

				t.Fill();
				tr2.ClearImage();
				tr2.ResetPen();
				tr2.Color = ColorSpace::Black;
				PutLine(tr2, str);
				t.Blit(tr2);
				t.Pause();

				t.Fill();
				tr2.ClearImage();
				tr2.ResetPen();
				tr2.Color = ColorSpace::Red;
				t.Blit(tr2);
				PutLine(tr2, str);
				t.Pause();

				t.Blit(tr2);
				t.Pause();
				break;

			case 2:
				t.c = ColorSpace::Lime;

				t.Fill();
				t.Pause();

			//	tr.BeFilledWith(ColorSpace::Black);
				PutLine(tr, str);
				t.Blit(tr);
				t.Pause();

				t.Fill();				
				tr.ClearImage();
				tr.ResetPen();
				tr.Color = ColorSpace::Black;
				PutLine(tr, str);
				t.Blit(tr);
				t.Pause();

				t.Fill();
				tr.ClearImage();
				tr.ResetPen();
				tr.Color = ColorSpace::Blue;
				PutLine(tr, str);
				t.Pause();

				t.Blit(tr);
				t.Pause();

			case 3:
				t.c = ColorSpace::Lime;

				tr2.SetSize(t.s.Width, t.s.Height);

				t.Fill();
				t.Pause();

				//	tr2.BeFilledWith(ColorSpace::Black);
				tr2.ClearImage();
				tr2.ResetPen();
				tr2.Color = ColorSpace::White;
				PutLine(tr2, str);
				t.Blit(tr2);
				t.Pause();

				t.Fill();
				tr2.ClearImage();
				tr2.ResetPen();
				tr2.Color = ColorSpace::Black;
				PutLine(tr2, str);
				t.Blit(tr2);
				t.Pause();

				t.Fill();
				tr2.ClearImage();
				tr2.ResetPen();
				tr2.Color = ColorSpace::Red;
				t.Blit(tr2);
				PutLine(tr2, str);
				t.Pause();

				t.Blit(tr2);
				t.Pause();

			default:
				break;
			}
		}
	}
}

char gstr[128] = "你妹喵\t的= =ijkAB DEǎ。i:みま╋㊣F2\t3Xsk\nw\vwwww";

