#ifndef __ITEM_DISPALY_HPP__
#define __ITEM_DISPALY_HPP__

#include <Core/Core.h>
#include <CtrlLib/CtrlLib.h>
#include <z2clib/Scanner.h>

#define IMAGECLASS ZImg
#define IMAGEFILE <zide/zide.iml>
#include <Draw/iml_header.h>
#include <z2clib/ZItem.h>

using namespace Upp;


class ItemDisplay: public Display {
public:
	int DoPaint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const;
	
	virtual Size GetStdSize(const Value& q) const {
		NilDraw w;
		return Size(DoPaint(w, Rect(0, 0, INT_MAX, INT_MAX), q, Null, Null, 0), StdFont().GetCy());
	}
	
	virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const {
		DoPaint(w, r, q, ink, paper, style);
	}
};

#endif