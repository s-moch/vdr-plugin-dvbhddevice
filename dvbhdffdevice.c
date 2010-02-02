/*
 * dvbhdffdevice.c: The DVB HD Full Featured device interface
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dvbhdffdevice.c 1.4 2010/01/04 12:53:47 kls Exp $
 */

#include "dvbhdffdevice.h"
#include <errno.h>
#include <limits.h>
#include <linux/videodev2.h>
#include <linux/dvb/audio.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/video.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "hdffosd.h"
#include "setup.h"
#include "vdr/eitscan.h"
#include "vdr/transfer.h"

// --- cDvbHdFfDevice ----------------------------------------------------------

int cDvbHdFfDevice::devHdffOffset = -1;

cDvbHdFfDevice::cDvbHdFfDevice(int Adapter, int Frontend)
:cDvbDevice(Adapter, Frontend)
{
  spuDecoder = NULL;
  digitalAudio = false;
  playMode = pmNone;
  mHdffCmdIf = NULL;

  // Devices that are only present on cards with decoders:

  fd_osd      = DvbOpen(DEV_DVB_OSD,    adapter, frontend, O_RDWR);
  fd_video    = DvbOpen(DEV_DVB_VIDEO,  adapter, frontend, O_RDWR | O_NONBLOCK);
  fd_audio    = DvbOpen(DEV_DVB_AUDIO,  adapter, frontend, O_RDWR | O_NONBLOCK);

  //TODO missing /dev/video offset calculation

  isHdffPrimary = false;
  if (devHdffOffset < 0) {
     devHdffOffset = adapter;
     isHdffPrimary = true;
     mHdffCmdIf = new HDFF::cHdffCmdIf(fd_osd);
     mHdffCmdIf->CmdHdmiSetVideoMode(gHdffSetup.GetVideoMode());
     }

  // Video format:

  SetVideoFormat(Setup.VideoFormat);
}

cDvbHdFfDevice::~cDvbHdFfDevice()
{
  delete spuDecoder;
  if (isHdffPrimary)
     delete mHdffCmdIf;
  // We're not explicitly closing any device files here, since this sometimes
  // caused segfaults. Besides, the program is about to terminate anyway...
}

void cDvbHdFfDevice::MakePrimaryDevice(bool On)
{
  if (On)
     new cHdffOsdProvider(mHdffCmdIf);
  cDvbDevice::MakePrimaryDevice(On);
}

bool cDvbHdFfDevice::HasDecoder(void) const
{
  return isHdffPrimary;
}

cSpuDecoder *cDvbHdFfDevice::GetSpuDecoder(void)
{
  if (!spuDecoder && IsPrimaryDevice())
     spuDecoder = new cDvbSpuDecoder();
  return spuDecoder;
}

uchar *cDvbHdFfDevice::GrabImage(int &Size, bool Jpeg, int Quality, int SizeX, int SizeY)
{
  //TODO
  return NULL;
}

void cDvbHdFfDevice::SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat)
{
  //TODO???
  cDevice::SetVideoDisplayFormat(VideoDisplayFormat);
}

void cDvbHdFfDevice::SetVideoFormat(bool VideoFormat16_9)
{
  HDFF::tVideoFormat videoFormat;
  videoFormat.AutomaticEnabled = true;
  videoFormat.AfdEnabled = true;
  videoFormat.TvFormat = (HDFF::eTvFormat) gHdffSetup.TvFormat;
  videoFormat.VideoConversion = (HDFF::eVideoConversion) gHdffSetup.VideoConversion;
  mHdffCmdIf->CmdAvSetVideoFormat(0, &videoFormat);
}

eVideoSystem cDvbHdFfDevice::GetVideoSystem(void)
{
  eVideoSystem VideoSystem = vsPAL;
  if (fd_video >= 0) {
     video_size_t vs;
     if (ioctl(fd_video, VIDEO_GET_SIZE, &vs) == 0) {
        if (vs.h == 480 || vs.h == 240)
           VideoSystem = vsNTSC;
        }
     else
        LOG_ERROR;
     }
  return VideoSystem;
}

