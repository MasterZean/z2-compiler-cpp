#include "ItemDisplay.hpp"

#define IMAGECLASS ZImg
#define IMAGEFILE <zide/zide.iml>
#include <Draw/iml_source.h>

int ItemDisplay::DoPaint(Draw& w, const Rect& r, const Value& q,
							Color ink, Color paper, dword style) const {
	if (!q.Is<ZItem>())
		return 0;
	
	const ZItem& m = ValueTo<ZItem>(q);
	
	int x = r.left + 3;
	
	Font font = StdFont();
	Font bold = StdFont().Bold();

	w.DrawRect(r, paper);
	int top = r.top;
	
	if (m.Kind == 1) {
		Image img = ZImg::cls;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	if (m.Kind == 2) {
		Image img = ZImg::enm;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	if (m.Kind == 3) {
		Image img = ZImg::func;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 4) {
		Image img = ZImg::statfunc;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 5) {
		Image img = ZImg::constructor;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 6) {
		Image img = ZImg::get;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 7) {
		Image img = ZImg::nconstructor;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 8) {
		Image img = ZImg::set;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 9) {
		Image img = ZImg::getset;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 10) {
		Image img = ZImg::cst;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	else if (m.Kind == 11) {
		Image img = ZImg::var;
		w.DrawImage(x, top, img);
		x += img.GetWidth() + 2;
	}
	
	if (m.Kind == 6) {
		int i = m.Name.Find(':');
		
		if (i != -1) {
			String first = m.Name.Mid(0, i);
			w.DrawText(x, top, first, font, ink);
			x += GetTextSize(first, font).cx;
			x++;
			w.DrawText(x, top, ":", bold, LtBlue);
			x += GetTextSize(":", bold).cx;
			x++;
			
			String rest = m.Name.Mid(i + 1);
			w.DrawText(x, top, rest, font, ink);
			x += GetTextSize(rest, font).cx;
		}
		else
			w.DrawText(x, top, m.Name, font, ink);
		
		x += 3;
	
		return x;
	}
	
	int i = m.Name.Find('{');
	
	if (i != -1) {
		String first = m.Name.Mid(0, i);
		w.DrawText(x, top, first, font, ink);
		x += GetTextSize(first, font).cx;
		x++;
		w.DrawText(x, top, "{", bold, LtBlue);
		x += GetTextSize("{", bold).cx;
		x++;
		
		String rest = m.Name.Mid(i + 1);
		i = rest.Find('}');
		if (i != -1) {
			String second = rest.Mid(0, i);
			int j = second.Find(':');
			if (j == -1) {
				w.DrawText(x, top, second, font, ink);
				x += GetTextSize(second, font).cx;
			}
			else {
				String pn = second.Mid(0, j);
				String pt = second.Mid(j + 1);
				w.DrawText(x, top, pn, font, ink);
				x += GetTextSize(pn, font).cx;
				w.DrawText(x, top, ":", bold, LtBlue);
				x += GetTextSize(":", bold).cx;
				w.DrawText(x, top, pt, font, ink);
				x += GetTextSize(pt, font).cx;
			}
		
			x++;
			w.DrawText(x, top, "}", bold, LtBlue);
			x += GetTextSize("}", bold).cx;
		}
		else {
			w.DrawText(x, top, rest, font, ink);
			x += GetTextSize(m.Name, font).cx;
		}
	}
	else {
		i = m.Name.Find('(');
		
		if (i != -1) {
			String first = m.Name.Mid(0, i);
			w.DrawText(x, top, first, font, ink);
			x += GetTextSize(first, font).cx;
			x++;
			w.DrawText(x, top, "(", bold, LtBlue);
			x += GetTextSize("(", bold).cx;
			x++;
			
			String rest = m.Name.Mid(i + 1);
			i = rest.Find(')');
			if (i != -1) {
				String second = rest.Mid(0, i);
				int j = second.Find(':');
				if (j == -1) {
					w.DrawText(x, top, second, font, ink);
					x += GetTextSize(second, font).cx;
				}
				else {
					String pn = second.Mid(0, j);
					String pt = second.Mid(j + 1);
					w.DrawText(x, top, pn, font, ink);
					x += GetTextSize(pn, font).cx;
					w.DrawText(x, top, ":", bold, LtBlue);
					x += GetTextSize(":", bold).cx;
					w.DrawText(x, top, pt, font, ink);
					x += GetTextSize(pt, font).cx;
				}
				
				x++;
				w.DrawText(x, top, ")", bold, LtBlue);
				x += GetTextSize(")", bold).cx;
				
				i = rest.Find(':', i);
				if (i != -1) {
					w.DrawText(x, top, ":", bold, LtBlue);
					x += GetTextSize(":", bold).cx;
					rest = rest.Mid(i + 1);
					w.DrawText(x, top, rest, font, ink);
					x += GetTextSize(rest, font).cx;
				}
			}
			else {
				w.DrawText(x, top, rest, font, ink);
				x += GetTextSize(rest, font).cx;
			}
		}
		else {
			w.DrawText(x, top, m.Name, font, ink);
			x += GetTextSize(m.Name, font).cx;
		}
	}

	x += 3;
	
	return x;
}
