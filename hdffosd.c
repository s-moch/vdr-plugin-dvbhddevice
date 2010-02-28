/*
 * hdffosd.c: Implementation of the DVB HD Full Featured On Screen Display
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: hdffosd.c 1.1 2009/12/31 14:51:23 kls Exp $
 */

#include "hdffosd.h"
#include <linux/dvb/osd.h>
#include <sys/ioctl.h>
#include "hdffcmd.h"
#include "setup.h"

#define MAX_NUM_FONTFACES  8
#define MAX_NUM_FONTS      8

typedef struct _tFontFace
{
    cString Name;
    uint32_t Handle;
} tFontFace;

typedef struct _tFont
{
    uint32_t hFontFace;
    int Size;
    uint32_t Handle;
} tFont;

class cHdffOsd : public cOsd
{
private:
    HDFF::cHdffCmdIf * mHdffCmdIf;
    int mLeft;
    int mTop;
    int mDispWidth;
    int mDispHeight;
    bool shown;
    bool mChanged;
    bool mBitmapModified;
    uint32_t mDisplay;
    tFontFace mFontFaces[MAX_NUM_FONTFACES];
    tFont mFonts[MAX_NUM_FONTS];
    uint32_t mBitmapPalette;
    uint32_t mBitmapColors[256];
    uint32_t mBitmapNumColors;

protected:
    virtual void SetActive(bool On);
public:
    cHdffOsd(int Left, int Top, HDFF::cHdffCmdIf * pHdffCmdIf, uint Level);
    virtual ~cHdffOsd();
    cBitmap *GetBitmap(int Area);
    //virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);
    virtual void SaveRegion(int x1, int y1, int x2, int y2);
    virtual void RestoreRegion(void);
    virtual void DrawPixel(int x, int y, tColor Color);
    virtual void DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0, bool ReplacePalette = false, bool Overlay = false);
    virtual void DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width = 0, int Height = 0, int Alignment = taDefault);
    virtual void DrawRectangle(int x1, int y1, int x2, int y2, tColor Color);
    virtual void DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants = 0);
    virtual void DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type);
    virtual void Flush(void);
};

cHdffOsd::cHdffOsd(int Left, int Top, HDFF::cHdffCmdIf * pHdffCmdIf, uint Level)
:   cOsd(Left, Top, Level)
{
    double pixelAspect;
    HDFF::tOsdConfig config;

    mHdffCmdIf = pHdffCmdIf;
    mLeft = Left;
    mTop = Top;
    shown = false;
    mChanged = false;
    mBitmapModified = false;
    mBitmapPalette = InvalidHandle;
    config.FontKerning = false;
    config.FontAntialiasing = Setup.AntiAlias ? true : false;
    mHdffCmdIf->CmdOsdConfigure(&config);

    gHdffSetup.GetOsdSize(mDispWidth, mDispHeight, pixelAspect);
    mDisplay = mHdffCmdIf->CmdOsdCreateDisplay(mDispWidth, mDispHeight, HDFF::colorTypeARGB8888);
    mHdffCmdIf->CmdOsdSetDisplayOutputRectangle(mDisplay, 0, 0, SizeFullScreen, SizeFullScreen);
    for (int i = 0; i < MAX_NUM_FONTFACES; i++)
    {
        mFontFaces[i].Name = "";
        mFontFaces[i].Handle = InvalidHandle;
    }
    for (int i = 0; i < MAX_NUM_FONTS; i++)
    {
        mFonts[i].hFontFace = InvalidHandle;
        mFonts[i].Size = 0;
        mFonts[i].Handle = InvalidHandle;
    }
}

cHdffOsd::~cHdffOsd()
{
    printf("~cHdffOsd %d %d\n", mLeft, mTop);
    SetActive(false);

    for (int i = 0; i < MAX_NUM_FONTS; i++)
    {
        if (mFonts[i].Handle == InvalidHandle)
            break;
        mHdffCmdIf->CmdOsdDeleteFont(mFonts[i].Handle);
    }
    for (int i = 0; i < MAX_NUM_FONTFACES; i++)
    {
        if (mFontFaces[i].Handle == InvalidHandle)
            break;
        mHdffCmdIf->CmdOsdDeleteFontFace(mFontFaces[i].Handle);
    }

    if (mBitmapPalette != InvalidHandle)
        mHdffCmdIf->CmdOsdDeletePalette(mBitmapPalette);
    mHdffCmdIf->CmdOsdDrawRectangle(mDisplay, 0, 0, mDispWidth, mDispHeight, 0);
    mHdffCmdIf->CmdOsdRenderDisplay(mDisplay);
    mHdffCmdIf->CmdOsdDeleteDisplay(mDisplay);
}

