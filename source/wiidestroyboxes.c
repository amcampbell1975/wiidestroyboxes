#include <stdio.h>
#include <stdlib.h>

#include "../box2d/box2d/box2d.h"
#include "wiidestroyboxes.h"

int box_gravity[MAX_BOXES];
int boxes = 0;
float box_size[MAX_BOXES];
float box_density[MAX_BOXES];

// time setup
const float timeStep = 1.0 / 60.0;
const int subStep = 4;
int frame = 0;

b2WorldDef worldDef;
b2WorldId worldId;
b2BodyDef groundBodyDef;
b2BodyId groundId;
b2Polygon groundBox;
b2ShapeDef groundShapeDef;
b2BodyDef boxDef;
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
    groundBox = b2MakeBox(10.0, 5.0);
    groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // setup boxes
    boxDef = b2DefaultBodyDef();
    box = b2MakeBox(0.5, 0.5);
    shapeDef = b2DefaultShapeDef();
    boxDef.type = b2_dynamicBody;
}


static void make_box(float friction, float density, float gravity, float size) {
    shapeDef.material.friction = friction;
    box_density[boxes] = density;
    box_gravity[boxes] = gravity;
    box_size[boxes] = size;
    shapeDef.density = density;
    box = b2MakeBox(size, size);
    boxID[boxes] = b2CreateBody(worldId, &boxDef);
    b2CreatePolygonShape(boxID[boxes], &shapeDef, &box);
    boxes++;
}


void box2d_next_frame(void) {
    b2World_Step(worldId, timeStep, subStep);
    
    // make boxs
    if (boxes < MAX_BOXES && frame % 9 == 0) {
        boxDef.position = (b2Vec2){rand() % 16 - 8, 16};
        if (rand() % 9 == 0) {
            // big box
            make_box(0.5, 2.25, -56.0, 1.5);
        }            
        else if (rand() % 9 == 0) {
            // teleport box
            make_box(0.5, 0.25, -1.0, 0.5);
        }
        else {
            // box
            make_box(0.5, 1.0, -56.0, 1.0);
        }
    }

    // gravity for the boxes
    for (int i=0; i<boxes; i++) {
        b2Vec2 apply_force = {0, box_density[i] * box_gravity[i]};
        b2Body_ApplyForceToCenter(boxID[i], apply_force, true);
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