/*
 * Copied from the gloox library to be used in the 
 * GOIM xfire gateway to parse a file.
 */

/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef PARSER_H__
#define PARSER_H__

#include <iksemel.h>
#include <gloox/gloox.h>
#include <gloox/stanza.h>
#include <string>

namespace xfiregateway
{
  using namespace gloox;

  class gloox::Stanza;

  /**
   * @brief This class is an abstraction of libiksemel's XML parser.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class Parser
  {
    public:
      /**
       * Describes the return values of the parser.
       */
      enum ParserState
      {
        PARSER_OK,                     /**< Everything's alright. */
        PARSER_NOMEM,                  /**< Memory allcation error. */
        PARSER_BADXML                  /**< XML parse error. */
      };

      /**
       * Constructs a new Parser object.
       * @param parent The object to send incoming Tags to.
       */
      Parser(  );

      /**
       * Virtual destructor.
       */
      virtual ~Parser();

      /**
       * Use this function to feed the parser with more XML.
       * @param data Raw xml to parse.
       * @return The return value indicates success or failure of the parsing.
       */
      ParserState feed( const std::string& data );
      virtual void streamEvent( Stanza *stanza ) = 0;

    private:

      iksparser *m_parser;
      Stanza *m_current;
      Stanza *m_root;

      friend int cdataHook( Parser *parser, char *data, size_t len );
      friend int tagHook( Parser *parser, char *name, char **atts, int type );
  };

}

#endif // PARSER_H__
