/*
 * gaim - Xfire Protocol Plugin
 *
 * Copyright (C) 2000-2001, Beat Wolf <asraniel@fryx.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/ 

#include "internal.h"
#include "x_utils.h"
#include "blist.h"
#include "debug.h"
#include "xmlnode.h"
#include "x_games.h"


/* global reference for handling game to name conversion */
xmlnode *xfire_games_xmldoc = NULL;
/* global reference for handling game launch information */
xmlnode *xfire_linfo_xmldoc = NULL;


/**
 * Parses XML file to convert xfire game ID's to names
 *
 * @param filename		The filename to parse (xml)
 *
 * @return TRUE if parsed ok, FALSE otherwise
*/ 
gboolean
xfire_parse_games(const char *filename)
{
	xmlnode *node = NULL;
	GError *error = NULL;
	gchar *contents = NULL;
	gsize length;

	gaim_debug(GAIM_DEBUG_INFO, "xfire games import",
			   "Reading %s\n", filename);
	if (!g_file_get_contents(filename, &contents, &length, &error)) {
		gaim_debug(GAIM_DEBUG_ERROR, "xfire games import",
				   "Error reading game list: %s\n", error->message);
		g_error_free(error);
		return FALSE;
	}

	node = xmlnode_from_str(contents, length);
	
	if (!node) {
		gaim_debug(GAIM_DEBUG_ERROR, "xfire games import",
				   "Error parsing XML file: %s\n", filename);
		g_free(contents);
		return FALSE;
	}

	xfire_games_xmldoc = node;	
	g_free(contents);
	return TRUE;
};

/**
 * Returns via *buffer the name of the game identified by int game
 * if the game doesn't exist, it uses the game number as the name
 *
 * @param buffer			pointer to string return value will be here
 * @param game			integer ID of the game to translate
*/
void
xfire_game_name(char *buffer, int game)
{
	xmlnode *node = NULL;
	const char *name, *num;
	int found = 0;
	int id = 0;
	
	if (xfire_games_xmldoc != NULL) {
		node = xmlnode_get_child(xfire_games_xmldoc, "game");
		while (node) {
			name = xmlnode_get_attrib(node, "name");
			num  = xmlnode_get_attrib(node, "id");
			id = atoi((const char *)num);
			if (id == game) {
				found = 1;
			}
			if (found) break;
			node = xmlnode_get_next_twin(node);
		}
		/* if we didn't find the game just show game ID */
		if (!found) {
			name = g_strdup_printf("%d",game);
		}
		memcpy(buffer,name,strlen(name)+1);
		if (!found) free((void *)name);
	}else{
		name = g_strdup_printf("%d",game);
		memcpy(buffer,name,strlen(name)+1);
	}

}

/**
 * Frees memory associated with XML for game translation
*/
void xfire_games_cleanup()
{
	xmlnode_free(xfire_games_xmldoc);
}


/**
 *	Parses launchinfo.xml file into memory for launching games
 *	directly from right click menu
 *
 *	@param		filename		filename of the xml file to parse (dir+fname)
 *
 *	@return		boolean		return true on success, false on failure
**/
gboolean
xfire_parse_linfo_xml(const char *filename)
{
	xmlnode *node = NULL;
	GError *error = NULL;
	gchar *contents = NULL;
	gsize length;

	gaim_debug(GAIM_DEBUG_INFO, "xfire launchinfo import",
			   "Reading %s\n", filename);
	if (!g_file_get_contents(filename, &contents, &length, &error)) {
		gaim_debug(GAIM_DEBUG_ERROR, "xfire launchinfo import",
				   "Error reading launchinfo list: %s\n", error->message);
		g_error_free(error);
		return FALSE;
	}

	node = xmlnode_from_str(contents, length);
	
	if (!node) {
		gaim_debug(GAIM_DEBUG_ERROR, "xfire launchinfo import",
				   "Error parsing XML file: %s\n", filename);
		g_free(contents);
		return FALSE;
	}

	xfire_linfo_xmldoc = node;	
	g_free(contents);
	return TRUE;
}


/**
 * Frees memory associated with XML for game launching
*/
void xfire_linfo_cleanup()
{
	xmlnode_free(xfire_linfo_xmldoc);
}


/**
 * Determines of a game is configured, and is playable
 *
 * @param game		integer ID of game to check to see if its playable
 *
 * @return TRUE if game is playable, FALSE if not
*/
gboolean
xfire_game_playable(int game)
{
	xfire_linfo *foo = NULL;
	
	if (!game) return FALSE;
	foo = xfire_linfo_get(game);
	if (!foo) return FALSE;
	xfire_linfo_free(foo);
	return TRUE;

}


