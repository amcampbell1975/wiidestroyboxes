// errors are fix in docker
#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>

// Font
#include "BMfont5_png.h"
//Image

#include "box_png.h"
#include "dark_png.h"

#include "../box2d/box2d/box2d.h"
#include "../source/wiidestroyboxes.h"

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

extern int frame;
extern int boxes;
extern float box_size[MAX_BOXES];
extern b2BodyId boxID[MAX_BOXES];
extern b2BodyId groundId;

void setup_box2d(void);
void box2d_next_frame(void);
void clean_up_box2d(void);

GRRLIB_texImg *tex_BMfont5;
GRRLIB_texImg *tex_crate;
GRRLIB_texImg *tex_dark;


void draw(float x, float y, GRRLIB_texImg *img, b2Rot rot, float size_x, float size_y) {
    GRRLIB_DrawImg((x * 25) + 320, (y * - 25) + 264, img, b2Rot_GetAngle(rot) / B2_PI * - 180, size_x * 0.25, size_y * 0.25, 0xffffffff);
}


void draw_boxes_and_floor(void) {
    // draw boxes
    for (int i=0; i<boxes; i++) {
        b2Vec2 pos = b2Body_GetPosition(boxID[i]);
        b2Rot rot = b2Body_GetRotation(boxID[i]);
		draw(pos.x, pos.y, tex_crate, rot, box_size[i], box_size[i]);
    }
    // draw floor
    b2Vec2 pos = b2Body_GetPosition(groundId);
    b2Rot rot = b2Body_GetRotation(groundId);
    draw(pos.x, pos.y, tex_crate, rot, 10, 5);
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

    // Load crate image
    tex_crate = GRRLIB_LoadTexture(box_png);
    tex_dark = GRRLIB_LoadTexture(dark_png);

    // Move handle to center of crate. This is so it rotates around the centre.
    GRRLIB_SetMidHandle(tex_crate, true);

    // for the wiimote data
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	
	setup_box2d();
	while(true) {
		box2d_next_frame();

        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;

        GRRLIB_FillScreen(GRRLIB_PURPLE);
		draw_boxes_and_floor();
        
        for (int i=0; i<1; i++) {
            WPADData* data = WPAD_Data(i);
            if(data->data_present) {
                GRRLIB_DrawImg(data->ir.x - 1000, data->ir.y - 1000, tex_dark, 1, 10, 10, 0xffffffff);
                // GRRLIB_Printf(100, 0 + i * 20, tex_BMfont5, GRRLIB_WHITE, 1, "wiimote %d: x -> %f y-> %f angle -> %f", i, data->ir.x, data->ir.y, data->ir.angle);
            }
        }
        GRRLIB_Printf(0, 0, tex_BMfont5, GRRLIB_WHITE, 1, "Time %d", 20 - (frame / 60));
        GRRLIB_Render();

        if (20 - (frame / 60) <= 0) break;
    }

	// clean up
	clean_up_box2d();
    GRRLIB_FreeTexture(tex_crate);
    GRRLIB_FreeTexture(tex_BMfont5);

    GRRLIB_Exit();
    return 0;
}