void cDvbHdFfDevice::GetVideoSize(int &Width, int &Height, double &VideoAspect)
{
  if (fd_video >= 0) {
     video_size_t vs;
     if (ioctl(fd_video, VIDEO_GET_SIZE, &vs) == 0) {
        Width = vs.w;
        Height = vs.h;
        switch (vs.aspect_ratio) {
          default:
          case VIDEO_FORMAT_4_3:   VideoAspect =  4.0 / 3.0; break;
          case VIDEO_FORMAT_16_9:  VideoAspect = 16.0 / 9.0; break;
          case VIDEO_FORMAT_221_1: VideoAspect =       2.21; break;
          }
        return;
        }
     else
        LOG_ERROR;
     }
  cDevice::GetVideoSize(Width, Height, VideoAspect);
}

void cDvbHdFfDevice::GetOsdSize(int &Width, int &Height, double &PixelAspect)
{
  gHdffSetup.GetOsdSize(Width, Height, PixelAspect);
}

/*TODO obsolete?
bool cDvbHdFfDevice::SetAudioBypass(bool On)
{
  if (setTransferModeForDolbyDigital != 1)
     return false;
  return ioctl(fd_audio, AUDIO_SET_BYPASS_MODE, On) == 0;
}
TODO*/

bool cDvbHdFfDevice::SetPid(cPidHandle *Handle, int Type, bool On)
{
  if (Handle->pid) {
     dmx_pes_filter_params pesFilterParams;
     memset(&pesFilterParams, 0, sizeof(pesFilterParams));
     if (On) {
        if (Handle->handle < 0) {
           Handle->handle = DvbOpen(DEV_DVB_DEMUX, adapter, frontend, O_RDWR | O_NONBLOCK, true);
           if (Handle->handle < 0) {
              LOG_ERROR;
              return false;
              }
           }
        if (Type == ptPcr)
           mHdffCmdIf->CmdAvSetPcrPid(0, Handle->pid);
        else if (Type == ptVideo) {
           if (Handle->streamType == 0x1B)
              mHdffCmdIf->CmdAvSetVideoPid(0, Handle->pid, HDFF::videoStreamH264);
           else
              mHdffCmdIf->CmdAvSetVideoPid(0, Handle->pid, HDFF::videoStreamMpeg2);
           }
        else if (Type == ptAudio)
           mHdffCmdIf->CmdAvSetAudioPid(0, Handle->pid, HDFF::audioStreamMpeg1);
        else if (Type == ptDolby)
           mHdffCmdIf->CmdAvSetAudioPid(0, Handle->pid, HDFF::audioStreamAc3);
        if (!(Type <= ptDolby && Handle->used <= 1)) {
           pesFilterParams.pid     = Handle->pid;
           pesFilterParams.input   = DMX_IN_FRONTEND;
           pesFilterParams.output  = DMX_OUT_TS_TAP;
           pesFilterParams.pes_type= DMX_PES_OTHER;
           pesFilterParams.flags   = DMX_IMMEDIATE_START;
           if (ioctl(Handle->handle, DMX_SET_PES_FILTER, &pesFilterParams) < 0) {
              LOG_ERROR;
              return false;
              }
           }
        }
     else if (!Handle->used) {
        CHECK(ioctl(Handle->handle, DMX_STOP));
        if (Type == ptPcr)
           mHdffCmdIf->CmdAvSetPcrPid(0, 0);
        else if (Type == ptVideo)
           mHdffCmdIf->CmdAvSetVideoPid(0, 0, HDFF::videoStreamMpeg2);
        else if (Type == ptAudio)
           mHdffCmdIf->CmdAvSetAudioPid(0, 0, HDFF::audioStreamMpeg1);
        else if (Type == ptDolby)
           mHdffCmdIf->CmdAvSetAudioPid(0, 0, HDFF::audioStreamAc3);
        //TODO missing setting to 0x1FFF??? see cDvbDevice::SetPid()
        close(Handle->handle);
        Handle->handle = -1;
        }
     }
  return true;
}

void cDvbHdFfDevice::TurnOffLiveMode(bool LiveView)
{
  // Turn off live PIDs:

  DetachAll(pidHandles[ptAudio].pid);
  DetachAll(pidHandles[ptVideo].pid);
  DetachAll(pidHandles[ptPcr].pid);
  DetachAll(pidHandles[ptTeletext].pid);
  DelPid(pidHandles[ptAudio].pid);
  DelPid(pidHandles[ptVideo].pid);
  DelPid(pidHandles[ptPcr].pid, ptPcr);
  DelPid(pidHandles[ptTeletext].pid);
  DelPid(pidHandles[ptDolby].pid);
}