/**
 * gives the integer representation of the game the buddy is playing
 *
 * @param b		Buddy to get game number of
 *
 * @return returns the game or 0 if the buddy is not playing
*/
int
xfire_get_buddy_game(GaimBlistNode *b)
{
	return (int)gaim_blist_node_get_int(b,"game");
}


/**
 * get port number of the game the buddy is playing
 *
 * @param b		Buddy to get game number of
 *
 * @return returns the game or 0 if the buddy is not playing
*/
int
xfire_get_buddy_port(GaimBlistNode *b)
{
	return (int)gaim_blist_node_get_int(b,"port");
}


/**
 * get ip address of server where buddy is in game
 *
 * @param b		Buddy to get game ip number of
 *
 * @return returns the ip address as a string or NULL if budy isn't playing
*/
const gchar *
xfire_get_buddy_ip(GaimBlistNode *b)
{
	if(xfire_get_buddy_game(b) != 0){
			
		return (char *)gaim_blist_node_get_string(b,"ip");
 	}
 	return NULL;
}


/**
 *  Creates a newly allocated and zeroed launch info struct
 *
 *  @return     pointer to newly created zeroed structure
*/
xfire_linfo *
xfire_linfo_new()
{
	xfire_linfo *ret = NULL;
	ret = g_malloc(sizeof(xfire_linfo));
	if (!ret) return ret;
	ret->gameid=0; ret->c_bin=ret->c_wdir=ret->name=ret->xqfname=ret->c_gmod = NULL;
	ret->c_cmd=ret->c_connect=ret->c_options=ret->xqfmods =  NULL;
	return ret;
}


/**
 *	Frees memory associated with launch info structure, passing null pointer
 *	causes function to return.
 *
 *	@param		*l		pointer to associated structure to free (null ok)
*/
void
xfire_linfo_free(xfire_linfo *l)
{
	if (!l) return;
	g_free(l->name);
	g_free(l->xqfname);
	g_free(l->xqfmods);
	g_free(l->c_bin);
	g_free(l->c_wdir);
	g_free(l->c_gmod);
	g_free(l->c_connect);
	g_free(l->c_options);
	g_free(l->c_cmd);
	g_free(l);
}


/**
 *	Searches launch.xml node structure for associated game, and then returns launch
 *	info struct associated with that game. Returns NULL if game is not found.
 *
 *	@param		game		integer xfire game ID of game to get and return
 *
 *	@return					Returns launch info structure with fields filled or
 *							NULL if game can't be found
*/
xfire_linfo *
xfire_linfo_get(int game)
{
	xfire_linfo *l = NULL;
	xmlnode *node = NULL;
	xmlnode *cnode = NULL;
	xmlnode *command = NULL;
	const char *name, *num;
	int found = 0;
	int id = 0;
	
	if (xfire_linfo_xmldoc != NULL) {
		node = xmlnode_get_child(xfire_linfo_xmldoc, "game");
		while (node) {
			name = xmlnode_get_attrib(node, "name");
			num  = xmlnode_get_attrib(node, "id");
			id = atoi((const char *)num);
// printf("linfo id=%d\tname=\"%s\"\n", id, name);
			if (id == game) {
				found = 1;
			}
			if (found) break;
			node = xmlnode_get_next_twin(node);
		}    
		/* if we didn't find the game */
		if (!found) {
			return NULL;
		}
		/* got the game */
		l = xfire_linfo_new();
		if (!l) return NULL; /* Out of Memory */
		
		l->gameid = game;
		l->name = g_strdup(name);
		for (cnode = node->child; cnode; cnode = cnode->next) {
			if (cnode->type != XMLNODE_TYPE_TAG)
				continue;
			if (!strcmp(cnode->name, "xqf")) {
				l->xqfname = g_strdup((gchar *)xmlnode_get_attrib(cnode, "name"));
				l->xqfmods = g_strdup((gchar *)xmlnode_get_attrib(cnode, "modlist"));
			}
			
			if (!strcmp(cnode->name, "command")) {
				if ((command = xmlnode_get_child(cnode, "bin")))
					l->c_bin = g_strdup(xmlnode_get_data(command));
				if ((command = xmlnode_get_child(cnode, "dir")))
					l->c_wdir = g_strdup(xmlnode_get_data(command));
				if ((command = xmlnode_get_child(cnode, "gamemod")))
					l->c_gmod = g_strdup(xmlnode_get_data(command));
				if ((command = xmlnode_get_child(cnode, "options")))
					l->c_options = g_strdup(xmlnode_get_data(command));
				if ((command = xmlnode_get_child(cnode, "connect")))
					l->c_connect = g_strdup(xmlnode_get_data(command));
				if ((command = xmlnode_get_child(cnode, "launch")))
					l->c_cmd = g_strdup(xmlnode_get_data(command));
			}
		}
	}
	
	return l;
}