cBitmap * cHdffOsd::GetBitmap(int Area)
{
    mChanged = true;
    mBitmapModified = true;
    return cOsd::GetBitmap(Area);
}

eOsdError cHdffOsd::SetAreas(const tArea *Areas, int NumAreas)
{
    for (int i = 0; i < NumAreas; i++)
    {
        printf("SetAreas %d: %d %d %d %d %d\n", i, Areas[i].x1, Areas[i].y1, Areas[i].x2, Areas[i].y2, Areas[i].bpp);
    }
    mHdffCmdIf->CmdOsdDrawRectangle(mDisplay, 0, 0, mDispWidth, mDispHeight, 0);
    mHdffCmdIf->CmdOsdRenderDisplay(mDisplay);
    return cOsd::SetAreas(Areas, NumAreas);
}

void cHdffOsd::SetActive(bool On)
{
    if (On != Active())
    {
        cOsd::SetActive(On);
        if (On)
        {
        }
        else if (shown)
        {
            shown = false;
        }
    }
}

void cHdffOsd::SaveRegion(int x1, int y1, int x2, int y2)
{
}

void cHdffOsd::RestoreRegion(void)
{
}

void cHdffOsd::DrawPixel(int x, int y, tColor Color)
{
    printf("DrawPixel\n");
    mBitmapModified = false;
}

void cHdffOsd::DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg, tColor ColorBg, bool ReplacePalette, bool Overlay)
{
    printf("DrawBitmap\n");
    int i;
    int numColors;
    const tColor * colors = Bitmap.Colors(numColors);

    for (i = 0; i < numColors; i++)
    {
        mBitmapColors[i] = colors[i];
        if (ColorFg || ColorBg)
        {
            if (i == 0)
                mBitmapColors[i] = ColorBg;
            else if (i == 1)
                mBitmapColors[i] = ColorFg;
        }
    }
    if (mBitmapPalette == InvalidHandle)
    {
        mBitmapPalette = mHdffCmdIf->CmdOsdCreatePalette(HDFF::colorTypeClut8,
                HDFF::colorFormatARGB, numColors, mBitmapColors);
    }
    else
    {
        mHdffCmdIf->CmdOsdSetPaletteColors(mBitmapPalette,
                HDFF::colorFormatARGB, 0, numColors, mBitmapColors);
    }
    mHdffCmdIf->CmdOsdDrawBitmap(mDisplay, mLeft + x, mTop + y,
        (uint8_t *) Bitmap.Data(0, 0), Bitmap.Width(), Bitmap.Height(),
        Bitmap.Width() * Bitmap.Height(), HDFF::colorTypeClut8, mBitmapPalette);
#if 0
    uint32_t * tmpBitmap = new uint32_t[Bitmap.Width() * Bitmap.Height()];
    for (int ix = 0; ix < Bitmap.Width(); ix++)
    {
        for (int iy = 0; iy < Bitmap.Height(); iy++)
        {
            const tIndex * pixel = Bitmap.Data(ix, iy);
            tColor color = Bitmap.Color(*pixel);
            if (!Overlay || *pixel != 0)
            {
                if (ColorFg || ColorBg)
                {
                    if (*pixel == 0)
                        color = ColorBg;
                    else if (*pixel == 1)
                        color = ColorFg;
                }
                tmpBitmap[Bitmap.Width() * iy + ix] = color;
            }
        }
    }
    mHdffCmdIf->CmdOsdDrawBitmap(mDisplay, mLeft + x, mTop + y,
        (uint8_t *) tmpBitmap, Bitmap.Width(), Bitmap.Height(),
        Bitmap.Width() * Bitmap.Height() * 4, HDFF::colorTypeARGB8888, InvalidHandle);
    delete[] tmpBitmap;
#endif
    mChanged = true;
    mBitmapModified = false;
}

