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
#include "dark_png.h"
#include "thing_1_png.h"

#include "../box2d/box2d/box2d.h"
#include "../source/wiidestroyboxes.h"

// RGBA Colors
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
extern char box_imgs[MAX_BOXES];
extern float box_size[MAX_BOXES];
extern b2BodyId boxID[MAX_BOXES];
extern b2BodyId groundId;

void setup_box2d(void);
void box2d_next_frame(void);
void clean_up_box2d(void);

GRRLIB_texImg *tex_BMfont5;
GRRLIB_texImg *tex_box;
GRRLIB_texImg *tex_gold_box;
GRRLIB_texImg *tex_dark;
GRRLIB_texImg *tex_thing_1;


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
        b2Vec2 pos = b2Body_GetPosition(boxID[i]);
        b2Rot rot = b2Body_GetRotation(boxID[i]);
        if (box_imgs[i] == 'b') {
            draw(pos.x, pos.y, tex_box, rot, box_size[i], box_size[i]);
        }
        else if (box_imgs[i] == 'g') {
            draw(pos.x, pos.y, tex_gold_box, rot, box_size[i], box_size[i]);
        }
    }
    // draw floor
    b2Vec2 pos = b2Body_GetPosition(groundId);
    b2Rot rot = b2Body_GetRotation(groundId);
    draw(pos.x, pos.y, tex_thing_1, rot, 10, 10);
}


int main(int argc, char **argv) {
    // Initialise GRRLIB
    GRRLIB_Init();

    // Initialise Wiimotes
    WPAD_Init();

    // Load Font image
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);

    // Convert to individual letters.
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0); 

    // Load box image
    tex_box = GRRLIB_LoadTexture(box_png);
    tex_gold_box = GRRLIB_LoadTexture(gold_box_png);
    tex_dark = GRRLIB_LoadTexture(dark_png);
    tex_thing_1 = GRRLIB_LoadTexture(thing_1_png);

    // Move handle to center of box. This is so it rotates around the centre.
    GRRLIB_SetMidHandle(tex_box, true);
    GRRLIB_SetMidHandle(tex_gold_box, true);
    GRRLIB_SetMidHandle(tex_dark, true);
    GRRLIB_SetMidHandle(tex_thing_1, true);

    // for the wiimote data
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	
	setup_box2d();
	while(true) {
		box2d_next_frame();
        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;

        GRRLIB_FillScreen(GRRLIB_PURPLE);
		draw_boxes_and_floor();

        for (int wiimote=0; wiimote<1; wiimote++) {
            WPADData* data = WPAD_Data(wiimote);

            if(data->data_present) {
                GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_dark, 1, 10, 10, 0xffffffff);
                GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_box, 1, 0.1, 0.1, 0xffffffff);

                GRRLIB_Printf(295, 5 + wiimote * 20, tex_BMfont5, GRRLIB_WHITE, 1, "wiimote %d: %0.1fX %f0.1Y %fA", wiimote, data->ir.x, data->ir.y, data->ir.angle);
                
                for (int i=0; i<boxes; i++) {
                    
                    if (isPointTouchingBox(data->ir.x, data->ir.y, b2Body_GetPosition(boxID[i]).x, b2Body_GetPosition(boxID[i]).y, box_size[i])) {
                        b2Body_SetTransform(boxID[i], (b2Vec2){-1000, -1000}, b2MakeRot(0));
                    }
                }
            }
        }

        GRRLIB_Printf(5, 5, tex_BMfont5, GRRLIB_WHITE, 1, "Time %0.1f", 20.0 - (frame / 60.0));
        if (frame % 15 == 0) {
            GRRLIB_Printf(5, 30, tex_BMfont5, GRRLIB_WHITE, 1, "Frame %d", frame);
        }
        GRRLIB_Render();

        if (20.0 - (frame / 60.0) <= 0.0) break;
    }

	// clean up
	clean_up_box2d();
    GRRLIB_FreeTexture(tex_box);
    GRRLIB_FreeTexture(tex_dark);
    GRRLIB_FreeTexture(tex_thing_1);
    GRRLIB_FreeTexture(tex_BMfont5);

    GRRLIB_Exit();
    return 0;
}
