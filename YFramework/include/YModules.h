/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YFramework YFramework Library
\brief YFramework 框架库。
*/

/*!	\defgroup YSLib YShell Library
\ingroup YFramework
\brief YShell 库。
*/

/*!	\file YModules.h
\ingroup YFramework
\brief YSLib 库模块配置文件。
\version r156
\author FrankHB <frankhb1989@gmail.com>
\since build 461
\par 创建时间:
	2013-12-23 16:44:41 +0800
\par 修改时间:
	2013-12-23 18:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YModules
*/


#ifndef YF_INC_YModules_h_
#define YF_INC_YModules_h_ 1

#define YFM_YBaseMacro <YBaseMacro.h>

#define YFM_CHRLib_CHRDefinition <CHRLib/chrdef.h>
#define YFM_CHRLib_CharacterMapping <CHRLib/chrmap.h>
#define YFM_CHRLib_CharacterProcessing <CHRLib/chrproc.h>
#define YFM_CHRLib_Encoding <CHRLib/encoding.h>
#define YFM_CHRLib_Convert <CHRLib/Convert.hpp>
#define YFM_CHRLib_StaticMapping <CHRLib/smap.hpp>
#define YFM_CHRLib_MappingEx <CHRLib/MapEx.h>

#define YFM_YCLib_Platform <YCLib/Platform.h>
#define YFM_YCLib_YCommon <YCLib/ycommon.h>
#define YFM_YCLib_NativeAPI <YCLib/NativeAPI.h>
#define YFM_YCLib_Debug <YCLib/Debug.h>
#define YFM_YCLib_FileSystem <YCLib/FileSystem.h>
#define YFM_YCLib_Timer <YCLib/Timer.h>
#define YFM_YCLib_Video <YCLib/Video.h>
#define YFM_YCLib_Keys <YCLib/Keys.h>
#define YFM_YCLib_Input <YCLib/Input.h>
#define YFM_YCLib_MemoryMapping <YCLib/MemoryMapping.h>

#define YFM_YSLib_Adaptor_YAdaptor <YSLib/Adaptor/YAdaptor.h>
#define YFM_YSLib_Adaptor_YNew <YSLib/Adaptor/YNew.h>
#define YFM_YSLib_Adaptor_YContainer <YSLib/Adaptor/ycont.h>
#define YFM_YSLib_Adaptor_YReference <YSLib/Adaptor/yref.hpp>
#define YFM_YSLib_Adaptor_YTextBase <YSLib/Adaptor/YTextBase.h>
#define YFM_YSLib_Adaptor_Configuration <YSLib/Adaptor/config.h>
#define YFM_YSLib_Adaptor_Image <YSLib/Adaptor/Image.h>
#define YFM_YSLib_Adaptor_Font <YSLib/Adaptor/Font.h>
#define YFM_YSLib_Adaptor_Configuration <YSLib/Adaptor/config.h>

#define YFM_YSLib_Core_YShellDefinition <YSLib/Core/ysdef.h>
#define YFM_YSLib_Core_YException <YSLib/Core/yexcept.h>
#define YFM_YSLib_Core_YCoreUtilities <YSLib/Core/ycutil.h>
#define YFM_YSLib_Core_YObject <YSLib/Core/yobject.h>
#define YFM_YSLib_Core_YFunc <YSLib/Core/yfunc.h>
#define YFM_YSLib_Core_YEvent <YSLib/Core/yevt.hpp>
#define YFM_YSLib_Core_YString <YSLib/Core/ystring.h>
#define YFM_YSLib_Core_YGDIBase <YSLib/Core/ygdibase.h>
#define YFM_YSLib_Core_YDevice <YSLib/Core/ydevice.h>
#define YFM_YSLib_Core_YMessage <YSLib/Core/ymsg.h>
#define YFM_YSLib_Core_YMessageDefinition <YSLib/Core/ymsgdef.h>
#define YFM_YSLib_Core_YFileSystem <YSLib/Core/yfilesys.h>
#define YFM_YSLib_Core_YShell <YSLib/Core/yshell.h>
#define YFM_YSLib_Core_YApplication <YSLib/Core/yapp.h>
#define YFM_YSLib_Core_YCounter <YSLib/Core/ycounter.hpp>
#define YFM_YSLib_Core_YStorage <YSLib/Core/ystorage.hpp>
#define YFM_YSLib_Core_ValueNode <YSLib/Core/ValueNode.h>
#define YFM_YSLib_Core_Task <YSLib/Core/Task.h>
#define YFM_YSLib_Core_Cache <YSLib/Core/Cache.hpp>

