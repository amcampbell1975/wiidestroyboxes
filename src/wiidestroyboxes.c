#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <stdlib.h>

#include "draw.h"
#include "box2d/box2d.h"

Display *display;
Window window;
int screen;
GC gc;


// for x11
int scale_x(float in) {
    return (in * 50) +540;
}


// for x11
int scale_y(float in) {
    return (in * -50) +270;
}


int main(void) {
    // x11 setup
    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow( display,RootWindow(display, screen),0, 0,1080, 540,1,BlackPixel(display, screen),WhitePixel(display, screen));
    XMapWindow(display, window);
    gc = XCreateGC(display, window, 0, NULL);

    // box2d make world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0, -10.0};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    // make ground
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    // groundBodyDef.type = b2_dynamicBody;
    groundBodyDef.rotation = b2MakeRot(0.5);
    groundBodyDef.gravityScale = 0;
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(10.0, 1.0);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // make pivot
    // b2BodyDef pivotBodyDef = b2DefaultBodyDef();
    // pivotBodyDef.position = (b2Vec2){0.0, 0.0};
    // b2BodyId pivotId = b2CreateBody(worldId, &pivotBodyDef);
    // b2Polygon pivotBox = b2MakeBox(0.1, 0.1);
    // b2ShapeDef pivotShapeDef = b2DefaultShapeDef();
    // b2CreatePolygonShape(pivotId, &pivotShapeDef, &pivotBox);

    // b2RevoluteJointDef groundJointDef = b2DefaultRevoluteJointDef();
    // b2JointId groundJointId = b2CreateRevoluteJoint(worldId, &groundJointDef);
    // b2Joint_GetBodyA(groundJointId);
    
    // b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    // jointDef.base.bodyIdA = groundId;
    // jointDef.base.bodyIdB = pivotId;
    
    // b2JointId test = b2CreateRevoluteJoint(worldId, &jointDef);

    // make boxes
    int boxes = 0;
    float box_size[1000];
    b2BodyId bodyId[1000];
    b2BodyDef bodyDef = b2DefaultBodyDef();
    b2Polygon box = b2MakeBox(0.5, 0.5);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    bodyDef.type = b2_dynamicBody;

    // set simulatin
    const float timeStep = 1.0 / 60.0;
    const int subStep = 4;
    while (true) {
        b2World_Step(worldId, timeStep, subStep);
        XClearWindow(display, window);
        b2Vec2 vel = { 5.0f, 0.0f };
        b2Body_SetTransform(groundId, vel, b2MakeRot(boxes /100.0));
        
        if (boxes <1000) {
            bodyDef.position = (b2Vec2){rand()%16 -8, rand()%32 +1};
            shapeDef.density = 1.0;
            shapeDef.material.friction = 0.35;
            box_size[boxes] = ((rand() %3) +2) /10.0;
            box = b2MakeBox(box_size[boxes], box_size[boxes]);
            bodyId[boxes] = b2CreateBody(worldId, &bodyDef);
            b2CreatePolygonShape(bodyId[boxes], &shapeDef, &box);
            boxes++;
        }
        // draw boxes
        for (int i=0;i<boxes;i++) {
            b2Vec2 pos = b2Body_GetPosition(bodyId[i]);
            b2Rot rot = b2Body_GetRotation(bodyId[i]);
            draw_rotated_rect(display, window, gc, scale_x(pos.x), scale_y(pos.y), box_size[i] *100, box_size[i] *100, -b2Rot_GetAngle(rot));
        }
        // b2Vec2 pos = b2Body_GetPosition(pivotId);
        // draw_rotated_rect(display, window, gc, scale_x(pos.x), scale_y(pos.y), 10, 10, 0);
        b2Vec2 pos = b2Body_GetPosition(groundId);
        b2Rot rot = b2Body_GetRotation(groundId);
        draw_rotated_rect(display, window, gc, scale_x(pos.x), scale_y(pos.y), 1000, 100, -b2Rot_GetAngle(rot));

        XFlush(display);
        usleep((1/60.0)/ 0.000001);
    }
    // clean up x11 and box2d
    b2DestroyWorld(worldId);
    XCloseDisplay(display);
}
