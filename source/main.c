// errors are fix in docker
#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdlib.h>

// Font
#include "BMfont5_png.h"

//Image
#include "box_png.h"
#include "gold_box_png.h"
#include "tele_box_png.h"

#include "dark_png.h"
#include "thing_1_png.h"
#include "black_png.h"
#include "hole_png.h"

#include "../box2d/box2d/box2d.h"
#include "../source/wiidestroyboxes.h"

// RGB Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_WHITE   0xFFFFFFFF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_GREEN   0x408000FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_PURPLE  0x400040FF

extern int frame;
extern int boxes;
extern int box_hp[MAX_BOXES];
extern BoxType_T box_img[MAX_BOXES];
extern float box_size[MAX_BOXES];
extern b2BodyId boxID[MAX_BOXES];
extern b2BodyId groundId;

void setup_box2d(void);
void box2d_next_frame(void);
void respawn_box(int boxID_to_move);
void clean_up_box2d(void);

GRRLIB_texImg *tex_BMfont5;
GRRLIB_texImg *tex_box;
GRRLIB_texImg *tex_gold_box;
GRRLIB_texImg *tex_tele_box;

GRRLIB_texImg *tex_dark;
GRRLIB_texImg *tex_thing_1;
GRRLIB_texImg *tex_black;
GRRLIB_texImg *tex_hole;


bool isPointTouchingBox(float pointX, float pointY, float boxX, float boxY, float boxsize) {
    float offsetX = abs(boxX * 25 + 320 - pointX);
    float offsetY = abs(boxY * - 25 + 264  - pointY);
    
    if (offsetY < boxsize * 25 && offsetX < boxsize * 25) {
        return true;
    }
    return false;
}


void draw(float x, float y, GRRLIB_texImg *img, b2Rot rot, float size_x, float size_y) {
    GRRLIB_DrawImg((x * 25) + 320, (y * - 25) + 264, img, b2Rot_GetAngle(rot) / B2_PI * - 180, size_x * 0.25, size_y * 0.25, 0xffffffff);
}


void draw_boxes_and_floor(void) {
    // draw boxes
    for (int i=0; i<boxes; i++) {
        if (box_hp[i] > 0) {
            b2Vec2 pos = b2Body_GetPosition(boxID[i]);
            b2Rot rot = b2Body_GetRotation(boxID[i]);

            if (box_img[i] == BOX) {
                draw(pos.x, pos.y, tex_box, rot, box_size[i], box_size[i]);
            }
            else if (box_img[i] == GOLD_BOX) {
                draw(pos.x, pos.y, tex_gold_box, rot, box_size[i], box_size[i]);
            }
            else if (box_img[i] == TELE_BOX) {
                draw(pos.x, pos.y, tex_tele_box, rot, box_size[i], box_size[i]);
            }
        }
    }
    // draw floor
    b2Vec2 pos = b2Body_GetPosition(groundId);
    b2Rot rot = b2Body_GetRotation(groundId);
    draw(pos.x, pos.y, tex_thing_1, rot, 10, 10);
}


int main(int argc, char **argv) {
    GRRLIB_Init();
    WPAD_Init();

    // Load Font image
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);

    // Convert to individual letters.
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0); 

    // Load box image
    tex_box = GRRLIB_LoadTexture(box_png);
    GRRLIB_SetMidHandle(tex_box, true);

    tex_gold_box = GRRLIB_LoadTexture(gold_box_png);
    GRRLIB_SetMidHandle(tex_gold_box, true);
    
    tex_tele_box = GRRLIB_LoadTexture(tele_box_png);
    GRRLIB_SetMidHandle(tex_tele_box, true);

    tex_dark = GRRLIB_LoadTexture(dark_png);
    GRRLIB_SetMidHandle(tex_dark, true);

    tex_thing_1 = GRRLIB_LoadTexture(thing_1_png);
    GRRLIB_SetMidHandle(tex_thing_1, true);

    tex_black = GRRLIB_LoadTexture(black_png);
    GRRLIB_SetMidHandle(tex_black, true);

    tex_hole = GRRLIB_LoadTexture(hole_png);
    GRRLIB_SetMidHandle(tex_hole, true);

    // for the wiimote data
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	
	setup_box2d();
	while(true) {
        box2d_next_frame();

        GRRLIB_FillScreen(GRRLIB_PURPLE);
        draw_boxes_and_floor();

        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
        
        for (int wiimote=0; wiimote<4; wiimote++) {
            WPADData* data = WPAD_Data(wiimote);
            
            if(data->data_present) {
                GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_dark, 1, 10, 10, 0xffffffff);
                // GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_hole, 1, 1, 1, 0xffffffff);
                
                // GRRLIB_Printf(295, 5 + wiimote * 20, tex_BMfont5, GRRLIB_WHITE, 1, "wiimote %d: %0.1fX %f0.1Y %fA", wiimote, data->ir.x, data->ir.y, data->ir.angle);
                
                if (data->btns_d & WPAD_BUTTON_A) {
                    for (int i=0; i<boxes; i++) {
                        if (isPointTouchingBox(data->ir.x, data->ir.y, b2Body_GetPosition(boxID[i]).x, b2Body_GetPosition(boxID[i]).y, box_size[i])) {
                            box_hp[i] -= 1;
                            
                            if (box_img[i] == TELE_BOX) {
                                respawn_box(i);
                            }
                            
                            if (box_hp[i] <= 0) {
                                b2Body_SetTransform(boxID[i], (b2Vec2){-1000, -1000}, b2MakeRot(0));
                            }
                        }
                    }
                }
            }
        }

        GRRLIB_Printf(5, 5, tex_BMfont5, GRRLIB_WHITE, 1, "Time %0.1f", 20.0 - (frame / 60.0));
        
        GRRLIB_Render();
        if (20.0 - (frame / 60.0) <= 0.0) break;
    }
    
	// clean up
	clean_up_box2d();
    GRRLIB_FreeTexture(tex_box);
    GRRLIB_FreeTexture(tex_gold_box);
    GRRLIB_FreeTexture(tex_tele_box);
    GRRLIB_FreeTexture(tex_dark);
    GRRLIB_FreeTexture(tex_thing_1);
    GRRLIB_FreeTexture(tex_black);
    GRRLIB_FreeTexture(tex_hole);
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_Exit();
    return 0;
}
