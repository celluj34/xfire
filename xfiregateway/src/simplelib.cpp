#include "simplelib.h"

#include <string>
#include <ctype.h>

namespace xfiregateway {

  std::string SimpleLib::stringToLower(std::string myString)
    {
      for( std::string::iterator it = myString.begin();
	   it != myString.end() ;
	   it++ ) {
	*it = tolower(*it);
      }
      return myString;
      /*
      const char *str = myString.c_str();
      tolower(str);
      return std::string(str);
      */
      /*
      static ToLower down(std::locale::classic());
      std::transform(myString.begin(), myString.end(), myString.begin(), down);
      return myString;
      */
    }


};
