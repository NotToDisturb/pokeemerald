#include "global.h"
#include "palette.h"
#include "string_util.h"
#include "window.h"
#include "decompress.h"
#include "task.h"
#include "text.h"
#include "field_name_box.h"

#define NAMEBOX_TAG 0x2722

//  Usage:
//  namebox <text>  ->  displays the namebox with the given text
//  hidenamebox     ->  hides the namebox

static void LoadNameboxWindow(const struct WindowTemplate *window);
static void LoadNameboxTilemap();
static void CreateTask_DisplayNamebox();
static void Task_DisplayNamebox(u8 taskId);
static void LoadNameboxSprite(s8 x, s8 y);
static void AddTextPrinterForName();
static void ClearNameboxTiles();

static EWRAM_DATA u8 sNameboxWindowId = 0;
static EWRAM_DATA u8 sNameboxGfxId = 0;

static const u32 sNamebox_Gfx[] = INCBIN_U32("graphics/text_window/name_box.4bpp.lz");
static const u16 sNamebox_Pal[] = INCBIN_U16("graphics/text_window/name_box.gbapal");

static const struct OamData sOam_Namebox =
{
    .x = 0,
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
};


static const struct CompressedSpriteSheet sSpriteSheet_Namebox = {
    .data = sNamebox_Gfx,
    .size = 1024,
    .tag = NAMEBOX_TAG,
};

static const union AnimCmd sSpriteAnim_Namebox[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_Namebox[] = {
    sSpriteAnim_Namebox,
};

static const struct SpriteTemplate sSpriteTemplate_Namebox = {
    .tileTag = NAMEBOX_TAG,
    .paletteTag = NAMEBOX_TAG,
    .oam = &sOam_Namebox,
    .anims = sSpriteAnimTable_Namebox,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct SpritePalette sSpritePalette_Namebox = {
    .data = sNamebox_Pal, 
    .tag = NAMEBOX_TAG,
};

static const struct WindowTemplate sNamebox_WindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1, 
    .tilemapTop = 12,
    .width = 6, 
    .height = 2,                //required to be at least two
    .paletteNum = 15,
    .baseBlock = 0x188          //placed before the msgbox tiles
};


void ShowFieldName(const u8 *str) {
    if(IsNameboxDisplayed())
            ClearNamebox();
            
    LoadNameboxWindow(&sNamebox_WindowTemplate);
    StringExpandPlaceholders(gStringVar3, str);
    AddTextPrinterForName();
    CreateTask_DisplayNamebox();
}

bool8 IsNameboxDisplayed() {
    if(sNameboxWindowId == 0)
        return FALSE;
    return TRUE;
}

void ClearNamebox() {
    ClearNameboxTiles();
    RemoveWindow(sNameboxWindowId);
    sNameboxWindowId = 0;
    DestroySpriteAndFreeResources(&gSprites[sNameboxGfxId]);
}


#define tTimer data[0]

static void CreateTask_DisplayNamebox() {
    u8 taskId = CreateTask(Task_DisplayNamebox, 0x50);
    gTasks[taskId].tTimer = 0x1;
}

static void Task_DisplayNamebox(u8 taskId) {
    struct Task *task = &gTasks[taskId];
    
    if (gTasks[taskId].tTimer) 
        gTasks[taskId].tTimer--;
    else{
        LoadNameboxTilemap();
        LoadNameboxSprite(32, 104);
        
        DestroyTask(taskId);
    }
}

#undef tTimer


static void LoadNameboxWindow(const struct WindowTemplate *window) {
    sNameboxWindowId = AddWindow(window);
    
    CopyWindowToVram(sNameboxWindowId, 3);
}

static void LoadNameboxTilemap() {
    PutWindowTilemap(sNameboxWindowId);
}

static void LoadNameboxSprite(s8 x, s8 y) {
    u8 spriteId;
    
    LoadCompressedSpriteSheet(&sSpriteSheet_Namebox);
    LoadSpritePalette(&sSpritePalette_Namebox);
    spriteId = CreateSprite(&sSpriteTemplate_Namebox, x, y, 0);
    if (sNameboxGfxId == MAX_SPRITES)
        return;
    else
        sNameboxGfxId = spriteId;
}

static void AddTextPrinterForName() {
    struct TextPrinterTemplate printer;
    
    printer.currentChar = gStringVar3;
    printer.windowId = sNameboxWindowId;
    printer.fontId = 1;
    printer.x = 5; 
    printer.y = 0;
    printer.currentX = printer.x;
    printer.currentY = printer.y;
    printer.fgColor = GetFontAttribute(1, FONTATTR_COLOR_FOREGROUND);
    printer.bgColor = TEXT_COLOR_TRANSPARENT;
    printer.shadowColor = GetFontAttribute(1, FONTATTR_COLOR_SHADOW);
    printer.unk = GetFontAttribute(1, FONTATTR_UNKNOWN);
    printer.letterSpacing = 0;
    printer.lineSpacing = 0;

    AddTextPrinter(&printer, 0xFF, NULL);
}

static void ClearNameboxTiles(){
    ClearWindowTilemap(sNameboxWindowId);
    FillWindowPixelBuffer(sNameboxWindowId, PIXEL_FILL(0));
    CopyWindowToVram(sNameboxWindowId, 3);
}