/**
 *	Parses launch field for delim and replaces it with data. Returns pointer to newly
 *	created string with all subsitutions.
 *
 *	@param		src		string to search for delimeters (usually @xxxx@)
 *	@param		data	string to replace delimeter with. (ip address, ports, etc)
 *	@param		delim	the matching string that needs to be replaced (ex: @port@)
 *
 *	@return				pointer to string with delimeter replaced with data. memory
 *						is allocated using glib, free with g_free when done.
*/
gchar *
xfire_launch_parse(const char *src, const char *data, const char *delim)
{
	char **sv;
	gchar *tmp = NULL;
	
	if (!data || !delim || !src) {
		/* no data or delim return empty string */
		return g_strdup("");
	}
	
	sv = g_strsplit(src, delim, 0);
	if (g_strv_length(sv) <= 1) {
		/* no matches */
		return g_strdup(src);
	}

	if (g_strv_length(sv) > 2 ) {
		return g_strdup("");
//		fprintf(stderr, "ERR: (xfire_launch_parse) Only 1 delimeter allowed per subsitution!\n");
	}
	tmp = g_strjoinv(data, sv);	
	g_strfreev(sv);
	return tmp;

}


/**
 *	takes launch info struct, and returns a command line that is parsed with information
 *	in struct and replaced with data provided by other function arguments. String should be
 *	freed after use.
 *
 *	@param		l		populated launch info struct
 *	@param		ip		string ip address of server to connect to
 *	@param		prt		integer port number for connect line
 *	@param		mod		string name of mod that needs to be run (only used in xqf)
 *
 *	@return				pointer to string command line fully parsed. if l is NULL function
 						returns NULL. String should be freed after use. g_free()
*/
gchar *
xfire_linfo_get_cmd(xfire_linfo *l, const gchar *ip, int prt, const gchar *mod)
{
	gchar *old = NULL;
	gchar *cmd = NULL;
	gchar *options = NULL;
	gchar *connect = NULL;
	gchar *gamemod = NULL;
	gchar *port = NULL;

	if (!l) return NULL;
	
	port = g_strdup_printf("%d",prt);
	
	connect = xfire_launch_parse(l->c_connect, ip, "@ip@");
	old = connect;
	connect = xfire_launch_parse(connect, port, "@port@");
	g_free(old);
	
	gamemod = xfire_launch_parse(l->c_gmod, mod, "@mod@");
	
	cmd =  xfire_launch_parse(l->c_cmd, connect, "@connect@");
	old = cmd;
	cmd = xfire_launch_parse(cmd, gamemod, "@gamemod@");
	g_free(old); old = cmd;
	cmd = xfire_launch_parse(cmd, l->c_options, "@options@");
	g_free(old); g_strstrip(cmd); old = cmd;
	cmd = g_strdup_printf("%s %s", l->c_bin, cmd);
	g_free(gamemod); g_free(options); g_free(connect); g_free(port); g_free(old);
	return cmd;

}



/**
 *	produces a new xqf_linfo struct zeroed out needs to be freed using
 *	xqf_linfo_free(), returns struct pointer on success, NULL on failure
 *
 *	@return		pointer to newly created xfire_xqf_linfo struct or
 *				NULL on memory allocation error
*/
xfire_xqf_linfo *
xfire_xqf_linfo_new()
{
	xfire_xqf_linfo *l = NULL;
	l = g_malloc(sizeof(xfire_xqf_linfo));
	if (!l) return NULL;
	l->gtype=l->sname=l->ip=l->mod = NULL;
	l->port = 0;
	return l;
}


/**
 *	frees memory from struct created by xqf_linfo_new(), passing
 *	a null pointer is ok.
 *
 *	@param		l	pointer to structure
*/
void
xfire_xqf_linfo_free(xfire_xqf_linfo *l)
{
	if (!l) return;
	g_free(l->gtype);
	g_free(l->sname);
	g_free(l->ip);
	g_free(l->mod);
	g_free(l);
}


