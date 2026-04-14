#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdlib.h>

#include "../box2d/box2d/box2d.h"
#include "../source/game.h"

// Font
#include "BMfont5_png.h"
GRRLIB_texImg *tex_BMfont5;

//Image
#include "box_png.h"
GRRLIB_texImg *tex_box;
#include "gold_box_png.h"
GRRLIB_texImg *tex_gold_box;
#include "tele_box_png.h"
GRRLIB_texImg *tex_tele_box;
#include "tnt_box_png.h"
GRRLIB_texImg *tex_tnt_box;
#include "thing_1_png.h"
GRRLIB_texImg *tex_thing_1;

// Colors
#define GRRLIB_BLACK  0x000000FF
#define GRRLIB_WHITE  0xFFFFFFFF
#define GRRLIB_GRAY   0x808080FF
#define GRRLIB_RED    0xFF0000FF
#define GRRLIB_YELLOW 0xFFFF00FF
#define GRRLIB_GREEN  0x408000FF
#define GRRLIB_BLUE   0x0000FFFF
#define GRRLIB_PURPLE 0x360F44FF

void setup_box2d(void);
void box2d_next_frame(void);
void respawn_box(int boxID_to_move);
void clean_up_box2d(void);
int clamp(int value, int min, int max);
bool isPointTouchingBox(float pointX, float pointY, float boxX, float boxY, float boxsize);
int disToPoint(float pointX, float pointY, float boxX, float boxY);
int disToPoint(float pointX, float pointY, float boxX, float boxY);
void draw(float x, float y, GRRLIB_texImg *img, b2Rot rot, float size_x, float size_y, int color);

extern int frame;
extern int boxes;
extern int box_hp[MAX_BOXES];
extern int box_score[MAX_BOXES];
extern float box_size[MAX_BOXES];
extern BoxType_T box_img[MAX_BOXES];
extern b2BodyId boxID[MAX_BOXES];
extern b2BodyId groundId;

float time_limit = 30.0;
float time_left;
int difficulty = 2;
int score = 0;
bool debug = false;


void draw_box(int box, int light_x, int light_y) {
    b2Vec2 pos = b2Body_GetPosition(boxID[box]);
    b2Rot rot = b2Body_GetRotation(boxID[box]);

    int light_effect = clamp(disToPoint(light_x, light_y, pos.x, pos.y) * 2, 0, 255);
    if (box_img[box] == BOX) {
        draw(pos.x, pos.y, tex_box, rot, box_size[box], box_size[box], 0xFFFFFFFF - light_effect);
    }
    else if (box_img[box] == GOLD_BOX) {
        draw(pos.x, pos.y, tex_gold_box, rot, box_size[box], box_size[box], 0xFFFFFFFF - light_effect);
    }
    else if (box_img[box] == TELE_BOX) {
        draw(pos.x, pos.y, tex_tele_box, rot, box_size[box], box_size[box], 0xFFFFFFFF - light_effect);
    }
    else if (box_img[box] == TNT_BOX) {
        draw(pos.x, pos.y, tex_tnt_box, rot, box_size[box], box_size[box], 0xFFFFFFFF - light_effect);
    }
    
    if (debug) {
        GRRLIB_Printf((pos.x * 25) + 320, (pos.y * - 25) + 264, tex_BMfont5, GRRLIB_WHITE, 1, "%d", light_effect);
    }
}


