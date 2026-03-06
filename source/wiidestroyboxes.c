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
    // make world
    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0, 0.0};
    worldId = b2CreateWorld(&worldDef);

    // make ground
    groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.gravityScale = 0;
    groundId = b2CreateBody(worldId, &groundBodyDef);
    groundBox = b2MakeBox(8.0, 8.0);
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
    box_density[boxes] = pow(density, 2);
    box_gravity[boxes] = gravity;
    box_size[boxes] = size *0.75;
    shapeDef.density = density;
    box = b2MakeBox(size *0.75, size *0.75);
    boxID[boxes] = b2CreateBody(worldId, &boxDef);
    b2CreatePolygonShape(boxID[boxes], &shapeDef, &box);
    boxes++;
}


void box2d_next_frame(void) {
    b2World_Step(worldId, timeStep, subStep);
    
    // make boxs
    if (boxes < MAX_BOXES && frame % 9 == 0) {
        boxDef.position = (b2Vec2){rand() % 16 - 8, 16};

        // for the "boxes" density is X ^ 2
        // for the "gold boxes" density is (X * 1.5) ^ 2
        // for the "boss boxes" density is a big number.

        if (rand() % 3 == 0) {
            if (rand() % 9 == 0) {
                // boss gold box
                make_box(1.0, 32.0, -150.0 , 2.0);
            } 
            else if (rand() % 5 == 0) {
                // big gold box
                make_box(0.35, 5.0, -100.0, 1.5);
            }            
            else if (rand() % 5 == 0) {
                // small gold box
                make_box(0.35, 0.55, -100.0, 0.5);
            }
            else {
                // gold box
                make_box(0.35, 2.25, -100.0, 1.0);
            }
        }
        else {
            if (rand() % 9 == 0) {
                // boss box
                make_box(1.0, 8.0, -75.0, 2.5);
            } 
            else if (rand() % 5 == 0) {
                // big box
                make_box(0.5, 2.25, -100.0, 1.5);
            }            
            else if (rand() % 5 == 0) {
                // small box
                make_box(0.5, 0.25, -100.0, 0.5);
            }
            else {
                // box
                make_box(0.5, 1.0, -100.0, 1.0);
            }
        }
    }

    // gravity for the boxes
    for (int i=0; i<boxes; i++) {
        b2Vec2 apply_force = {0, box_density[i] * box_gravity[i]};
        printf("%f\n",box_density[i]);
        b2Body_ApplyForceToCenter(boxID[i], apply_force, true);
    }

    // move ground
    b2Body_SetTransform(groundId, (b2Vec2){0.0, - 10}, b2MakeRot(sin(frame / 60.0) / 4));
    frame++;
}


void clean_up_box2d(void) {
    b2DestroyWorld(worldId);
}