bool cDvbHdFfDevice::SetChannelDevice(const cChannel *Channel, bool LiveView)
{
  int apid = Channel->Apid(0);
  int vpid = Channel->Vpid();
  int dpid = Channel->Dpid(0);

  bool DoTune = !IsTunedToTransponder(Channel);

  bool pidHandlesVideo = pidHandles[ptVideo].pid == vpid;
  bool pidHandlesAudio = pidHandles[ptAudio].pid == apid;

  bool TurnOffLivePIDs = DoTune
                         || !IsPrimaryDevice()
                         || LiveView // for a new live view the old PIDs need to be turned off
                         || pidHandlesVideo // for recording the PIDs must be shifted from DMX_PES_AUDIO/VIDEO to DMX_PES_OTHER
                         ;

  bool StartTransferMode = IsPrimaryDevice() && !DoTune
                           && (LiveView && HasPid(vpid ? vpid : apid) && (!pidHandlesVideo || (!pidHandlesAudio && (dpid ? pidHandles[ptAudio].pid != dpid : true)))// the PID is already set as DMX_PES_OTHER
                              || !LiveView && (pidHandlesVideo || pidHandlesAudio) // a recording is going to shift the PIDs from DMX_PES_AUDIO/VIDEO to DMX_PES_OTHER
                              );
  if (CamSlot() && !ChannelCamRelations.CamDecrypt(Channel->GetChannelID(), CamSlot()->SlotNumber()))
     StartTransferMode |= LiveView && IsPrimaryDevice() && Channel->Ca() >= CA_ENCRYPTED_MIN;

  bool TurnOnLivePIDs = !StartTransferMode && LiveView;

  // Turn off live PIDs if necessary:

  if (TurnOffLivePIDs)
     TurnOffLiveMode(LiveView);

  // Set the tuner:

  if (!cDvbDevice::SetChannelDevice(Channel, LiveView))
     return false;

  // If this channel switch was requested by the EITScanner we don't wait for
  // a lock and don't set any live PIDs (the EITScanner will wait for the lock
  // by itself before setting any filters):

  if (EITScanner.UsesDevice(this)) //XXX
     return true;

  // PID settings:

  if (TurnOnLivePIDs) {
     //SetAudioBypass(false);//TODO obsolete?
     if (!(AddPid(Channel->Ppid(), ptPcr) && AddPid(vpid, ptVideo, Channel->Vtype()) && AddPid(apid, ptAudio))) {
        esyslog("ERROR: failed to set PIDs for channel %d on device %d", Channel->Number(), CardIndex() + 1);
        return false;
        }
     if (IsPrimaryDevice())
        AddPid(Channel->Tpid(), ptTeletext);//TODO obsolete?
     }
  else if (StartTransferMode)
     cControl::Launch(new cTransferControl(this, Channel));

  return true;
}

int cDvbHdFfDevice::GetAudioChannelDevice(void)
{
  audio_status_t as;
  CHECK(ioctl(fd_audio, AUDIO_GET_STATUS, &as));
  return as.channel_select;
}

void cDvbHdFfDevice::SetAudioChannelDevice(int AudioChannel)
{
  //CHECK(ioctl(fd_audio, AUDIO_SET_MIXER, &am));//TODO
}

void cDvbHdFfDevice::SetVolumeDevice(int Volume)
{
  if (digitalAudio)
     Volume = 0;
  audio_mixer_t am;
  // conversion for linear volume response:
  am.volume_left = am.volume_right = 2 * Volume - Volume * Volume / 255;
  //CHECK(ioctl(fd_audio, AUDIO_SET_MIXER, &am));//TODO
}

void cDvbHdFfDevice::SetDigitalAudioDevice(bool On)
{
  //TODO
  if (digitalAudio != On) {
     if (digitalAudio)
        cCondWait::SleepMs(1000); // Wait until any leftover digital data has been flushed
     digitalAudio = On;
     SetVolumeDevice(On || IsMute() ? 0 : CurrentVolume());
     }
}

