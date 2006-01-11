#include <string>

#ifndef __VARIABVLEVALUE_H
#define __VARIABVLEVALUE_H

namespace xfirelib {

class VariableValue {
 public:
    VariableValue();
    void setName(std::string name);
    void setValueLength(int valueLength);
    void setValue( char * value );
    void setValue( std::string value );
    void setValue( const char *value, int valueLength );
    void setValueFromLong( long value, int bytes );
    std::string getName();
    int getValueLength();
    char* getValue();

    int readName(char *packet, int index);
    int readValue(char *packet, int index, int length = -1, int ignoreZeroAfterLength = 0);
    int readVariableValue(char *packet, int index, int packetLength);
    int VariableValue::readFixValue(char *packet, int index, int packetLength, int valueLength);

    int writeName(char *buf, int index);
    int writeValue(char *buf, int index);

    long getValueAsLong();
  private:
    long myPow(int x, int y);

    std::string name;
    char *value;
    int valueLength;
};

};

#endif
