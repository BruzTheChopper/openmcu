
#include "precompile.h"
#include "mcu.h"
#include "yuv.h"

extern unsigned char preMediaFrameData[];
extern unsigned preMediaFrameData_width, preMediaFrameData_height;
unsigned char
  *preMediaFrame = NULL,
  *logoFrame = NULL,
  *offlineFrame = NULL,
  *emptyFrame   = NULL,
  *backFrame    = NULL,
  *noVideoFrame = NULL;
unsigned
  preMediaFrame_width, preMediaFrame_height,
  logoFrame_width, logoFrame_height,
  offlineFrame_width, offlineFrame_height,
  emptyFrame_width, emptyFrame_height,
  backFrame_width, backFrame_height,
  noVideoFrame_width, noVideoFrame_height;
PMutex
  preMediaFrame_mutex,
  logoFrame_mutex,
  offlineFrame_mutex,
  emptyFrame_mutex,
  backFrame_mutex,
  noVideoFrame_mutex;

//////////////////////////////////////////////////////////////////////////////////////////////

class MyMCU : public QazMeet
{
  public:
    MyMCU()
    { }

    static MyMCU & Current()
    { return (MyMCU &)PProcess::Current(); }

#if MCU_VIDEO
    void * GetPreMediaFrame(unsigned & w, unsigned & h);
    void * GetLogoFramePointer(unsigned & w, unsigned & h);
    void * GetOfflineFramePointer(unsigned & w, unsigned & h);
    void * GetEmptyFramePointer(unsigned & w, unsigned & h);
    void * GetBackgroundPointer(unsigned & w, unsigned & h);
    void * GetNoVideoFramePointer(unsigned & w, unsigned & h);
    void * LoadFrame(PString fileName, unsigned & width, unsigned & height, BOOL allowLogo);

#endif // MCU_VIDEO

  protected:
};

//////////////////////////////////////////////////////////////////////////////////////////////

#if MCU_VIDEO

void * MyMCU::GetPreMediaFrame(unsigned & w, unsigned & h)
{
  if(preMediaFrame == NULL)
  {
    PWaitAndSignal m(preMediaFrame_mutex);
    if(preMediaFrame == NULL)
    {
      preMediaFrame_width = CIF_WIDTH;
      preMediaFrame_height = CIF_HEIGHT;
      BYTE *new_preMediaFrame = new BYTE [preMediaFrame_width*preMediaFrame_height*3/2];
      FillYUVFrame(new_preMediaFrame, 0, 0, 0, preMediaFrame_width, preMediaFrame_height);
      CopyRectIntoFrame(&preMediaFrameData, new_preMediaFrame,
            (preMediaFrame_width - preMediaFrameData_width) / 2,
                (preMediaFrame_height - preMediaFrameData_height) / 2,
            preMediaFrameData_width, preMediaFrameData_height,
            preMediaFrame_width, preMediaFrame_height);
      preMediaFrame = new_preMediaFrame;
    }
  }
  w = preMediaFrame_width;
  h = preMediaFrame_height;
  PTRACE(5, "MCU\tGetPreMediaFrame:" << preMediaFrame_width << "x" << preMediaFrame_height);
  return preMediaFrame;
}

void * MyMCU::GetLogoFramePointer(unsigned & w, unsigned & h)
{
  PWaitAndSignal m(logoFrame_mutex);
  if(!logoFrame)
  {
    if(logoFilename != "")
      logoFrame = (unsigned char*)LoadFrame(logoFilename, logoFrame_width, logoFrame_height, TRUE);
    else
      logoFrame = (unsigned char*)GetPreMediaFrame(logoFrame_width, logoFrame_height);
  }
  w = logoFrame_width;
  h = logoFrame_height;
  return logoFrame;
}

void * MyMCU::GetOfflineFramePointer(unsigned & w, unsigned & h)
{
  PWaitAndSignal m(offlineFrame_mutex);
  if(!offlineFrame) offlineFrame=(unsigned char*)LoadFrame("offlineFrame.jpg", offlineFrame_width, offlineFrame_height, FALSE);
  w = offlineFrame_width;
  h = offlineFrame_height;
  return offlineFrame;
}

void * MyMCU::GetEmptyFramePointer(unsigned & w, unsigned & h)
{
  PWaitAndSignal m(emptyFrame_mutex);
  if(!emptyFrame) emptyFrame=(unsigned char*)LoadFrame("emptySubframe.jpg", emptyFrame_width, emptyFrame_height, FALSE);
  w = emptyFrame_width;
  h = emptyFrame_height;
  return emptyFrame;
}

void * MyMCU::GetBackgroundPointer(unsigned & w, unsigned & h)
{
  PWaitAndSignal m(backFrame_mutex);
  if(!backFrame) backFrame=(unsigned char*)LoadFrame("background.jpg", backFrame_width, backFrame_height, FALSE);
  w = backFrame_width;
  h = backFrame_height;
  return backFrame;
}

void * MyMCU::GetNoVideoFramePointer(unsigned & w, unsigned & h)
{
  PWaitAndSignal m(noVideoFrame_mutex);
  if(!noVideoFrame) noVideoFrame=(unsigned char*)LoadFrame("noVideoFrame.jpg", noVideoFrame_width, noVideoFrame_height, TRUE);
  w = noVideoFrame_width;
  h = noVideoFrame_height;
  return noVideoFrame;
}

void * MyMCU::LoadFrame(PString fileName, unsigned & width, unsigned & height, BOOL allowLogo)
{
  void * buffer = NULL;
  PString filename = PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + fileName;
  if(PFile::Exists(filename))
  {
    int frame_size = 5000000, frame_width, frame_height;
    MCUBuffer frame_buffer(frame_size);
    if(MCU_AVDecodeFrameFromFile(filename, frame_buffer.GetPointer(), frame_size, frame_width, frame_height))
    {
      width = frame_width;
      height = frame_height;
      unsigned check_size = width*height*3/2;
      if((unsigned)frame_size != check_size)
      {
        PTRACE(1, "LoadFrame\tSize check fail: " << frame_size << " != " << frame_width << "*" << frame_height << "*3/2");
        if(check_size<(unsigned)frame_size) check_size=frame_size;
      }
      buffer = new unsigned char[check_size];
      memcpy(buffer, frame_buffer.GetPointer(), frame_size);
    }
  }
  if(!buffer)
  {
    if(allowLogo)
    {
      buffer = GetPreMediaFrame(width, height);
    }
    else
    {
      width=height=16;
      unsigned amount = width*height*3/2;
      buffer = new unsigned char[amount];
      FillYUVFrame(buffer, 0, 0, 0, width, height);
    }
  }
  return buffer;
}

#endif // MCU_VIDEO

PCREATE_PROCESS(MyMCU);

//////////////////////////////////////////////////////////////////////////////////////////////

#include "preMediaFrameData.h"

// End of File ///////////////////////////////////////////////////////////////