#define YFM_YSLib_Service_YTimer <YSLib/Service/ytimer.h>
#define YFM_YSLib_Service_YResource <YSLib/Service/yres.h>
#define YFM_YSLib_Service_YPixel <YSLib/Service/YPixel.h>
#define YFM_YSLib_Service_YBlit <YSLib/Service/yblit.h>
#define YFM_YSLib_Service_YDraw <YSLib/Service/ydraw.h>
#define YFM_YSLib_Service_YGDI <YSLib/Service/ygdi.h>
#define YFM_YSLib_Service_YFile <YSLib/Service/yfile.h>
#define YFM_YSLib_Service_YFile_Text_ <YSLib/Service/yftext.h>
#define YFM_YSLib_Service_YTextBase <YSLib/Service/YTextBase.h>
#define YFM_YSLib_Service_YTextLayout <YSLib/Service/YTextLayout.h>
#define YFM_YSLib_Service_YTextRenderer <YSLib/Service/YTextRenderer.h>
#define YFM_YSLib_Service_TextManager <YSLib/Service/textmgr.h>
#define YFM_YSLib_Service_CharRenderer <YSLib/Service/CharRenderer.h>
#define YFM_YSLib_Service_ContentType <YSLib/Service/ContentType.h>
#define YFM_YSLib_Service_AccessHistory <YSLib/Service/AccessHistory.hpp>

#define YFM_YSLib_UI_YComponent <YSLib/UI/YComponent.h>
#define YFM_YSLib_UI_YStyle <YSLib/UI/ystyle.h>
#define YFM_YSLib_UI_YRenderer <YSLib/UI/yrender.h>
#define YFM_YSLib_UI_YFocus <YSLib/UI/yfocus.h>
#define YFM_YSLib_UI_YWidgetEvent <YSLib/UI/ywgtevt.h>
#define YFM_YSLib_UI_YWidgetView <YSLib/UI/ywgtview.h>
#define YFM_YSLib_UI_YWidget <YSLib/UI/ywidget.h>
#define YFM_YSLib_UI_YControl <YSLib/UI/ycontrol.h>
#define YFM_YSLib_UI_YBrush <YSLib/UI/YBrush.h>
#define YFM_YSLib_UI_YGUI <YSLib/UI/ygui.h>
#define YFM_YSLib_UI_YUIContainer <YSLib/UI/yuicont.h>
#define YFM_YSLib_UI_YPanel <YSLib/UI/ypanel.h>
#define YFM_YSLib_UI_YWindow <YSLib/UI/ywindow.h>
#define YFM_YSLib_UI_YDesktop <YSLib/UI/ydesktop.h>
#define YFM_YSLib_UI_Console <YSLib/UI/Console.h>
#define YFM_YSLib_UI_WidgetIteration <YSLib/UI/WidgetIteration.h>
#define YFM_YSLib_UI_Animation <YSLib/UI/Animation.h>
#define YFM_YSLib_UI_Border <YSLib/UI/Border.h>
#define YFM_YSLib_UI_Hover <YSLib/UI/Hover.h>
#define YFM_YSLib_UI_Viewer <YSLib/UI/viewer.hpp>
#define YFM_YSLib_UI_Loader <YSLib/UI/Loader.h>
#define YFM_YSLib_UI_Label <YSLib/UI/label.h>
#define YFM_YSLib_UI_Button <YSLib/UI/button.h>
#define YFM_YSLib_UI_UIContainerEx <YSLib/UI/uicontx.h>
#define YFM_YSLib_UI_Form <YSLib/UI/form.h>
#define YFM_YSLib_UI_Selector <YSLib/UI/Selector.h>
#define YFM_YSLib_UI_Scroll <YSLib/UI/scroll.h>
#define YFM_YSLib_UI_Progress <YSLib/UI/progress.h>
#define YFM_YSLib_UI_TextArea <YSLib/UI/textarea.h>
#define YFM_YSLib_UI_TextList <YSLib/UI/textlist.h>
#define YFM_YSLib_UI_ComboList <YSLib/UI/ComboList.h>
#define YFM_YSLib_UI_Menu <YSLib/UI/menu.h>

#define YFM_NPL_Lexical <NPL/Lexical.h>
#define YFM_NPL_SContext <NPL/SContext.h>
#define YFM_NPL_Configuration <NPL/Configuration.h>

#define YFM_Helper_YGlobal <Helper/yglobal.h>
#define YFM_Helper_Initialization <Helper/Initialization.h>
#define YFM_Helper_InputManager <Helper/InputManager.h>
#define YFM_Helper_GUIShell <Helper/GUIShell.h>
#define YFM_Helper_GUIApplication <Helper/GUIApplication.h>
#define YFM_Helper_Host <Helper/Host.h>
#define YFM_Helper_HostedUI <Helper/HostedUI.h>
#define YFM_Helper_HostRenderer <Helper/HostRenderer.h>
#define YFM_Helper_ScreenBuffer <Helper/ScreenBuffer.h>
#define YFM_Helper_HostWindow <Helper/HostWindow.h>
#define YFM_Helper_ShellHelper <Helper/ShellHelper.h>

#endif

