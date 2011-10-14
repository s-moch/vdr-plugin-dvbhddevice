/*
 * hdffcmd.c: TODO(short description)
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: hdffcmd.c 1.1 2009/12/29 14:30:08 kls Exp $
 */

#include "hdffcmd.h"
#include "libhdffcmd/hdffcmd.h"
#include <stdio.h>
#include <string.h>
#include <vdr/tools.h>


namespace HDFF
{

cHdffCmdIf::cHdffCmdIf(int OsdDev)
{
    mOsdDev = OsdDev;
    if (mOsdDev < 0)
    {
        //printf("ERROR: invalid OSD device handle (%d)!\n", mOsdDev);
    }
}

cHdffCmdIf::~cHdffCmdIf(void)
{
}


uint32_t cHdffCmdIf::CmdGetFirmwareVersion(char * pString, uint32_t MaxLength)
{
    uint32_t version;
    int err;

    err = HdffCmdGetFirmwareVersion(mOsdDev, &version, pString, MaxLength);
    if (err == 0)
        return version;
    return 0;
}

uint32_t cHdffCmdIf::CmdGetInterfaceVersion(char * pString, uint32_t MaxLength)
{
    uint32_t version;
    int err;

    err = HdffCmdGetInterfaceVersion(mOsdDev, &version, pString, MaxLength);
    if (err == 0)
        return version;
    return 0;
}

uint32_t cHdffCmdIf::CmdGetCopyrights(uint8_t Index, char * pString, uint32_t MaxLength)
{
    int err;

    err = HdffCmdGetCopyrights(mOsdDev, Index, pString, MaxLength);
    if (err == 0)
        return strlen(pString);
    return 0;
}


void cHdffCmdIf::CmdAvSetPlayMode(uint8_t PlayMode, bool Realtime)
{
    HdffCmdAvSetPlayMode(mOsdDev, PlayMode, Realtime);
}

void cHdffCmdIf::CmdAvSetVideoPid(uint8_t DecoderIndex, uint16_t VideoPid, eVideoStreamType StreamType, bool PlaybackMode)
{
    //printf("SetVideoPid %d %d\n", VideoPid, StreamType);
    HdffCmdAvSetVideoPid(mOsdDev, DecoderIndex, VideoPid,
                         (HdffVideoStreamType_t) StreamType);
}

void cHdffCmdIf::CmdAvSetAudioPid(uint8_t DecoderIndex, uint16_t AudioPid, eAudioStreamType StreamType, eAVContainerType ContainerType)
{
    //printf("SetAudioPid %d %d %d\n", AudioPid, StreamType, ContainerType);
    HdffCmdAvSetAudioPid(mOsdDev, DecoderIndex, AudioPid,
                         (HdffAudioStreamType_t) StreamType,
                         (HdffAvContainerType_t) ContainerType);
}

void cHdffCmdIf::CmdAvSetPcrPid(uint8_t DecoderIndex, uint16_t PcrPid)
{
    //printf("SetPcrPid %d\n", PcrPid);
    HdffCmdAvSetPcrPid(mOsdDev, DecoderIndex, PcrPid);
}

void cHdffCmdIf::CmdAvSetTeletextPid(uint8_t DecoderIndex, uint16_t TeletextPid)
{
    HdffCmdAvSetTeletextPid(mOsdDev, DecoderIndex, TeletextPid);
}

void cHdffCmdIf::CmdAvSetVideoWindow(uint8_t DecoderIndex, bool Enable, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height)
{
    HdffCmdAvSetVideoWindow(mOsdDev, DecoderIndex, Enable, X, Y, Width, Height);
}

void cHdffCmdIf::CmdAvShowStillImage(uint8_t DecoderIndex, const uint8_t * pStillImage, int Size, eVideoStreamType StreamType)
{
    HdffCmdAvShowStillImage(mOsdDev, DecoderIndex, pStillImage, Size,
                            (HdffVideoStreamType_t) StreamType);
}

void cHdffCmdIf::CmdAvSetDecoderInput(uint8_t DecoderIndex, uint8_t DemultiplexerIndex)
{
    HdffCmdAvSetDecoderInput(mOsdDev, DecoderIndex, DemultiplexerIndex);
}

void cHdffCmdIf::CmdAvSetDemultiplexerInput(uint8_t DemultiplexerIndex, uint8_t TsInputIndex)
{
    HdffCmdAvSetDemultiplexerInput(mOsdDev, DemultiplexerIndex, TsInputIndex);
}

void cHdffCmdIf::CmdAvSetVideoFormat(uint8_t DecoderIndex, const tVideoFormat * pVideoFormat)
{
    HdffVideoFormat_t videoFormat;

    videoFormat.AutomaticEnabled = pVideoFormat->AutomaticEnabled;
    videoFormat.AfdEnabled = pVideoFormat->AfdEnabled;
    videoFormat.TvFormat = (HdffTvFormat_t) pVideoFormat->TvFormat;
    videoFormat.VideoConversion = (HdffVideoConversion_t) pVideoFormat->VideoConversion;

    HdffCmdAvSetVideoFormat(mOsdDev, DecoderIndex, &videoFormat);
}

void cHdffCmdIf::CmdAvSetVideoOutputMode(uint8_t DecoderIndex, eVideoOutputMode OutputMode)
{
    HdffCmdAvSetVideoOutputMode(mOsdDev, DecoderIndex,
                                (HdffVideoOutputMode_t) OutputMode);
}

void cHdffCmdIf::CmdAvSetStc(uint8_t DecoderIndex, uint64_t Stc)
{
    HdffCmdAvSetStc(mOsdDev, DecoderIndex, Stc);
}

void cHdffCmdIf::CmdAvFlushBuffer(uint8_t DecoderIndex, bool FlushAudio, bool FlushVideo)
{
    HdffCmdAvFlushBuffer(mOsdDev, DecoderIndex, FlushAudio, FlushVideo);
}

void cHdffCmdIf::CmdAvEnableSync(uint8_t DecoderIndex, bool EnableSync)
{
    HdffCmdAvEnableSync(mOsdDev, DecoderIndex, EnableSync, EnableSync);
}

void cHdffCmdIf::CmdAvSetVideoSpeed(uint8_t DecoderIndex, int32_t Speed)
{
    HdffCmdAvSetVideoSpeed(mOsdDev, DecoderIndex, Speed);
}

void cHdffCmdIf::CmdAvSetAudioSpeed(uint8_t DecoderIndex, int32_t Speed)
{
    HdffCmdAvSetAudioSpeed(mOsdDev, DecoderIndex, Speed);
}

void cHdffCmdIf::CmdAvEnableVideoAfterStop(uint8_t DecoderIndex, bool EnableVideoAfterStop)
{
    HdffCmdAvEnableVideoAfterStop(mOsdDev, DecoderIndex, EnableVideoAfterStop);
}

void cHdffCmdIf::CmdAvSetAudioDelay(int16_t Delay)
{
    HdffCmdAvSetAudioDelay(mOsdDev, Delay);
}

void cHdffCmdIf::CmdAvSetAudioDownmix(eDownmixMode DownmixMode)
{
    HdffCmdAvSetAudioDownmix(mOsdDev, (HdffAudioDownmixMode_t) DownmixMode);
}

void cHdffCmdIf::CmdAvSetAudioChannel(uint8_t AudioChannel)
{
    HdffCmdAvSetAudioChannel(mOsdDev, AudioChannel);
}


void cHdffCmdIf::CmdOsdConfigure(const tOsdConfig * pConfig)
{
    HdffOsdConfig_t osdConfig;

    memset(&osdConfig, 0, sizeof(osdConfig));
    osdConfig.FontAntialiasing = pConfig->FontAntialiasing;
    osdConfig.FontKerning = pConfig->FontKerning;

    HdffCmdOsdConfigure(mOsdDev, &osdConfig);
}

void cHdffCmdIf::CmdOsdReset(void)
{
    HdffCmdOsdReset(mOsdDev);
}

uint32_t cHdffCmdIf::CmdOsdCreateDisplay(uint32_t Width, uint32_t Height, eColorType ColorType)
{
    //printf("CreateDisplay %d %d %d\n", Width, Height, ColorType);
    uint32_t newDisplay;

    if (HdffCmdOsdCreateDisplay(mOsdDev, Width, Height, (HdffColorType_t) ColorType, &newDisplay) == 0)
        return newDisplay;
    return InvalidHandle;
}

void cHdffCmdIf::CmdOsdDeleteDisplay(uint32_t hDisplay)
{
    //printf("DeleteDisplay\n");
    HdffCmdOsdDeleteDisplay(mOsdDev, hDisplay);
}

void cHdffCmdIf::CmdOsdEnableDisplay(uint32_t hDisplay, bool Enable)
{
    //printf("EnableDisplay\n");
    HdffCmdOsdEnableDisplay(mOsdDev, hDisplay, Enable);
}

void cHdffCmdIf::CmdOsdSetDisplayOutputRectangle(uint32_t hDisplay, uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height)
{
    //printf("SetOutputRect %d %d %d %d %d\n", hDisplay, X, Y, Width, Height);
    HdffCmdOsdSetDisplayOutputRectangle(mOsdDev, hDisplay, X, Y, Width, Height);
}

void cHdffCmdIf::CmdOsdSetDisplayClippingArea(uint32_t hDisplay, bool Enable, uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height)
{
    //printf("SetClippingArea %d %d %d %d %d %d\n", hDisplay, Enable, X, Y, Width, Height);
    HdffCmdOsdSetDisplayClippingArea(mOsdDev, hDisplay, Enable, X, Y, Width, Height);
}

void cHdffCmdIf::CmdOsdRenderDisplay(uint32_t hDisplay)
{
    //printf("Render\n");
    HdffCmdOsdRenderDisplay(mOsdDev, hDisplay);
}

uint32_t cHdffCmdIf::CmdOsdCreatePalette(eColorType ColorType, eColorFormat ColorFormat,
                                         uint32_t NumColors, const uint32_t * pColors)
{
    uint32_t newPalette;
    int err;

    err = HdffCmdOsdCreatePalette(mOsdDev, (HdffColorType_t) ColorType,
                                  (HdffColorFormat_t) ColorFormat, NumColors,
                                  pColors, &newPalette);
    if (err == 0)
        return newPalette;
    return InvalidHandle;
}

void cHdffCmdIf::CmdOsdDeletePalette(uint32_t hPalette)
{
    HdffCmdOsdDeletePalette(mOsdDev, hPalette);
}

void cHdffCmdIf::CmdOsdSetDisplayPalette(uint32_t hDisplay, uint32_t hPalette)
{
    HdffCmdOsdSetDisplayPalette(mOsdDev, hDisplay, hPalette);
}

void cHdffCmdIf::CmdOsdSetPaletteColors(uint32_t hPalette, eColorFormat ColorFormat,
                                        uint8_t StartColor, uint32_t NumColors, const uint32_t * pColors)
{
    HdffCmdOsdSetPaletteColors(mOsdDev, hPalette, (HdffColorFormat_t) ColorFormat,
                               StartColor, NumColors, pColors);
}

uint32_t cHdffCmdIf::CmdOsdCreateFontFace(const uint8_t * pFontData, uint32_t DataSize)
{
    //printf("CreateFontFace %d\n", DataSize);
    uint32_t newFontFace;
    int err;

    err = HdffCmdOsdCreateFontFace(mOsdDev, pFontData, DataSize, &newFontFace);
    if (err == 0)
        return newFontFace;
    return InvalidHandle;
}

void cHdffCmdIf::CmdOsdDeleteFontFace(uint32_t hFontFace)
{
    //printf("DeleteFontFace %08X\n", hFontFace);
    HdffCmdOsdDeleteFontFace(mOsdDev, hFontFace);
}

uint32_t cHdffCmdIf::CmdOsdCreateFont(uint32_t hFontFace, uint32_t Size)
{
    //printf("CreateFont %d\n", Size);
    uint32_t newFont;
    int err;

    err = HdffCmdOsdCreateFont(mOsdDev, hFontFace, Size, &newFont);
    if (err == 0)
        return newFont;
    return InvalidHandle;
}

void cHdffCmdIf::CmdOsdDeleteFont(uint32_t hFont)
{
    //printf("DeleteFont %08X\n", hFont);
    HdffCmdOsdDeleteFont(mOsdDev, hFont);
}

void cHdffCmdIf::CmdOsdDrawRectangle(uint32_t hDisplay, int X, int Y, int Width, int Height, uint32_t Color)
{
    //printf("Rect (%d,%d) %d x %d, %08X\n", X, Y, Width, Height, Color);
    HdffCmdOsdDrawRectangle(mOsdDev, hDisplay, X, Y, Width, Height, Color);
}

void cHdffCmdIf::CmdOsdDrawEllipse(uint32_t hDisplay, int CX, int CY, int RadiusX, int RadiusY,
                                 uint32_t Color, uint32_t Flags)
{
    //printf("Ellipse (%d,%d) %d x %d, %08X, %d\n", CX, CY, RadiusX, RadiusY, Color, Flags);
    HdffCmdOsdDrawEllipse(mOsdDev, hDisplay, CX, CY, RadiusX, RadiusY, Color, Flags);
}

void cHdffCmdIf::CmdOsdDrawText(uint32_t hDisplay, uint32_t hFont, int X, int Y, const char * pText, uint32_t Color)
{
    //printf("Text %08X (%d,%d), %s, %08X\n", hFont, X, Y, pText, Color);
    HdffCmdOsdDrawText(mOsdDev, hDisplay, hFont, X, Y, pText, Color);
}

void cHdffCmdIf::CmdOsdDrawTextW(uint32_t hDisplay, uint32_t hFont, int X, int Y, const uint16_t * pText, uint32_t Color)
{
    //printf("TextW %08X (%d,%d), %08X\n", hFont, X, Y, Color);
    HdffCmdOsdDrawWideText(mOsdDev, hDisplay, hFont, X, Y, pText, Color);
}

void cHdffCmdIf::CmdOsdDrawBitmap(uint32_t hDisplay, int X, int Y, const uint8_t * pBitmap,
                                  int BmpWidth, int BmpHeight, int BmpSize,
                                  eColorType ColorType, uint32_t hPalette)
{
    //printf("Bitmap (%d,%d) %d x %d\n", X, Y, BmpWidth, BmpHeight);
    HdffCmdOsdDrawBitmap(mOsdDev, hDisplay, X, Y, pBitmap, BmpWidth, BmpHeight,
                         BmpSize, (HdffColorType_t) ColorType, hPalette);
}

void cHdffCmdIf::CmdOsdSaveRegion(uint32_t hDisplay, int X, int Y, int Width, int Height)
{
    HdffCmdOsdSaveRegion(mOsdDev, hDisplay, X, Y, Width, Height);
}

void cHdffCmdIf::CmdOsdRestoreRegion(uint32_t hDisplay)
{
    HdffCmdOsdRestoreRegion(mOsdDev, hDisplay);
}

void cHdffCmdIf::CmdMuxSetVideoOut(eVideoOut VideoOut)
{
    HdffCmdMuxSetVideoOut(mOsdDev, (HdffVideoOut_t) VideoOut);
}

void cHdffCmdIf::CmdMuxSetVolume(uint8_t Volume)
{
    HdffCmdMuxSetVolume(mOsdDev, Volume);
}

void cHdffCmdIf::CmdMuxMuteAudio(bool Mute)
{
    HdffCmdMuxMuteAudio(mOsdDev, Mute);
}

void cHdffCmdIf::CmdHdmiSetVideoMode(eHdmiVideoMode VideoMode)
{
    //printf("HdmiSetVideoMode %d\n", VideoMode);
    HdffCmdHdmiSetVideoMode(mOsdDev, (HdffVideoMode_t) VideoMode);
}

void cHdffCmdIf::CmdHdmiConfigure(const tHdmiConfig * pConfig)
{
    HdffHdmiConfig_t hdmiConfig;

    hdmiConfig.TransmitAudio = pConfig->TransmitAudio;
    hdmiConfig.ForceDviMode = pConfig->ForceDviMode;
    hdmiConfig.CecEnabled = pConfig->CecEnabled;
    hdmiConfig.VideoModeAdaption = (HdffVideoModeAdaption_t) pConfig->VideoModeAdaption;

    HdffCmdHdmiConfigure(mOsdDev, &hdmiConfig);
}

void cHdffCmdIf::CmdHdmiSendCecCommand(eCecCommand Command)
{
    HdffCmdHdmiSendCecCommand(mOsdDev, (HdffCecCommand_t) Command);
}

void cHdffCmdIf::CmdRemoteSetProtocol(eRemoteProtocol Protocol)
{
    //printf("%s %d\n", __func__, Protocol);
    HdffCmdRemoteSetProtocol(mOsdDev, (HdffRemoteProtocol_t) Protocol);
}

void cHdffCmdIf::CmdRemoteSetAddressFilter(bool Enable, uint32_t Address)
{
    //printf("%s %d %d\n", __func__, Enable, Address);
    HdffCmdRemoteSetAddressFilter(mOsdDev, Enable, Address);
}

} // end of namespace