void cHdffOsd::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment)
{
    int w = Font->Width(s);
    int h = Font->Height();
    int limit = 0;
    int cw = Width ? Width : w;
    int ch = Height ? Height : h;
    int i;
    int size = Font->Size();
    tFontFace * pFontFace;
    tFont * pFont;

    if (ColorBg != clrTransparent)
        mHdffCmdIf->CmdOsdDrawRectangle(mDisplay, mLeft + x, mTop + y, cw, ch, ColorBg);

    if (s == NULL)
        return;

    pFontFace = NULL;
    for (i = 0; i < MAX_NUM_FONTFACES; i++)
    {
        if (mFontFaces[i].Handle == InvalidHandle)
            break;

        if (strcmp(mFontFaces[i].Name, Font->FontName()) == 0)
        {
            pFontFace = &mFontFaces[i];
            break;
        }
    }
    if (pFontFace == NULL)
    {
        if (i < MAX_NUM_FONTFACES)
        {
            cString fontFileName = Font->FontName();
            FILE * fp = fopen(fontFileName, "rb");
            if (fp)
            {
                fseek(fp, 0, SEEK_END);
                long fileSize = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                if (fileSize > 0)
                {
                    uint8_t * buffer = new uint8_t[fileSize];
                    if (buffer)
                    {
                        if (fread(buffer, fileSize, 1, fp) == 1)
                        {
                            mFontFaces[i].Handle = mHdffCmdIf->CmdOsdCreateFontFace(buffer, fileSize);
                            if (mFontFaces[i].Handle != InvalidHandle)
                            {
                                mFontFaces[i].Name = Font->FontName();
                                pFontFace = &mFontFaces[i];
                            }
                        }
                        delete[] buffer;
                    }
                }
                fclose(fp);
            }
        }
    }
    if (pFontFace == NULL)
        return;

    pFont = NULL;
    for (i = 0; i < MAX_NUM_FONTS; i++)
    {
        if (mFonts[i].Handle == InvalidHandle)
            break;

        if (mFonts[i].hFontFace == pFontFace->Handle
          && mFonts[i].Size == size)
        {
            pFont = &mFonts[i];
            break;
        }
    }
    if (pFont == NULL)
    {
        if (i < MAX_NUM_FONTS)
        {
            mFonts[i].Handle = mHdffCmdIf->CmdOsdCreateFont(pFontFace->Handle, size);
            if (mFonts[i].Handle != InvalidHandle)
            {
                mFonts[i].hFontFace = pFontFace->Handle;
                mFonts[i].Size = size;
                pFont = &mFonts[i];
            }
        }
    }
    if (pFont == NULL)
        return;

    mHdffCmdIf->CmdOsdSetDisplayClippingArea(mDisplay, true, mLeft + x, mTop + y, cw, ch);

    if (Width || Height)
    {
        limit = x + cw;// - mLeft;
        if (Width)
        {
            if ((Alignment & taLeft) != 0)
                ;
            else if ((Alignment & taRight) != 0)
            {
                if (w < Width)
                    x += Width - w;
            }
            else
            { // taCentered
                if (w < Width)
                    x += (Width - w) / 2;
            }
        }
        if (Height)
        {
            if ((Alignment & taTop) != 0)
                ;
            else if ((Alignment & taBottom) != 0)
            {
                if (h < Height)
                    y += Height - h;
            }
            else
            { // taCentered
                if (h < Height)
                    y += (Height - h) / 2;
            }
        }
    }
    //x -= mLeft;
    //y -= mTop;
    {
        uint16_t tmp[1000];
        uint16_t len = 0;
        while (*s && (len < (sizeof(tmp) - 1)))
        {
            int sl = Utf8CharLen(s);
            uint sym = Utf8CharGet(s, sl);
            s += sl;
            tmp[len] = sym;
            len++;
        }
        tmp[len] = 0;
        mHdffCmdIf->CmdOsdDrawTextW(mDisplay, pFont->Handle, x + mLeft, y + mTop + h, tmp, ColorFg);
    }
    //mHdffCmdIf->CmdOsdDrawText(mDisplay, pFont->Handle, x + mLeft, y + mTop + h - 7, s, ColorFg);
    mHdffCmdIf->CmdOsdSetDisplayClippingArea(mDisplay, false, 0, 0, 0, 0);
    //Font->DrawText(this, x, y, s, ColorFg, ColorBg, limit);
    mChanged = true;
    mBitmapModified = false;
}

