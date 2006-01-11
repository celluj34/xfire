#ifndef __XFIREPACKETCONTENT_H
#define __XFIREPACKETCONTENT_H


namespace xfirelib {

  class XFirePacketContent {
  public:
    XFirePacketContent();

    virtual XFirePacketContent* newPacket() = 0;

    virtual int getPacketContent(char *buf) = 0;
    virtual int getPacketId() = 0;
    virtual int getPacketAttributeCount() = 0;
    virtual int getPacketSize() = 0;
    virtual void parseContent(char *buf, int length, int numberOfAtts) = 0;
  };

};


#endif
