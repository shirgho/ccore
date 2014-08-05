#include <ccore/types.h>

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

	xmin = rectArray[0].x;
	xmax = rectArray[0].x + rectArray[0].width;
	ymin = rectArray[0].y;
	ymax = rectArray[0].y + rectArray[0].height;

	for(i = 1; i < amount; i++) {
		if(rectArray[i].x < xmin) xmin = rectArray[i].x;
		if(rectArray[i].y < ymin) ymin = rectArray[i].y;
		if(rectArray[i].x + rectArray[i].width > xmax) xmax = rectArray[i].x + rectArray[i].width;
		if(rectArray[i].y + rectArray[i].height > ymax) ymax = rectArray[i].y + rectArray[i].height;
	}

	return (ccRect){ xmin, ymin, xmax - xmin, ymax - ymin };
}