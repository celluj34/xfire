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
*/ 

struct xfire_linfo
{
	int 	gameid;
	gchar	*name;
	gchar	*xqfname;
	gchar	*xqfmods;
	gchar	*c_bin;
	gchar	*c_wdir;
	gchar	*c_gmod;
	gchar	*c_connect;
	gchar	*c_options;
	gchar	*c_cmd;
} typedef xfire_linfo;

struct xfire_xqf_linfo
{
	gchar	*gtype;			/* qstat/xqf game type ie: xqfname in gfire_launch.xml */
	gchar	*sname;			/* server name */
	gchar	*ip;			/* ip address of server */
	int		port;			/* port to connect to game */
	gchar	*mod;			/* game mod ie: xqf modlist in gfire_launch.xml */

} typedef xfire_xqf_linfo;


void xfire_game_name(char *buffer, int game);

/* parses XML file of game # to name conversions see x_games.xml */
gboolean xfire_parse_games(const char *filename);

/* called at end of run to clean up game xml */
void xfire_games_cleanup();

/* parse launchinfo.xml for launching games direct from gfire */
gboolean xfire_parse_linfo_xml(const char *filename);

/* called at end of run to cleanup memory from launch xml */
void xfire_linfo_cleanup();

/* tells if an xfire game id is playable (configure thru launch.xml) */
gboolean xfire_game_playable(int game);

/* get game id of game buddy is currently playing */
int xfire_get_buddy_game(GaimBlistNode *b);

/* get server port of game buddy is playing */
int xfire_get_buddy_port(GaimBlistNode *b);

/* get server ip address of game buddy is playing */
const gchar *xfire_get_buddy_ip(GaimBlistNode *b);

/* creates new xfire_linfo struct */
xfire_linfo *xfire_linfo_new();

/* free's mem of a created xfire_linfo struct */
void xfire_linfo_free(xfire_linfo *l);

/* return struct for a game configured in launch.xml */
xfire_linfo *xfire_linfo_get(int game);

/* returns string of parsed launch.xml information */
gchar *xfire_launch_parse(const char *src, const char *data, const char *delim);

/* return command line to launch game from right click menu "Join Game. . ." */
gchar *xfire_linfo_get_cmd(xfire_linfo *l, const gchar *ip, int prt, const gchar *mod);

/* returns a new xqf_linfo struct zeroed out */
xfire_xqf_linfo *xfire_xqf_linfo_new();

/* frees memory associtated with a xqf_linfo_struct */
void xfire_xqf_linfo_free(xfire_xqf_linfo *l);

/* searches the launch.xml memory structure for matches to a xqf launchinfo.txt */
int xfire_xqf_search(xfire_xqf_linfo *xqfs);

/* opens file and parses launchinfo.txt generate by xqf */
xfire_xqf_linfo *xfire_linfo_parse_xqf(gchar *fname);