void cDvbHdFfDevice::SetAudioTrackDevice(eTrackType Type)
{
  printf("SetAudioTrackDevice %d\n", Type);
  const tTrackId *TrackId = GetTrack(Type);
  if (TrackId && TrackId->id) {
     if (IS_AUDIO_TRACK(Type)) {
        if (pidHandles[ptAudio].pid && pidHandles[ptAudio].pid != TrackId->id) {
           DetachAll(pidHandles[ptAudio].pid);
           if (CamSlot())
              CamSlot()->SetPid(pidHandles[ptAudio].pid, false);
           pidHandles[ptAudio].pid = TrackId->id;
           SetPid(&pidHandles[ptAudio], ptAudio, true);
           if (CamSlot()) {
              CamSlot()->SetPid(pidHandles[ptAudio].pid, true);
              CamSlot()->StartDecrypting();
              }
           }
	}
     else if (IS_DOLBY_TRACK(Type)) {
        pidHandles[ptDolby].pid = TrackId->id;
        SetPid(&pidHandles[ptDolby], ptDolby, true);
        }
     }
}

bool cDvbHdFfDevice::CanReplay(void) const
{
  return cDevice::CanReplay();
}

bool cDvbHdFfDevice::SetPlayMode(ePlayMode PlayMode)
{
  if (PlayMode == pmNone) {
     mHdffCmdIf->CmdAvEnableVideoAfterStop(0, false);
     mHdffCmdIf->CmdAvSetPcrPid(0, 0);
     mHdffCmdIf->CmdAvSetVideoPid(0, 0, HDFF::videoStreamMpeg2);
     mHdffCmdIf->CmdAvSetAudioPid(0, 0, HDFF::audioStreamMpeg1);

     ioctl(fd_video, VIDEO_SELECT_SOURCE, VIDEO_SOURCE_DEMUX);
     mHdffCmdIf->CmdAvSetDemultiplexerInput(0, 0);
     mHdffCmdIf->CmdAvEnableSync(0, true);
     }
  else {
     if (playMode == pmNone)
        TurnOffLiveMode(true);

     mHdffCmdIf->CmdAvSetStc(0, 100000);
     mHdffCmdIf->CmdAvEnableSync(0, true);
     mHdffCmdIf->CmdAvEnableVideoAfterStop(0, true);

     playVideoPid = -1;
     playAudioPid = -1;

     mHdffCmdIf->CmdAvSetDemultiplexerInput(0, 2);
     ioctl(fd_video, VIDEO_SELECT_SOURCE, VIDEO_SOURCE_MEMORY);
     }
  playMode = PlayMode;
  return true;
}

int64_t cDvbHdFfDevice::GetSTC(void)
{
  if (fd_video >= 0) {
     uint64_t pts;
     if (ioctl(fd_video, VIDEO_GET_PTS, &pts) == -1) {
        esyslog("ERROR: pts %d: %m", CardIndex() + 1);
        return -1;
        }
     return pts;
     }
  if (fd_audio >= 0) {
     uint64_t pts;
     if (ioctl(fd_audio, AUDIO_GET_PTS, &pts) == -1) {
        esyslog("ERROR: pts %d: %m", CardIndex() + 1);
        return -1;
        }
     return pts;
     }
  return -1;
}

void cDvbHdFfDevice::TrickSpeed(int Speed)
{
  mHdffCmdIf->CmdAvEnableSync(0, false);
  mHdffCmdIf->CmdAvSetAudioPid(0, 0, HDFF::audioStreamMpeg1);
  playAudioPid = -1;
  if (Speed > 0)
     mHdffCmdIf->CmdAvSetVideoSpeed(0, 100 / Speed);
}

void cDvbHdFfDevice::Clear(void)
{
  CHECK(ioctl(fd_video, VIDEO_CLEAR_BUFFER));
  mHdffCmdIf->CmdAvSetVideoPid(0, 0, HDFF::videoStreamMpeg1);
  mHdffCmdIf->CmdAvSetAudioPid(0, 0, HDFF::audioStreamMpeg1);
  playVideoPid = -1;
  playAudioPid = -1;
  cDevice::Clear();
}