int main(int argc, char **argv) {
    GRRLIB_Init();
    
    // font
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0); 

    // image
    tex_box = GRRLIB_LoadTexture(box_png);
    GRRLIB_SetMidHandle(tex_box, true);

    tex_gold_box = GRRLIB_LoadTexture(gold_box_png);
    GRRLIB_SetMidHandle(tex_gold_box, true);
    
    tex_tele_box = GRRLIB_LoadTexture(tele_box_png);
    GRRLIB_SetMidHandle(tex_tele_box, true);
    
    tex_tnt_box = GRRLIB_LoadTexture(tnt_box_png);
    GRRLIB_SetMidHandle(tex_tnt_box, true);

    tex_thing_1 = GRRLIB_LoadTexture(thing_1_png);
    GRRLIB_SetMidHandle(tex_thing_1, true);

    // wiimote
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	
	setup_box2d();

    while (true)
    {
        GRRLIB_FillScreen(GRRLIB_PURPLE);
        if (difficulty != 4) {
            GRRLIB_Printf(250, 264, tex_BMfont5, GRRLIB_WHITE, 1, "Press A to start");
            GRRLIB_Printf(100, 284, tex_BMfont5, GRRLIB_WHITE, 1, "Press the up or down arrows to change difficulty (%d)", difficulty);
        }
        else {
            GRRLIB_Printf(250 + rand() % 5, 264 + rand() % 5, tex_BMfont5, GRRLIB_WHITE, 1, "Press A to start");
            GRRLIB_Printf(100 + rand() % 5, 284 + rand() % 5, tex_BMfont5, GRRLIB_WHITE, 1, "Press the up or down arrows to change difficulty (%d)", difficulty);
        }
        GRRLIB_Render();
        
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0) | WPAD_ButtonsDown(1) | WPAD_ButtonsDown(2) | WPAD_ButtonsDown(3);
        if (pressed & WPAD_BUTTON_A) {
            break;
        }
        
        if (pressed & WPAD_BUTTON_HOME) {
            difficulty = 4;
        }

        if (pressed & WPAD_BUTTON_UP) {
            difficulty++;
            if (difficulty > 3) {
                difficulty = 1;
            }
        }

        if (pressed & WPAD_BUTTON_DOWN) {
            difficulty--;
            if (difficulty < 1) {
                difficulty = 3;
            }
        }
    }

	while(true) {
        box2d_next_frame();
        time_left = time_limit - (frame / 60.0);
        GRRLIB_FillScreen(GRRLIB_PURPLE);
        
        // draw floor
        b2Vec2 pos = b2Body_GetPosition(groundId);
        b2Rot rot = b2Body_GetRotation(groundId);
        draw(pos.x, pos.y, tex_thing_1, rot, 10, 10, 0xffffffff);
        
        WPAD_ScanPads();
        for (int wiimote = 0; wiimote <= 3; wiimote++) {
            WPADData* data = WPAD_Data(wiimote);
            if(data->data_present) {
                for (int i=0; i<boxes; i++) {
                   draw_box(i, data->ir.x, data->ir.y);
                }

                if (wiimote == 0) {
                    GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_box, 1, 0.1, 0.1, 0xff0000ff);
                }
                else if (wiimote == 1) {
                    GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_box, 1, 0.1, 0.1, 0x0000ffff);
                }
                else if (wiimote == 2) {
                    GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_box, 1, 0.1, 0.1, 0x11ff00ff);
                }
                else {
                    GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_box, 1, 0.1, 0.1, 0xffee00ff);
                }

                if ((data->btns_d && WPAD_BUTTON_A) || (data->btns_d && WPAD_BUTTON_B)) {
                    for (int i=0; i<boxes; i++) {
                        if (isPointTouchingBox(data->ir.x, data->ir.y, b2Body_GetPosition(boxID[i]).x, b2Body_GetPosition(boxID[i]).y, box_size[i])) {
                            box_hp[i] -= 1;
                            if (box_hp[i] <= 0) {
                                b2Body_SetTransform(boxID[i], (b2Vec2){-1000, -1000}, b2MakeRot(0));
                                if (difficulty == 1) {
                                    score += box_score[i] * 5;
                                    time_limit += 0.1;
                                }
                                else if (difficulty == 2) {
                                    score += box_score[i] * 4;
                                    time_limit += 0.05;
                                }
                                else if (difficulty == 3) {
                                    score += box_score[i] * 3;
                                }
                                else {
                                    score += box_score[i];
                                }
                            }
                            else {
                                score += box_score[i];
                                if (box_img[i] == TELE_BOX) {
                                    respawn_box(i);
                                }
                            }
                        }
                    }
                }
                if (debug) {
                    GRRLIB_Printf(295, 5 + wiimote * 20, tex_BMfont5, GRRLIB_WHITE, 1, "wiimote %d: %0.1fX %f0.1Y %fA", wiimote, data->ir.x, data->ir.y, data->ir.angle);
                }
            }
        }
        GRRLIB_Printf(5, 5, tex_BMfont5, GRRLIB_WHITE, 1, "Time remaining %0.1f", time_left);
        GRRLIB_Printf(5, 20, tex_BMfont5, GRRLIB_WHITE, 1, "Score %d", score);
        GRRLIB_Printf(5, 35, tex_BMfont5, GRRLIB_WHITE, 1, "Difficulty %d", difficulty);
        GRRLIB_Render();
        
        u32 pressed = WPAD_ButtonsDown(0) | WPAD_ButtonsDown(1) | WPAD_ButtonsDown(2) | WPAD_ButtonsDown(3);
        if ((time_left <= 0.0) || 
                (difficulty > 2 && score < 0) || 
                (pressed & WPAD_BUTTON_HOME)) {
            // stop the game
            break;
        }
    }
	// clean up box2d and GRRLIB
	clean_up_box2d();
    GRRLIB_FreeTexture(tex_box);
    GRRLIB_FreeTexture(tex_gold_box);
    GRRLIB_FreeTexture(tex_tele_box);
    GRRLIB_FreeTexture(tex_thing_1);
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_Exit();
    return 0;
}


int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}


bool isPointTouchingBox(float pointX, float pointY, float boxX, float boxY, float boxsize) {
    float offsetX = abs(boxX * 25 + 320 - pointX);
    float offsetY = abs(boxY * - 25 + 264  - pointY);
    
    if (offsetY < boxsize * 35 && offsetX < boxsize * 35) {
        return true;
    }
    return false;
}


int disToPoint(float pointX, float pointY, float boxX, float boxY) {
    float offsetX = abs(boxX * 25 + 320 - pointX);
    float offsetY = abs(boxY * - 25 + 264  - pointY);
    return (offsetX + offsetY) / 2;
}


void draw(float x, float y, GRRLIB_texImg *img, b2Rot rot, float size_x, float size_y, int color) {
    GRRLIB_DrawImg((x * 25) + 320, (y * - 25) + 264, img, b2Rot_GetAngle(rot) / B2_PI * - 180, size_x * 0.25, size_y * 0.25, color);
}