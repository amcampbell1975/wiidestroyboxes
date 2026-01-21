#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"

typedef struct {
    float x, y;
} Point;


void rotate_point(Point *p, float cx, float cy, float angle) {
    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin
    p->x -= cx;
    p->y -= cy;

    // rotate
    float xnew = p->x * c - p->y * s;
    float ynew = p->x * s + p->y * c;

    // translate back
    p->x = xnew + cx;
    p->y = ynew + cy;
}


void draw_rotated_rect(Display *d, Window w, GC gc,
                       int x, int y, int width, int height,
                       float angle)
{
    Point pts[4] = {
        {x - width/2, y - height/2},
        {x + width/2, y - height/2},
        {x + width/2, y + height/2},
        {x - width/2, y + height/2}
    };

    for (int i = 0; i < 4; i++) {
        rotate_point(&pts[i], x, y, angle);
    }

    XPoint xpts[5];
    for (int i = 0; i < 4; i++) {
        xpts[i].x = (short)pts[i].x;
        xpts[i].y = (short)pts[i].y;
    }
    xpts[4] = xpts[0]; // close polygon
    XDrawLines(d, w, gc, xpts, 5, CoordModeOrigin);
}
