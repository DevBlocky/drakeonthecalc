#include <tice.h>
#include <graphx.h>
#include <compression.h>
#include <debug.h>

#include <string.h>
#include <assert.h>

#include "res/res.h"
#include "err.h"

gfx_sprite_t *drake1 = NULL, *drake2 = NULL;

int loadSprites(void) {
    gfx_sprite_t *td1, *td2;

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
    static const int LINE_LEN = 10;
    uint16_t nllen = 0, i;
    int yoff;
    char *tok, *cpy = msg;
    
    while (*cpy++ != '\0') {
        if (*cpy == '\n')
            nllen++;
    }

    gfx_SetTextFGColor(gfx_black);
    tok = strtok(msg, "\n");
    for (i = 0; tok; i++) {
        yoff = (i - (nllen / 2)) * LINE_LEN;
        gfx_PrintStringXY(tok, x - (gfx_GetStringWidth(tok) / 2), y + yoff);

        tok = strtok(NULL, "\n");
    }
}
int main(void) {
    static const int X = ((LCD_WIDTH - (LCD_HEIGHT / 2)) / 2) + (LCD_HEIGHT / 2);
    int err;
    const char caption1[256] = "Using built in calculator\nfunctions to make memes on\ncalculators";
    const char caption2[256] = "Making your own calculator\nProgram to show memes";

    dbg_SetWatchpoint(&err, sizeof(err));

    err = loadSprites();
    assert(!err);

    gfx_Begin();
    gfx_SetPalette(res_pal, sizeof(res_pal), 0);
    gfx_SetDrawBuffer();
    
    gfx_FillScreen(gfx_white);

    gfx_Sprite(drake1, 0, 0);
    gfx_Sprite(drake2, 0, LCD_HEIGHT / 2);

    drawText(caption1, X, (LCD_HEIGHT / 4));
    drawText(caption2, X, (LCD_HEIGHT / 4) * 3);

    gfx_BlitBuffer();

    while (!os_GetCSC());

    gfx_End();

    err = unloadSprites();
    assert(!err);

    return 0;
}
