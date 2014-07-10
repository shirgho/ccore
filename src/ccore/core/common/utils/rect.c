#include "../types.h"

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB)
{
	return
		(max(0, min(rectA->x + rectA->width, rectB->x + rectB->width) - max(rectA->x, rectB->x)))
		*max(0, min(rectA->y + rectA->height, rectB->y + rectB->height) - max(rectA->y, rectB->y));
}

ccRect ccRectConcatenate(int amount, ccRect* rectArray)
{
	int i;
	int xmin, ymin, xmax, ymax;

	for(i = 0; i < amount; i++) {
		if(i == 0) {
			xmin = rectArray[i].x;
			xmax = rectArray[i].x + rectArray[i].width;
			ymin = rectArray[i].y;
			ymax = rectArray[i].y + rectArray[i].height;
		}
		else {
			if(rectArray[i].x < xmin) xmin = rectArray[i].x;
			if(rectArray[i].y < ymin) ymin = rectArray[i].y;
			if(rectArray[i].x + rectArray[i].width > xmax) xmax = rectArray[i].x + rectArray[i].width;
			if(rectArray[i].y + rectArray[i].height > ymax) ymax = rectArray[i].y + rectArray[i].height;
		}
	}

	return (ccRect){ xmin, ymin, xmax - xmin, ymax - ymin };
}