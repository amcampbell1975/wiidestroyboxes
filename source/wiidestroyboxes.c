#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../box2d/box2d/box2d.h"
#include "wiidestroyboxes.h"

int box_gravity[MAX_BOXES];
int boxes = 0;
float box_size[MAX_BOXES];
float box_density[MAX_BOXES];

const float timeStep = 1.0 / 60.0;
const int subStep = 4;
int frame = 0;

b2WorldDef worldDef;
b2WorldId worldId;
b2BodyDef groundBodyDef;
b2BodyId groundId;
b2Polygon groundBox;
b2ShapeDef groundShapeDef;
b2BodyDef bodyDef;
b2BodyId boxID[MAX_BOXES];
b2Polygon box;
b2ShapeDef shapeDef;


void setup_box2d(void) {
    // box2d make world
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0, 0.0};
    worldId = b2CreateWorld(&worldDef);

    // make ground
    groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.gravityScale = 0;
    groundId = b2CreateBody(worldId, &groundBodyDef);
    groundBox = b2MakeBox(10.0, 3.0);
    groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // setup boxes
    bodyDef = b2DefaultBodyDef();
    box = b2MakeBox(0.5, 0.5);
    shapeDef = b2DefaultShapeDef();
    bodyDef.type = b2_dynamicBody;
}


void box2d_next_frame(void) {
    b2World_Step(worldId, timeStep, subStep);
    // make box
    if (boxes <MAX_BOXES && frame % 9 == 0) {
        bodyDef.position = (b2Vec2){rand()%16 -8, 16};
        if (rand() % 9 == 0) {
            // gold box
            shapeDef.material.friction = 0.35;
            shapeDef.material.restitution = 0.0;
            box_density[boxes] = 4.0;
            box_gravity[boxes] = - 32.0;
            box_size[boxes] = 1.0;
        }            
        else if (rand() % 9 == 0) {
            // teleporting box
            shapeDef.material.friction = 0.35;
            shapeDef.material.restitution = 0.5;
            box_density[boxes] = 0.25;
            box_gravity[boxes] = - 1.0;
            box_size[boxes] = 0.25;
        }
        else {
            // box
            shapeDef.material.friction = 0.35;
            shapeDef.material.restitution = 0.0;
            box_density[boxes] = 1.0;
            box_gravity[boxes] = - 32.0;
            box_size[boxes] = 0.5;
        }
        shapeDef.density = box_density[boxes];
        box = b2MakeBox(box_size[boxes], box_size[boxes]);
        boxID[boxes] = b2CreateBody(worldId, &bodyDef);
        b2CreatePolygonShape(boxID[boxes], &shapeDef, &box);
        boxes++;
    }
    // gravity for the boxes
    for (int i=0;i<boxes;i++) {
        b2Vec2 use_the_force = {0,box_gravity[i] *box_density[i]};
        b2Body_ApplyForceToCenter(boxID[i], use_the_force, true);
    }
    // move ground
    b2Vec2 setpos = {0.0, - 5};
    b2Body_SetTransform(groundId, setpos, b2MakeRot(sin(frame / 60.0) / 4));

    frame++;
}


void clean_up_box2d(void) {
    b2DestroyWorld(worldId);
}


void debug_box2d(void) {
    b2Vec2 pos = b2Body_GetPosition(boxID[0]);
    b2Rot rot = b2Body_GetRotation(boxID[0]);
    printf("%4.2f %4.2f %4.2f\n", pos.x, pos.y, b2Rot_GetAngle(rot));
}