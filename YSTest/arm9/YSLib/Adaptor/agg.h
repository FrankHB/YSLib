/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file agg.h
\ingroup Adaptor
\brief Anti-Grain Geometry 库关联。
\version 0.1674;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-21 15:46:00 + 08:00;
\par 修改时间:
	2010-12-17 18:57 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::AGG;
*/


#ifndef AGG_INCLUDED
#define AGG_INCLUDED

//#include <agg/agg_pixfmt_rgb.h>
//#include <agg/agg_pixfmt_rgba.h>
#include <agg/agg_pixfmt_rgb_packed.h>

#include <agg/agg_rendering_buffer.h>
#include <agg/agg_rasterizer_scanline_aa.h>
#include <agg/agg_scanline_u.h>
#include <agg/agg_renderer_scanline.h>

#include <agg/agg_path_storage.h> //path_storage
#include <agg/agg_ellipse.h>  // ellipse
#include <agg/agg_arc.h> // arc
#include <agg/agg_arrowhead.h> // arrowhead
#include <agg/agg_curves.h> // curve3, curve4
#include <agg/agg_gsv_text.h> // gsv_text, gsv_text_outline
#include <agg/agg_rounded_rect.h> // rounded_rect

#include <agg/agg_span_allocator.h>
#include <agg/agg_span_solid.h>
#include <agg/agg_span_gradient.h>
#include <agg/agg_span_gradient_alpha.h>
#include <agg/agg_span_gouraud_gray.h>
#include <agg/agg_span_gouraud_rgba.h>
#include <agg/agg_span_interpolator_linear.h>
#include <agg/agg_gradient_lut.h>

#include <agg/agg_conv_contour.h>
#include <agg/agg_conv_marker.h>
#include <agg/agg_conv_stroke.h>

#include <agg/agg_vcgen_markers_term.h>

/*
#include <agg/agg_renderer_base.h>
#include <agg/agg_renderer_markers.h>
#include <agg/agg_renderer_mclip.h>
#include <agg/agg_renderer_outline_aa.h>
#include <agg/agg_renderer_outline_image.h>
#include <agg/agg_renderer_primitives.h>
*/

//#include "rasterizer.h"
/*
#include <agg/agg_rasterizer_cells_aa.h>
#include <agg/agg_rasterizer_compound_aa.h>
#include <agg/agg_rasterizer_outline.h>
#include <agg/agg_rasterizer_outline_aa.h>

#include <agg/agg_rasterizer_sl_clip.h>*/

/*
#include "types.h"
#include "cell.h"
#include "outline.h"
#include "scanline.h"
#include "span_rgba32.h"
#include "span_bgra32.h"
#include "span_argb32.h"
#include "span_abgr32.h"
#include "span_rgb24.h"
#include "span_bgr24.h"
#include "span_rgb565.h"
#include "span_rgb555.h"
#include "span_rgb5551.h"
#include "span_abgr1555.h"
#include "span_mono8.h"

inline agg::rgba8
rgb8(const u8 r, const u8 g, const u8 b)
{
	return agg::rgba8(b, g, r);
}
inline
agg::rgba8 rgb5(const u8 r, const u8 g, const u8 b)
{
	return agg::rgba8((b & 31) << 3, (g & 31) << 3, (r & 31) << 3);
}
inline
agg::rgba8 argb16(const bool a, const u8 r, const u8 g, const u8 b)
{
	return agg::rgba8((b & 31) << 3, (g & 31) << 3, (r & 31) << 3, -a);
}*/

#endif

