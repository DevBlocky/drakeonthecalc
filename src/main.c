#include <tice.h>
#include <graphx.h>
#include <compression.h>
#include <debug.h>

#include <string.h>
#include <assert.h>

#include "res/res.h"
#include "err.h"

// global variables
gfx_sprite_t *drake1 = NULL, *drake2 = NULL;

int loadSprites(void) {
    // variable initialization
    gfx_sprite_t *td1, *td2;

    if (drake1 || drake2)
        return ERR_IVLDOP;

    // allocating memory for sprites
    td1 =    gfx_MallocSprite(drake1_width, drake1_height);
    td2 =    gfx_MallocSprite(drake2_width, drake2_height);
    drake1 = gfx_MallocSprite(LCD_HEIGHT / 2, LCD_HEIGHT / 2);
    drake2 = gfx_MallocSprite(LCD_HEIGHT / 2, LCD_HEIGHT / 2);
    // checking if no memory errors
    if (!td1 || !td2 || !drake1 || !drake2)
        return ERR_NOMEM;
    
    // decompressing sprites into memory
    zx7_Decompress(td1, drake1_compressed);
    zx7_Decompress(td2, drake2_compressed);

    // scaling sprites
    gfx_ScaleSprite(td1, drake1);
    gfx_ScaleSprite(td2, drake2);

    // freeing temporary sprite memory
    free(td1);
    free(td2);

    return ERR_NOERR;
}
int unloadSprites(void) {
    if (!drake1 || !drake2)
        return ERR_IVLDOP;

    // removing images from memory
    free(drake1);
    free(drake2);

    return ERR_NOERR;
}
void drawText(const char *msg, int x, int y) {
    // variable initialization
    static const int LINE_LEN = 10;
    uint16_t nllen = 0, i;
    int yoff;
    char *tok, *cpy = msg;
    
    // finding number of new lines
    while (*cpy++ != '\0') {
        if (*cpy == '\n')
            nllen++;
    }

    // initializing text draw
    gfx_SetTextFGColor(gfx_black);
    // splitting string by new line
    tok = strtok(msg, "\n");
    // incrementing "i" while checking if "tok" is still valid
    for (i = 0; tok; i++) {
        // finding y offset by using number of new lines, then multiplying that by the line length
        yoff = (i - (nllen / 2)) * LINE_LEN;
        // printing centered text with y offset
        gfx_PrintStringXY(tok, x - (gfx_GetStringWidth(tok) / 2), y + yoff);

        // finding next token
        tok = strtok(NULL, "\n");
    }
}
int main(void) {
    // variable initialization
    static const int X = ((LCD_WIDTH - (LCD_HEIGHT / 2)) / 2) + (LCD_HEIGHT / 2);
    int err;
    const char caption1[256] = "CAPTION 1\nEXAMPLE TEXT";
    const char caption2[256] = "CAPTION 2\nEXAMPLE TEXT";

    // debug watchpoint
    dbg_SetWatchpoint(&err, sizeof(err));

    // loading sprites into stack
    err = loadSprites();
    assert(!err);

    // initialize graphics
    gfx_Begin();
    gfx_SetPalette(res_pal, sizeof(res_pal), 0);
    gfx_SetDrawBuffer();
    
    // fill buffer with white
    gfx_FillScreen(gfx_white);

    // draw "drake" sprites
    gfx_Sprite(drake1, 0, 0);
    gfx_Sprite(drake2, 0, LCD_HEIGHT / 2);

    // draw texts with function above
    drawText(caption1, X, (LCD_HEIGHT / 4));
    drawText(caption2, X, (LCD_HEIGHT / 4) * 3);

    // display buffer on screen
    gfx_BlitBuffer();

    // wait for user input (any key)
    while (!os_GetCSC());

    // end graphics
    gfx_End();

    // unload sprites
    err = unloadSprites();
    assert(!err);

    return 0;
}
