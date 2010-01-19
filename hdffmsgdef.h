/*
 * hdffmsgdef.h: TODO(short description)
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: hdffmsgdef.h 1.1 2009/12/29 14:27:22 kls Exp $
 */

#ifndef _HDFF_MSGDEF_H_
#define _HDFF_MSGDEF_H_

#define MAX_CMD_LEN 1536

namespace HDFF
{

typedef enum _eMessageType
{
    msgTypeCommand,
    msgTypeAnswer,
    msgTypeResult,
    msgTypeEvent
} eMessageType;

typedef enum _eMessageGroup
{
    msgGroupGeneric,
    msgGroupAvDec,
    msgGroupAvMux,
    msgGroupFrontend,
    msgGroupOsd,
    msgGroupHdmi,
    msgGroupRemoteControl
} eMessageGroup;

typedef enum _eMessageId
{
    msgGenGetFirmwareVersion = 0,
    msgGenGetInterfaceVersion,
    msgGenGetCopyrights,

    msgAvSetAudioPid = 0,
    msgAvSetVideoPid,
    msgAvSetPcrPid,
    msgAvSetTeletextPid,
    msgAvShowStillImage,
    msgAvSetVideoWindow,
    msgAvSetDecoderInput,
    msgAvSetDemultiplexerInput,
    msgAvSetVideoFormat,
    msgAvSetVideoOutputMode,
    msgAvSetStc,
    msgAvFlushBuffer,
    msgAvEnableSync,
    msgAvSetVideoSpeed,
    msgAvSetAudioSpeed,
    msgAvEnableVideoAfterStop,
    msgAvGetVideoFormatInfo,

    msgMuxSetVideoOut = 0,
    msgMuxSetSlowBlank,
    msgMuxSetFastBlank,
    msgMuxSetVolume,
    msgMuxSetAudioMute,

    msgOsdConfigure = 0,
    msgOsdCreateDisplay = 10,
    msgOsdDeleteDisplay,
    msgOsdEnableDisplay,
    msgOsdSetDisplayOutputRectangle,
    msgOsdSetDisplayClippingArea,
    msgOsdRenderDisplay,
    msgOsdSaveRegion,
    msgOsdRestoreRegion,
    msgOsdCreatePalette = 30,
    msgOsdDeletePalette,
    msgOsdSetDisplayPalette,
    msgOsdSetPaletteColors,
    msgOsdCreateFontFace = 50,
    msgOsdDeleteFontFace,
    msgOsdCreateFont,
    msgOsdDeleteFont,
    msgOsdDrawPixel = 70,
    msgOsdDrawRectangle,
    msgOsdDrawCircle,
    msgOsdDrawEllipse,
    msgOsdDrawSlope,
    msgOsdDrawText,
    msgOsdDrawTextW,
    msgOsdDrawBitmap,

    msgHdmiEnableOutput = 0,
    msgHdmiSetVideoMode,
    msgHdmiConfigureOutput,
    msgHdmiIsDisplayConnected,
    msgHdmiGetDisplayInfo,

    msgRemoteSetProtocol = 0,
    msgRemoteSetAddressFilter,
    msgRemoteKeyEvent
} eMessageId;


// AvDec definitions

typedef enum _eAudioStreamType
{
    audioStreamMpeg1,
    audioStreamMpeg2,
    audioStreamAc3,
    audioStreamAac,
    audioStreamHeAac,
    audioStreamPcm,
    audioStreamMaxValue
} eAudioStreamType;

typedef enum _eVideoStreamType
{
    videoStreamMpeg1,
    videoStreamMpeg2,
    videoStreamH264,
    videoStreamMpeg4Asp,
    videoStreamVc1,
    videoStreamMaxValue
} eVideoStreamType;


typedef enum _eTvFormat
{
    tvFormat4by3,
    tvFormat16by9,
    tvFormatMaxValue
} eTvFormat;

typedef enum _eVideoConversion
{
    videoConversionAutomatic,
    videoConversionLetterbox16by9,
    videoConversionLetterbox14by9,
    videoConversionPillarbox,
    videoConversionCentreCutOut,
    videoConversionAlways16by9,
    videoConversionMaxValue
} eVideoConversion;

typedef struct _tVideoFormat
{
    bool AutomaticEnabled;
    bool AfdEnabled;
    eTvFormat TvFormat;
    eVideoConversion VideoConversion;
} tVideoFormat;

typedef enum _eVideoOutputMode
{
    videoOutputClone,
    videoOutputDualView,
    videoOutputMaxValue
} eVideoOutputMode;

// AvMux definitions

typedef enum _eVideoOut
{
    videoOutDisabled,
    videoOutRgb,
    videoOutCvbs,
    videoOutYc,
    videoOutMaxValue
} eVideoOut;

typedef enum _eSlowBlank
{
    slowBlankOff,
    slowBlank16by9,
    slowBlank4by3,
    slowBlankMaxValue
} eSlowBlank;

typedef enum _eFastBlank
{
    fastBlankCvbs,
    fastBlankRgb,
    fastBlankMaxValue
} eFastBlank;


// OSD definitions

#define InvalidHandle           0xFFFFFFFF
#define ScreenDisplayHandle     0xFFFFFFFE

#define PositionScreenCentered  0xFFFF

#define SizeFullScreen    0xFFFF
#define SizeSameAsSource  0xFFFE

#define FontFaceTiresias  0x00000000

typedef enum _eColorType
{
    colorTypeClut1,
    colorTypeClut2,
    colorTypeClut4,
    colorTypeClut8,
    colorTypeARGB8888,
    colorTypeARGB8565,
    colorTypeARGB4444,
    colorTypeARGB1555,
} eColorType;

typedef enum _eColorFormat
{
    colorFormatARGB,
    colorFormatACbYCr,
} eColorFormat;

typedef enum _eDrawingFlags
{
    drawFull,
    drawHalfTop,
    drawHalfLeft,
    drawHalfBottom,
    drawHalfRight,
    drawQuarterTopLeft,
    drawQuarterTopRight,
    drawQuarterBottomLeft,
    drawQuarterBottomRight,
    drawQuarterTopLeftInverted,
    drawQuarterTopRightInverted,
    drawQuarterBottomLeftInverted,
    drawQuarterBottomRightInverted
} eDrawingFlags;


// HDMI definitions

typedef enum _eHdmiVideoMode
{
    videoMode576p50 = 18,
    videoMode720p50 = 19,
    videoMode1080i50 = 20,
    videoMode576i50 = 22,
    videoModeMaxValue
} eHdmiVideoMode;


// Remote control definitions

typedef enum _eRemoteProtocol
{
    remoteProtocolNone,
    remoteProtocolRc5,
    remoteProtocolRc6,
    remoteProtocolMaxValue
} eRemoteProtocol;

} // end of namespace

#endif
