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
void debug_box2d(void);
void clean_up_box2d(void);

GRRLIB_texImg *tex_BMfont5;
GRRLIB_texImg *tex_crate;


void setup_video(void) {
    // Initialise GRRLIB
    GRRLIB_Init();

    // Initialise Wiimotes
    WPAD_Init();
}


int scale_x(float in) {
    return (in * 25) + 320;
}


int scale_y(float in) {
    return (in * - 25) + 264;
}


void draw_boxes_and_floor(void) {
    for (int i=0; i<boxes; i++) {
        b2Vec2 pos = b2Body_GetPosition(boxID[i]);
        b2Rot rot = b2Body_GetRotation(boxID[i]);
		GRRLIB_DrawImg(scale_x(pos.x), scale_y(pos.y), tex_crate, b2Rot_GetAngle(rot) / B2_PI * - 180, box_size[i] * 0.25, box_size[i] * 0.25, 0xFFFFFFFF);
    }
    b2Vec2 pos = b2Body_GetPosition(groundId);
    b2Rot rot = b2Body_GetRotation(groundId);
    GRRLIB_DrawImg(scale_x(pos.x), scale_y(pos.y), tex_crate, b2Rot_GetAngle(rot) / B2_PI * - 180, 2.5, 1.25, 0x74347aff);
}


int main(int argc, char **argv) {
	setup_video();
    // Load Font image
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    // Convert to individual letters.
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0); 
    // Load crate image
    tex_crate = GRRLIB_LoadTexture(box_png);
    // Move handle to center of crate. This is so it rotates around the centre.
    GRRLIB_SetMidHandle(tex_crate, true);
	
	setup_box2d();

	while(true) {
		box2d_next_frame();

        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;

        // Clear screen
        GRRLIB_FillScreen(GRRLIB_PURPLE);
		draw_boxes_and_floor();
        GRRLIB_Printf(0, 0, tex_BMfont5, GRRLIB_WHITE, 1, "Time %d", 20 - (frame / 60));
        // GRRLIB_Printf(100, 0, tex_BMfont5, GRRLIB_WHITE, 1, "Debug");
        GRRLIB_Render();
    }

	// clean up
	clean_up_box2d();
    GRRLIB_FreeTexture(tex_crate);
    GRRLIB_FreeTexture(tex_BMfont5);

    GRRLIB_Exit();
    return 0;
}
