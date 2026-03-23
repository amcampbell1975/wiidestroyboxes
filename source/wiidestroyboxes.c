#include <stdio.h>
#include <stdlib.h>

#include "../box2d/box2d/box2d.h"
#include "wiidestroyboxes.h"

extern int difficulty;

int boxes = 0;
int box_gravity[MAX_BOXES];
int box_hp[MAX_BOXES];
BoxType_T box_img[MAX_BOXES];
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

    box_img[boxes] = img;

    boxID[boxes] = b2CreateBody(worldId, &boxDef);
    b2CreatePolygonShape(boxID[boxes], &shapeDef, &box);
    boxes++;
}


void box2d_next_frame(void) {
    b2World_Step(worldId, timeStep, subStep);
    
    // make boxs
    if (boxes < MAX_BOXES && frame % 10 == 0) {
        boxDef.position = (b2Vec2){rand() % 12 - 6, 12};

        if (frame == 900) {
            // boss boxes
            if (difficulty == 1) {
                make_box(1.0, 6.0, -20.0, 3.5, BOX, 24);
            }
            else if (difficulty == 2) {
                make_box(1.0, 9.0, -25.0 , 2.5, GOLD_BOX, 10);
            }
            else {
                make_box(1.0, 6.0, -10.0 , 2.5, TELE_BOX, 12);
            }
        }
        else if (rand() % 6 == 0) {
            if (rand() % 4 == 0) {
                // big teleport box
                make_box(0.6, 2.25, -20.0, 1.5, TELE_BOX, 3);
            }     
            else if (rand() % 4 == 0) {
                // small teleport box
                make_box(0.6, 0.25, -20.0, 0.5, TELE_BOX, 2);
            }
            else {
                // teleport box
                make_box(0.6, 1.0, -20.0, 1.0, TELE_BOX, 2);
            }
        }
        else if (rand() % 4 == 0) {
            if (rand() % 4 == 0) {
                // big gold box
                make_box(0.4, 4.0, -80.0, 1.5, GOLD_BOX, 8);
            }     
            else if (rand() % 4 == 0) {
                // small gold box
                make_box(0.4, 0.75, -80.0, 0.5, GOLD_BOX, 5);
            }
            else {
                // gold box
                make_box(0.4, 1.5, -80.0, 1.0, GOLD_BOX, 3);
            }
        }
        else {
            if (rand() % 4 == 0) {
                // big box
                make_box(0.8, 2.25, -80.0, 1.5, BOX, 5);
            }
            else if (rand() % 4 == 0) {
                // small box
                make_box(0.8, 0.25, -80.0, 0.5, BOX, 3);
            }
            else {
                // box
                make_box(0.8, 1.0, -80.0, 1.0, BOX, 2);
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


void respawn_box(int boxID_to_move) {
    b2Body_SetTransform(boxID[boxID_to_move], (b2Vec2){rand() % 12 - 6, 12}, b2MakeRot(0));
}


void clean_up_box2d(void) {
    b2DestroyWorld(worldId);
}