#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "draw.h"

void draw_rotated_rect(Display *d, Window w, GC gc,
                       int x, int y, int width, int height,
                       double angle_deg) {
    double angle = angle_deg;

    double x1 = x,         y1 = y;
    double x2 = x+width,   y2 = y;
    double x3 = x+width,   y3 = y+height;
    double x4 = x,         y4 = y+height;

    double cx = x + width/2.0;
    double cy = y + height/2.0;

    rotate_point(cx, cy, angle, &x1, &y1);
    rotate_point(cx, cy, angle, &x2, &y2);
    rotate_point(cx, cy, angle, &x3, &y3);
    rotate_point(cx, cy, angle, &x4, &y4);

    XPoint pts[5] = {
        { (short)x1, (short)y1 },
        { (short)x2, (short)y2 },
        { (short)x3, (short)y3 },
        { (short)x4, (short)y4 },
        { (short)x1, (short)y1 }
    };

    XDrawLines(d, w, gc, pts, 5, CoordModeOrigin);
    // Draw angle text inside the rectangle (not rotated) 
    if (0) {
        char buf[64]; 
        snprintf(buf, sizeof(buf), "%3.1f", angle_deg); 
        // Draw text near the center 
        XDrawString(d, w, gc, (int)cx - 30, (int)cy, buf, strlen(buf));        
    }

}
void rotate_point(double cx, double cy, double angle,
    double *x, double *y) {
double s = sin(angle);
double c = cos(angle);

*x -= cx;
*y -= cy;

double xnew = *x * c - *y * s;
double ynew = *x * s + *y * c;

*x = xnew + cx;
*y = ynew + cy;
}