void cHdffOsd::DrawRectangle(int x1, int y1, int x2, int y2, tColor Color)
{
    mHdffCmdIf->CmdOsdDrawRectangle(mDisplay, mLeft + x1, mTop + y1, x2 - x1 + 1, y2 - y1 + 1, Color);
    mChanged = true;
    mBitmapModified = false;
}

void cHdffOsd::DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants)
{
    uint32_t flags;
    int cx;
    int cy;
    int rx;
    int ry;

    switch (abs(Quadrants))
    {
        case 1:
            if (Quadrants > 0)
                flags = HDFF::drawQuarterTopRight;
            else
                flags = HDFF::drawQuarterTopRightInverted;
            cx = x1;
            cy = y2;
            rx = x2 - x1;
            ry = y2 - y1;
            break;
        case 2:
            if (Quadrants > 0)
                flags = HDFF::drawQuarterTopLeft;
            else
                flags = HDFF::drawQuarterTopLeftInverted;
            cx = x2;
            cy = y2;
            rx = x2 - x1;
            ry = y2 - y1;
            break;
        case 3:
            if (Quadrants > 0)
                flags = HDFF::drawQuarterBottomLeft;
            else
                flags = HDFF::drawQuarterBottomLeftInverted;
            cx = x2;
            cy = y1;
            rx = x2 - x1;
            ry = y2 - y1;
            break;
        case 4:
            if (Quadrants > 0)
                flags = HDFF::drawQuarterBottomRight;
            else
                flags = HDFF::drawQuarterBottomRightInverted;
            cx = x1;
            cy = y1;
            rx = x2 - x1;
            ry = y2 - y1;
            break;
        case 5:
            flags = HDFF::drawHalfRight;
            cx = x1;
            cy = (y1 + y2) / 2;
            rx = x2 - x1;
            ry = (y2 - y1) / 2;
            break;
        case 6:
            flags = HDFF::drawHalfTop;
            cx = (x1 + x2) / 2;
            cy = y2;
            rx = (x2 - x1) / 2;
            ry = y2 - y1;
            break;
        case 7:
            flags = HDFF::drawHalfLeft;
            cx = x2;
            cy = (y1 + y2) / 2;
            rx = x2 - x1;
            ry = (y2 - y1) / 2;
            break;
        case 8:
            flags = HDFF::drawHalfBottom;
            cx = (x1 + x2) / 2;
            cy = y1;
            rx = (x2 - x1) / 2;
            ry = y2 - y1;
            break;
        default:
            flags = HDFF::drawFull;
            cx = (x1 + x2) / 2;
            cy = (y1 + y2) / 2;
            rx = (x2 - x1) / 2;
            ry = (y2 - y1) / 2;
            break;
    }
    mHdffCmdIf->CmdOsdDrawEllipse(mDisplay, mLeft + cx, mTop + cy, rx, ry, Color, flags);
    mChanged = true;
    mBitmapModified = false;
}

void cHdffOsd::DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type)
{
    mChanged = true;
    mBitmapModified = false;
}

void cHdffOsd::Flush(void)
{
    if (!Active())
        return;

    if (!mChanged)
        return;

    if (mBitmapModified)
    {
        cBitmap *Bitmap;
        for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++)
        {
            DrawBitmap(0, 0, *Bitmap);
        }
    }

    mHdffCmdIf->CmdOsdRenderDisplay(mDisplay);

    mChanged = false;
    mBitmapModified = false;
}


cHdffOsdProvider::cHdffOsdProvider(HDFF::cHdffCmdIf * HdffCmdIf)
{
    mHdffCmdIf = HdffCmdIf;
}

cOsd *cHdffOsdProvider::CreateOsd(int Left, int Top, uint Level)
{
    printf("CreateOsd %d %d %d\n", Left, Top, Level);
    return new cHdffOsd(Left, Top, mHdffCmdIf, Level);
}
