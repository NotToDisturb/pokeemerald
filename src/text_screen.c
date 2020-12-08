#include "global.h"
#include "main.h"
#include "bg.h"
#include "sprite.h"
#include "palette.h"
#include "text.h"
#include "text_window.h"
#include "string.h"
#include "string_util.h"
#include "menu.h"
#include "menu_helpers.h"
#include "decompress.h"
#include "window.h"
#include "string.h"
#include "task.h"
#include "sound.h"
#include "gpu_regs.h"
#include "malloc.h"
#include "scanline_effect.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "constants/event_objects.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "trig.h"
#include "text_screen.h"
#include "data/text/text_screen.h"
#include "field_weather.h"
#include "script.h"

enum{BG_0, 
    BG_COUNT};
static const struct BgTemplate sBgTemplates_TextScreen[BG_COUNT] = {
    [BG_0] = {
        .bg = BG_0,
        .charBaseIndex = 0,
        .mapBaseIndex = 21,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
};

#define TEXT_SCREEN_WINDOW_BG               0
#define TEXT_SCREEN_WINDOW_PALETTE_NUM      15
#define TEXT_SCREEN_WINDOW_BASE_BLOCK       1

#define TEXT_WINDOW_BG_PALETTE              0
#define TEXT_WINDOW_BG_PALETTE_SIZE         2

static EWRAM_DATA u8 sTextScreenWindowId = 0;

static EWRAM_DATA u8 sTextScreenTextId = 0;
static EWRAM_DATA u8 sTextScreenTextX = 0;
static EWRAM_DATA u8 sTextScreenTextY = 0;
static EWRAM_DATA u8 sTextScreenTextWidth = 0;
static EWRAM_DATA u8 sTextScreenTextHeight = 0;
static EWRAM_DATA u8 sTextScreenTextSpeed = 0;
static EWRAM_DATA u8 sTextScreenBgColor = 0;
static EWRAM_DATA u8 sTextScreenFontId = 0;
static EWRAM_DATA u8 sTextScreenFontColor = 0;

static const u8* const sTextScreen_Texts[TEXT_SCREEN_TEXT_COUNT] =
{
    [TEXT_SCREEN_TEXT_EXAMPLE] = sText_Example,
};

static const u16 sTextScreen_BgColor[TEXT_SCREEN_BG_COLOR_COUNT][1] = 
{
    [TEXT_SCREEN_BG_COLOR_BLACK] = {RGB(0,0,0)},
};

static const u8 sTextScreen_FontColors[TEXT_SCREEN_FONT_COLOR_COUNT][3] = 
{
    [TEXT_SCREEN_FONT_COLOR_WHITE] = {0, 1, 2},
};

static void TextScreen_ScriptReadValues(struct ScriptContext *ctx);
void CB2_InitTextScreen();
static void TextScreen_InitBgs();
static void TextScreen_InitWindow();
static void TextScreen_LoadText();
static void CB2_TextScreen();
static void VBlankCallback_TextScreen();
static void Task_TextScreen_PrintText(u8 taskId);
static void Task_TextScreen_HandleInput(u8 taskId);
static u8 GetValidValue(u8 value, u8 maxValue);

void InitTextScreen(struct ScriptContext *ctx)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB_BLACK);
    PlayRainStoppingSoundEffect();
    CleanupOverworldWindowsAndTilemaps();
    TextScreen_ScriptReadValues(ctx);
    SetMainCallback2(CB2_InitTextScreen);
}

static void TextScreen_ScriptReadValues(struct ScriptContext *ctx)
{
    sTextScreenTextId = GetValidValue(ScriptReadByte(ctx), TEXT_SCREEN_TEXT_COUNT);
    sTextScreenTextX = ScriptReadByte(ctx);
    sTextScreenTextY = ScriptReadByte(ctx);
    sTextScreenTextWidth = ScriptReadByte(ctx);
    sTextScreenTextHeight = ScriptReadByte(ctx);
    sTextScreenTextSpeed = ScriptReadByte(ctx);
    sTextScreenBgColor = GetValidValue(ScriptReadByte(ctx), TEXT_SCREEN_BG_COLOR_COUNT);
    sTextScreenFontId = ScriptReadByte(ctx);
    sTextScreenFontColor = GetValidValue(ScriptReadByte(ctx), TEXT_SCREEN_FONT_COLOR_COUNT);
}

void CB2_InitTextScreen()
{
    if(!UpdatePaletteFade())
    {
        switch (gMain.state)
        {
        case 0:
            SetVBlankCallback(NULL);
            ResetPaletteFade();
            ScanlineEffect_Stop();
            ResetTasks();
            ResetSpriteData();
            gMain.state++;
            break;
        case 1:
            TextScreen_InitBgs();
            TextScreen_InitWindow();
            gMain.state++;
            break;
        case 2:
            LoadPalette(sTextScreen_BgColor[sTextScreenBgColor], TEXT_WINDOW_BG_PALETTE * 0x10, TEXT_WINDOW_BG_PALETTE_SIZE * 2);
            gMain.state++;
            break;
        case 3:
            PutWindowTilemap(sTextScreenWindowId);
            TextScreen_LoadText();
            gMain.state++;
            break;
        default:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0x10, 0, RGB_BLACK);
            
            SetMainCallback2(CB2_TextScreen);
            SetVBlankCallback(VBlankCallback_TextScreen);
            
            CreateTask(Task_TextScreen_PrintText, 0);
            break;
        }
    }
}

static void TextScreen_InitBgs()
{
    ResetVramOamAndBgCntRegs();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates_TextScreen, BG_COUNT);
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    ShowBg(0);
}

static void TextScreen_InitWindow()
{
    sTextScreenWindowId = sub_8198AA4(TEXT_SCREEN_WINDOW_BG, 
                                    sTextScreenTextX, sTextScreenTextY, 
                                    sTextScreenTextWidth, sTextScreenTextHeight, 
                                    TEXT_SCREEN_WINDOW_PALETTE_NUM, TEXT_SCREEN_WINDOW_BASE_BLOCK);
    DeactivateAllTextPrinters();
}

static void TextScreen_LoadText(){
    AddTextPrinterParameterized3(sTextScreenWindowId, sTextScreenFontId, 0, 0, 
                                sTextScreen_FontColors[sTextScreenFontColor], sTextScreenTextSpeed, 
                                sTextScreen_Texts[sTextScreenTextId]);
    CopyWindowToVram(sTextScreenWindowId, 3);
}

static void CB2_TextScreen()
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCallback_TextScreen()
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_TextScreen_PrintText(u8 taskId)
{
    if(UpdatePaletteFade())
        return;
    
    RunTextPrinters();
    if (!IsTextPrinterActive(sTextScreenWindowId))
    {
        gTasks[taskId].func = Task_TextScreen_HandleInput;
    }
}

static void Task_TextScreen_HandleInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScript);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB_BLACK);
        DestroyTask(taskId);
    }
}

static u8 GetValidValue(u8 value, u8 maxValue)
{
    if(value < maxValue)
        return value;
    return 0;
}