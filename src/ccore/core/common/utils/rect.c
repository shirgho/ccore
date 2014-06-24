#include "../types.h"

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB)
{
	return
		(max(0, min(rectA->x + rectA->width, rectB->x + rectB->width) - max(rectA->x, rectB->x)))
		*max(0, min(rectA->y + rectA->height, rectB->y + rectB->height) - max(rectA->y, rectB->y));
}

ccRect ccRectConcatenate(int amount, ...)
{
	ccRect rect;
	ccRect r;
	va_list rects;
	int i;

	va_start(rects, amount);
	for(i = 0; i < amount; i++) {
		r = va_arg(rects, ccRect);
		if(i == 0) rect = r;
		else {
			if(r.x < rect.x) rect.x = r.x;
			if(r.y < rect.y) rect.y = r.y;
			if(r.x + r.width > rect.x + rect.width) rect.width += r.x + r.width - rect.x - rect.width;
			if(r.y + r.height > rect.y + rect.height) rect.height += r.y + r.height - rect.y - rect.height;
		}
	}
	va_end(rects);

	return rect;
}