#include "../types.h"

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB)
{
	return
		(max(0, min(rectA->x + rectA->width, rectB->x + rectB->width) - max(rectA->x, rectB->x)))
		*max(0, min(rectA->y + rectA->height, rectB->y + rectB->height) - max(rectA->y, rectB->y));
}

ccRect ccRectConcatenate(int amount, ccRect* rectArray)
{
	ccRect rect;
	int i;

	for(i = 0; i < amount; i++) {
		if(i == 0) rect = rectArray[i];
		else {
			if(rectArray[i].x < rect.x) rect.x = rectArray[i].x;
			if(rectArray[i].y < rect.y) rect.y = rectArray[i].y;
			if(rectArray[i].x + rectArray[i].width > rect.x + rect.width) rect.width += rectArray[i].x + rectArray[i].width - rect.x - rect.width;
			if(rectArray[i].y + rectArray[i].height > rect.y + rect.height) rect.height += rectArray[i].y + rectArray[i].height - rect.y - rect.height;
		}
	}

	return rect;
}