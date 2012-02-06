/*
 * dvbhddevice.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dvbhddevice.c 1.2 2010/01/01 15:00:53 kls Exp $
 */

#include <vdr/plugin.h>
#include <vdr/shutdown.h>
#include "dvbhdffdevice.h"
#include "setup.h"

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = trNOOP("HD Full Featured DVB device");

class cPluginDvbhddevice : public cPlugin {
private:
  cDvbHdFfDeviceProbe *probe;
  bool mIsUserInactive;
public:
  cPluginDvbhddevice(void);
  virtual ~cPluginDvbhddevice();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual void MainThreadHook(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginDvbhddevice::cPluginDvbhddevice(void)
:   mIsUserInactive(true)
{
  probe = new cDvbHdFfDeviceProbe;
}

cPluginDvbhddevice::~cPluginDvbhddevice()
{
  delete probe;
}

void cPluginDvbhddevice::MainThreadHook(void)
{
    bool isUserInactive = ShutdownHandler.IsUserInactive();
    if (isUserInactive != mIsUserInactive)
    {
        mIsUserInactive = isUserInactive;
        if (gHdffSetup.CecEnabled)
        {
            HDFF::cHdffCmdIf * hdffCmdIf = cDvbHdFfDevice::GetHdffCmdHandler();
            if (mIsUserInactive)
            {
                hdffCmdIf->CmdHdmiSendCecCommand(HDFF_CEC_COMMAND_TV_OFF);
            }
            else
            {
                hdffCmdIf->CmdHdmiSendCecCommand(HDFF_CEC_COMMAND_TV_ON);
            }
        }
    }
}

cMenuSetupPage *cPluginDvbhddevice::SetupMenu(void)
{
  return new cHdffSetupPage(cDvbHdFfDevice::GetHdffCmdHandler());
}

bool cPluginDvbhddevice::SetupParse(const char *Name, const char *Value)
{
  return gHdffSetup.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cPluginDvbhddevice); // Don't touch this!