void cDvbHdFfDevice::Play(void)
{
  mHdffCmdIf->CmdAvEnableSync(0, true);
  mHdffCmdIf->CmdAvSetVideoSpeed(0, 100);
  mHdffCmdIf->CmdAvSetAudioSpeed(0, 100);
  cDevice::Play();
}

void cDvbHdFfDevice::Freeze(void)
{
  mHdffCmdIf->CmdAvSetVideoSpeed(0, 0);
  mHdffCmdIf->CmdAvSetAudioSpeed(0, 0);
  cDevice::Freeze();
}

void cDvbHdFfDevice::Mute(void)
{
  //TODO???
  cDevice::Mute();
}

static HDFF::eVideoStreamType MapVideoStreamTypes(int Vtype)
{
  switch (Vtype) {
    case 0x01: return HDFF::videoStreamMpeg1;
    case 0x02: return HDFF::videoStreamMpeg2;
    case 0x1B: return HDFF::videoStreamH264;
    default: return HDFF::videoStreamMaxValue; // there is no HDFF::videoStreamNone
    }
}

void cDvbHdFfDevice::StillPicture(const uchar *Data, int Length)
{
  if (!Data || Length < TS_SIZE)
     return;
  if (Data[0] == 0x47) {
     // TS data
     cDevice::StillPicture(Data, Length);
     }
  else if (Data[0] == 0x00 && Data[1] == 0x00 && Data[2] == 0x01 && (Data[3] & 0xF0) == 0xE0) {
     // PES data
     char *buf = MALLOC(char, Length);
     if (!buf)
        return;
     int i = 0;
     int blen = 0;
     while (i < Length - 6) {
           if (Data[i] == 0x00 && Data[i + 1] == 0x00 && Data[i + 2] == 0x01) {
              int len = Data[i + 4] * 256 + Data[i + 5];
              if ((Data[i + 3] & 0xF0) == 0xE0) { // video packet
                 // skip PES header
                 int offs = i + 6;
                 // skip header extension
                 if ((Data[i + 6] & 0xC0) == 0x80) {
                    // MPEG-2 PES header
                    if (Data[i + 8] >= Length)
                       break;
                    offs += 3;
                    offs += Data[i + 8];
                    len -= 3;
                    len -= Data[i + 8];
                    if (len < 0 || offs + len > Length)
                       break;
                    }
                 else {
                    // MPEG-1 PES header
                    while (offs < Length && len > 0 && Data[offs] == 0xFF) {
                          offs++;
                          len--;
                          }
                    if (offs <= Length - 2 && len >= 2 && (Data[offs] & 0xC0) == 0x40) {
                       offs += 2;
                       len -= 2;
                       }
                    if (offs <= Length - 5 && len >= 5 && (Data[offs] & 0xF0) == 0x20) {
                       offs += 5;
                       len -= 5;
                       }
                    else if (offs <= Length - 10 && len >= 10 && (Data[offs] & 0xF0) == 0x30) {
                       offs += 10;
                       len -= 10;
                       }
                    else if (offs < Length && len > 0) {
                       offs++;
                       len--;
                       }
                    }
                 if (blen + len > Length) // invalid PES length field
                    break;
                 memcpy(&buf[blen], &Data[offs], len);
                 i = offs + len;
                 blen += len;
                 }
              else if (Data[i + 3] >= 0xBD && Data[i + 3] <= 0xDF) // other PES packets
                 i += len + 6;
              else
                 i++;
              }
           else
              i++;
           }
     mHdffCmdIf->CmdAvShowStillImage(0, (uint8_t *)buf, blen, MapVideoStreamTypes(PatPmtParser()->Vtype()));
     free(buf);
     }
  else {
     // non-PES data
     mHdffCmdIf->CmdAvShowStillImage(0, Data, Length, MapVideoStreamTypes(PatPmtParser()->Vtype()));
     }
}

bool cDvbHdFfDevice::Poll(cPoller &Poller, int TimeoutMs)
{
  Poller.Add(fd_video, true);
  return Poller.Poll(TimeoutMs);
}

bool cDvbHdFfDevice::Flush(int TimeoutMs)
{
  //TODO actually this function should wait until all buffered data has been processed by the card, but how?
  return true;
}

