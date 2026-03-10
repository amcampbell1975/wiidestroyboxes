#include <stdio.h>
#include <stdlib.h>

#include "../box2d/box2d/box2d.h"
#include "wiidestroyboxes.h"

int boxes = 0;
int box_gravity[MAX_BOXES];
int box_hp[MAX_BOXES];
BoxType_T box_imgs[MAX_BOXES];
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
    groundBox = b2MakeBox(10.0, 10.0);
    groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    // setup boxes
    boxDef = b2DefaultBodyDef();
    box = b2MakeBox(0.5, 0.5);
    shapeDef = b2DefaultShapeDef();
    boxDef.type = b2_dynamicBody;
}


static void make_box(float friction, float density, float gravity, float size, BoxType_T img, int hp) {
    shapeDef.material.friction = friction;
    box_gravity[boxes] = gravity;
    box_hp[boxes] = hp;

    shapeDef.density = density;
    box_density[boxes] = pow(density, 2);

    box_size[boxes] = size * 0.8;
    box = b2MakeBox(size * 0.8, size * 0.8);

    box_imgs[boxes] = img;

    boxID[boxes] = b2CreateBody(worldId, &boxDef);
    b2CreatePolygonShape(boxID[boxes], &shapeDef, &box);
    boxes++;
}


void box2d_next_frame(void) {
    b2World_Step(worldId, timeStep, subStep);
    
    // make boxs
    if (boxes < MAX_BOXES && frame % 10 == 0) {
        boxDef.position = (b2Vec2){rand() % 12 - 6, 12};

        // for the "boss boxes" density is a big number that I like at the time.
        // for the "boxes" density is X ^ 2
        // for the "gold boxes" density is (X * 1.5) ^ 2
        if (frame == 300) {
            // boss box
            make_box(1.0, 8.0, -50.0, 3.5, BOX, 20);
        }
        else if (frame == 900) {
            // boss gold box
            make_box(1.0, 24.0, -150.0 , 2.5, GOLD_BOX, 14);
        }
        else if (rand() % 3 == 0) {
            if (rand() % 4 == 0) {
                // big gold box
                make_box(0.4, 5.0, -100.0, 1.5, GOLD_BOX, 9);
            }     
            else if (rand() % 4 == 0) {
                // small gold box
                make_box(0.4, 0.55, -100.0, 0.5, GOLD_BOX, 6);
            }
            else {
                // gold box
                make_box(0.4, 2.25, -100.0, 1.0, GOLD_BOX, 3);
            }
        }
        else {
            if (rand() % 4 == 0) {
                // big box
                make_box(0.6, 2.25, -100.0, 1.5, BOX, 6);
            }
            else if (rand() % 4 == 0) {
                // small box
                make_box(0.6, 0.25, -100.0, 0.5, BOX, 4);
            }
            else {
                // box
                make_box(0.6, 1.0, -100.0, 1.0, BOX, 2);
            }
        }
    }
    // gravity for the boxes
    for (int i=0; i<boxes; i++) {
        b2Vec2 apply_force = {0, box_density[i] * box_gravity[i]};
        b2Body_ApplyForceToCenter(boxID[i], apply_force, true);
    }
    // move ground
    b2Body_SetTransform(groundId, (b2Vec2){0.0, - 12}, b2MakeRot(sin(frame / 60.0) *0.35));
    frame++;
}


void clean_up_box2d(void) {
    b2DestroyWorld(worldId);
}