/**
 *	searches the xml data structure provided by launch.xml to
 *	to find a game that matches the xfq_linfo struct that is passed
 *	as an argument (if one can be found). matches are done case-
 *	insenitve.  xqfname="" property of game must match the LaunchInfo.txt
 *	game name. The xqf mod in LI.txt must match one in the modlist in the
 *	xml file.  This allows you to have many games in the xml file with
 *	the same xqfname but different sets of mods.  So that we can accurately
 *	identify what xfire game id must be used.
 *
 *	@param		xqfs	pointer to xqf_linfo struct that has the parsed
 *						xqf data. (provided by linfo_parse_xqf())
 *
 *	@return				returns the game id of the matching game in the
 *						launch.xml file or 0 if no game was found
*/
int
xfire_xqf_search(xfire_xqf_linfo *xqfs)
{
	xmlnode *node = NULL;
	xmlnode *cnode = NULL;
	const char *name, *num;
	gchar *mod, *xqfname;
	int found = 0;
	int game = 0;
	
	if (xfire_linfo_xmldoc == NULL) return 0;

	node = xmlnode_get_child(xfire_linfo_xmldoc, "game");
	while (node) {
		name = g_strdup(xmlnode_get_attrib(node, "name"));
		num  = xmlnode_get_attrib(node, "id");
		game = atoi((const char *)num);
		for (cnode = node->child; cnode; cnode = cnode->next) {
			if (cnode->type != XMLNODE_TYPE_TAG)
				continue;
			if (!strcmp(cnode->name, "xqf")) {
				xqfname = (gchar *)xmlnode_get_attrib(cnode, "name");
				mod = (gchar *)xmlnode_get_attrib(cnode, "modlist");
				if ( 0 == g_ascii_strcasecmp(xqfname, xqfs->gtype) ){
					if (g_strrstr(mod, xqfs->mod) ){
						found = TRUE;
						break;
					}
				}
			}
		}

		if (found) break;
		node = xmlnode_get_next_twin(node);
	}    
	if (found) return game;
		else return 0;
}


/**
 *	parses XQF launchinfo.txt file into a struct that can be used
 *	to search for the game id, and server/port information.  This
 *	data can be used to send join_game messages to the xfire network
 *
 *	@param		fname	string filename of the file to parse as a
 *						launchinfo.txt formatted file
 *
 *	@return				returns xqf_linfo structure if parsed ok
 *						otherwise it returns NULL
*/
xfire_xqf_linfo *
xfire_linfo_parse_xqf(gchar *fname)
{
	xfire_xqf_linfo *l = NULL;
	GError *error = NULL;
	gchar *contents = NULL;
	gsize length;
	gchar **sv = NULL;
	gchar **svtmp = NULL;
	int i = 0;
	gchar *tmp = NULL;

	if (!g_file_get_contents(fname, &contents, &length, &error)) {
		gaim_debug(GAIM_DEBUG_ERROR, "xfire xqf launchinfo import",
				   "Error reading LaunchInfo.txt: %s\n", error->message);
		g_error_free(error);
		return NULL;
	}
	if ( !(l = xfire_xqf_linfo_new()) ) {
		g_free(contents);
		return NULL;
	}
	sv = g_strsplit(contents, "\n", 0);
	for (i = 0; i < g_strv_length(sv); i++) {
		if ( 0 == strlen(sv[i]) ) continue;
		if (0 == g_ascii_strncasecmp(sv[i], "GameType", 8)) {
			tmp = (gchar *)&(sv[i][9]);
			l->gtype = g_strdup(tmp);
		}
		if (0 == g_ascii_strncasecmp(sv[i], "ServerName", 10)) {
			tmp = (gchar *)&(sv[i][11]);
			l->sname = g_strdup(tmp);
		}
		if (0 == g_ascii_strncasecmp(sv[i], "ServerAddr", 10)) {
			tmp = (gchar *)&(sv[i][11]);
			svtmp = g_strsplit(tmp, ":", 0);
			l->ip = g_strdup(svtmp[0]);
			l->port = atoi((const char *)svtmp[1]);
			g_strfreev(svtmp);
		}
		if (0 == g_ascii_strncasecmp(sv[i], "ServerMod", 9)) {
			tmp = (gchar *)&(sv[i][10]);
			if (g_strrstr(tmp, ",")) {
				svtmp = g_strsplit(tmp, ",", 0);
				l->mod = g_strdup(svtmp[0]);
				g_strfreev(svtmp);
			} else l->mod = g_strdup(tmp);
		}
	} // end of for loop
	return l;
}