int cDvbHdFfDevice::PlayVideo(const uchar *Data, int Length)
{
  return WriteAllOrNothing(fd_video, Data, Length, 1000, 10);
}

int cDvbHdFfDevice::PlayAudio(const uchar *Data, int Length, uchar Id)
{
  return WriteAllOrNothing(fd_audio, Data, Length, 1000, 10);
}

int cDvbHdFfDevice::PlayTsVideo(const uchar *Data, int Length)
{
  int pid = TsPid(Data);
  if (pid != playVideoPid) {
     playVideoPid = pid;
     PatPmtParser();
     mHdffCmdIf->CmdAvSetVideoPid(0, playVideoPid, MapVideoStreamTypes(PatPmtParser()->Vtype()));
     }
  return WriteAllOrNothing(fd_video, Data, Length, 1000, 10);
}

static HDFF::eAudioStreamType MapAudioStreamTypes(int Atype)
{
  switch (Atype) {
    case 0x03: return HDFF::audioStreamMpeg1;
    case 0x04: return HDFF::audioStreamMpeg2;
    case 0x06: return HDFF::audioStreamAc3;
    default: return HDFF::audioStreamMaxValue; // there is no HDFF::audioStreamNone
    }
}

int cDvbHdFfDevice::PlayTsAudio(const uchar *Data, int Length)
{
  int pid = TsPid(Data);
  if (pid != playAudioPid) {
     playAudioPid = pid;
     int AudioStreamType = -1;
     for (int i = 0; PatPmtParser()->Apid(i); i++) {
         if (playAudioPid == PatPmtParser()->Apid(i)) {
            AudioStreamType = PatPmtParser()->Atype(i);
            break;
            }
         }
     if (AudioStreamType < 0) {
        for (int i = 0; PatPmtParser()->Dpid(i); i++) {
            if (playAudioPid == PatPmtParser()->Dpid(i)) {
               AudioStreamType = PatPmtParser()->Dtype(i);
               break;
               }
            }
        }
     mHdffCmdIf->CmdAvSetAudioPid(0, playAudioPid, MapAudioStreamTypes(AudioStreamType));
     }
  return WriteAllOrNothing(fd_video, Data, Length, 1000, 10);
}

HDFF::cHdffCmdIf *cDvbHdFfDevice::GetHdffCmdHandler(void)
{
  //TODO why not just keep a pointer?
  if (devHdffOffset >= 0) {
     cDvbHdFfDevice *device = (cDvbHdFfDevice *)GetDevice(devHdffOffset);
     if (device)
        return device->mHdffCmdIf;
     }
  return NULL;
}

// --- cDvbHdFfDeviceProbe ---------------------------------------------------

bool cDvbHdFfDeviceProbe::Probe(int Adapter, int Frontend)
{
  static uint32_t SubsystemIds[] = {
    0x13C23009, // Technotrend S2-6400 HDFF
    0x00000000
    };
  cString FileName;
  cReadLine ReadLine;
  FILE *f = NULL;
  uint32_t SubsystemId = 0;
  FileName = cString::sprintf("/sys/class/dvb/dvb%d.frontend%d/device/subsystem_vendor", Adapter, Frontend);
  if ((f = fopen(FileName, "r")) != NULL) {
     if (char *s = ReadLine.Read(f))
        SubsystemId = strtoul(s, NULL, 0) << 16;
     fclose(f);
     }
  FileName = cString::sprintf("/sys/class/dvb/dvb%d.frontend%d/device/subsystem_device", Adapter, Frontend);
  if ((f = fopen(FileName, "r")) != NULL) {
     if (char *s = ReadLine.Read(f))
        SubsystemId |= strtoul(s, NULL, 0);
     fclose(f);
     }
  for (uint32_t *sid = SubsystemIds; *sid; sid++) {
      if (*sid == SubsystemId) {
         FileName = cString::sprintf("/dev/dvb/adapter%d/osd0", Adapter);
         int fd = open(FileName, O_RDWR);
         if (fd != -1) { //TODO treat the second path of the S2-6400 as a budget device
            close(fd);
            dsyslog("creating cDvbHdFfDevice");
            new cDvbHdFfDevice(Adapter, Frontend);
            return true;
            }
         }
      }
  return false;
}
