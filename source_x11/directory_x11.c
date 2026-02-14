#include <unistd.h>
#include <X11/Xlib.h>

#include "draw.h"
#include "../box2d/box2d/box2d.h"
#include "../source/wiidestroyboxes.h"

extern int frame;
extern int boxes;
extern float box_size[MAX_BOXES];
extern b2BodyId boxID[MAX_BOXES];
extern b2BodyId groundId;

void setup_box2d(void);
void box2d_next_frame(void);
void debug_box2d(void);
void clean_up_box2d(void);

Display *display;
Window window;
int screen;
GC gc;


int scale_x(float in) {
    return (in * 50) +540;
}


int scale_y(float in) {
    return (in * -50) +270;
}


void draw_x11(void) {
    XClearWindow(display, window);
    for (int i=0; i<boxes; i++) {
        b2Vec2 pos = b2Body_GetPosition(boxID[i]);
        b2Rot rot = b2Body_GetRotation(boxID[i]);
        draw_rotated_rect(display, window, gc, scale_x(pos.x), scale_y(pos.y), box_size[i] *100, box_size[i] *100, -b2Rot_GetAngle(rot));
    }
    b2Vec2 pos = b2Body_GetPosition(groundId);
    b2Rot rot = b2Body_GetRotation(groundId);
    draw_rotated_rect(display, window, gc, scale_x(pos.x), scale_y(pos.y), 1000, 300, - b2Rot_GetAngle(rot));
    XFlush(display);
}


int main(void) {
    // x11 setup
    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow( display,RootWindow(display, screen), 0, 0, 1080, 540, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, NULL);

    setup_box2d();

    while(true) {
		box2d_next_frame();
        draw_x11();
        usleep((1 / 60.0) / 0.000001);
    }

    // clean up
    clean_up_box2d();
    XCloseDisplay(display); 
}