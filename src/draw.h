#include <X11/Xlib.h>

void draw_rotated_rect(Display *d, Window w, GC gc,
                       int x, int y, int width, int height,
                       double angle_deg);

void rotate_point(double cx, double cy, double angle,
    double *x, double *y);
                    
