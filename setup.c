/*
 * setup.c: Setup for the DVB HD Full Featured On Screen Display
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: setup.c 1.1 2009/12/29 14:30:45 kls Exp $
 */

#include "setup.h"
#include "hdffcmd.h"

const int kResolutions = 4;
const int kTvFormats = 2;
const int kVideoConversions = 6;
const int kOsdSizes = 5;

const int kResolution1080i = 0;
const int kResolution720p = 1;
const int kResolution576p = 2;
const int kResolution576i = 3;

static const char * ResolutionItems[] =
{
    "1080i",
    "720p",
    "576p",
    "576i",
    NULL
};

static const char * TvFormatItems[] =
{
    "4/3",
    "16/9",
    NULL
};

static const char * VideoConversionItems[] =
{
    "Automatic",
    "Letterbox 16/9",
    "Letterbox 14/9",
    "Pillarbox",
    "CentreCutOut",
    "Always 16/9",
    NULL
};

static const char * OsdSizeItems[] =
{
    "Follow resolution",
    "1920x1080",
    "1280x720",
    "1024x576",
    "720x576",
    NULL
};

cHdffSetup gHdffSetup;

cHdffSetup::cHdffSetup(void)
{
    Resolution = kResolution1080i;
    TvFormat = HDFF::tvFormat16by9;
    VideoConversion = HDFF::videoConversionPillarbox;
    OsdSize = 0;
}

bool cHdffSetup::SetupParse(const char *Name, const char *Value)
{
    if      (strcmp(Name, "Resolution")      == 0) Resolution      = atoi(Value);
    else if (strcmp(Name, "TvFormat")        == 0) TvFormat        = atoi(Value);
    else if (strcmp(Name, "VideoConversion") == 0) VideoConversion = atoi(Value);
    else if (strcmp(Name, "OsdSize")         == 0) OsdSize         = atoi(Value);
    else return false;
    return true;
}

void cHdffSetup::GetOsdSize(int &Width, int &Height, double &PixelAspect)
{
    if (OsdSize == 0) {
        if (Resolution == kResolution1080i) {
            Width = 1920;
            Height = 1080;
        }
        else if (Resolution == kResolution720p) {
            Width = 1280;
            Height = 720;
        }
        else {
            Width = 720;
            Height = 576;
        }
        if (TvFormat == HDFF::tvFormat16by9)
            PixelAspect = 16.0 / 9.0;
        else
            PixelAspect = 4.0 / 3.0;
    }
    else if (OsdSize == 1) {
        Width = 1920;
        Height = 1080;
        PixelAspect = 16.0 / 9.0;
    }
    else if (OsdSize == 2) {
        Width = 1280;
        Height = 720;
        PixelAspect = 16.0 / 9.0;
    }
    else if (OsdSize == 3) {
        Width = 1024;
        Height = 576;
        PixelAspect = 16.0 / 9.0;
    }
    else {
        Width = 720;
        Height = 576;
        PixelAspect = 4.0 / 3.0;
    }
}

HDFF::eHdmiVideoMode cHdffSetup::GetVideoMode(void)
{
    switch (Resolution)
    {
        case kResolution1080i:
        default:
            return HDFF::videoMode1080i50;
        case kResolution720p:
            return HDFF::videoMode720p50;
        case kResolution576p:
            return HDFF::videoMode576p50;
        case kResolution576i:
            return HDFF::videoMode576i50;
    }
}

cHdffSetupPage::cHdffSetupPage(HDFF::cHdffCmdIf * pHdffCmdIf)
{
    mHdffCmdIf = pHdffCmdIf;
    mNewHdffSetup = gHdffSetup;

    Add(new cMenuEditStraItem("Resolution", &mNewHdffSetup.Resolution, kResolutions, ResolutionItems));
    Add(new cMenuEditStraItem("TV format", &mNewHdffSetup.TvFormat, kTvFormats, TvFormatItems));
    Add(new cMenuEditStraItem("Video Conversion", &mNewHdffSetup.VideoConversion, kVideoConversions, VideoConversionItems));
    Add(new cMenuEditStraItem("Osd Size", &mNewHdffSetup.OsdSize, kOsdSizes, OsdSizeItems));
}

cHdffSetupPage::~cHdffSetupPage(void)
{
}

void cHdffSetupPage::Store(void)
{
    SetupStore("Resolution", mNewHdffSetup.Resolution);
    SetupStore("TvFormat", mNewHdffSetup.TvFormat);
    SetupStore("VideoConversion", mNewHdffSetup.VideoConversion);
    SetupStore("OsdSize", mNewHdffSetup.OsdSize);

    if (mHdffCmdIf)
    {
        if (mNewHdffSetup.Resolution != gHdffSetup.Resolution)
        {
            mHdffCmdIf->CmdHdmiSetVideoMode(gHdffSetup.GetVideoMode());
        }
        HDFF::tVideoFormat videoFormat;

        videoFormat.AutomaticEnabled = true;
        videoFormat.AfdEnabled = true;
        videoFormat.TvFormat = (HDFF::eTvFormat) mNewHdffSetup.TvFormat;
        videoFormat.VideoConversion = (HDFF::eVideoConversion) mNewHdffSetup.VideoConversion;
        mHdffCmdIf->CmdAvSetVideoFormat(0, &videoFormat);
    }

    gHdffSetup = mNewHdffSetup;
}
