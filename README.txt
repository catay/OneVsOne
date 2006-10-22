OneVsOne plugin version 1.0.1  (October xx, 2006)
------------------------------------------------

 The OneVsOne plugin makes it possible to play
 one versus one matches. This plugin is heavily
 based on the original 1vs1 patch from Pimpinella. 
 It also behaves almost the same.

Plugin command Line:
====================

 -loadplugin PLUGINNAME[,<lives>,<filetologscores>]

 parm 1 => the amount of lives each player has
 parm 2 => the filename where the scores will be logged

 If only parm 1 (lives) is provided , the scores will
 will be logged at debug level 2.

 If no parameters are provided, 'lives' will default on 10
 and the scores will be logged at debug level 2.

 Example:   -loadplugin <path>/OneVsOne,5,/tmp/1vs1.log

In-game commands:
=================

The OneVsOne plugin provides the following in-game commands.

 /official		: 	declares an official match (requires global
					login)
 /contest		: 	declares a contest match (requires global
 					login)

 /setlives [<max lives>]	:	

 The setlives command makes it possible to change live the life 
 count on a server. This requires the SETLIVES permission.

 Players are also able to use the /superkill command. They
 can't use it when an official/contest match is in progress
 or when they declared themselfs already such a match.

 The difference between /official and /contest is the way 
 the scores get logged. 

Logging & recording:
===================

The scores get logged in the following format to a file or
to debug level 2. Only official or contest matches get
logged and recorded.

 * to a file

 contest 2006-08-27 12:28:32 karlik25-Koziol 10-8
 official 2006-08-27 12:34:56 Koziol-karlik25 10-9
 contest 2006-08-27 12:43:08 karlik25-catay 10-6
 official 2006-08-27 12:48:46 catay-karlik25 10-6
 contest 2006-08-27 12:57:50 karlik25-catay 10-8

 * to debug level 2

 DEBUG::OneVsOne PLUGIN:: SCORES :: official 2006-08-27 18:25:21
 Thonolan-catay 10-3


All the official and contest matches are also recorded.
For recording you have to specify the -recdir <dirname>
in your bzfs config file otherwise recording won't work.

Also don't put the -recbuf option in your config file,
recording won't work if you do.

The recording is named as follows : 

 winner[score]_loser[score]_yyyymmdd_hhmmss.bzr

Example : Koziol[10]_karlik25[9]_20060827_123456.bzr


Todo
====

 * Making the bz_Load method error proof.
   Doing better error checks for plugin parameters.
   Also making sure the plugin keeps working correct
   when load/unloadin it manually.

 * Remove the debug code. 

 * Make sure the plugin handles it well when 
   bzfs config allows more then 2 users. Now you have
   to make sure your bzfs config is correct.
   It is possible to make the plugin handle it correct.

 * Catch zelo info from the 1vs1 league site (Chestal's idea)

 * The official/contest commands should not be hardcoded. 
   It would be much nicer if it was a configuration option, that
   way everybody can decide which /command he wants for is server
   setup.

 * Seems not the compile on the windows platform because of 
   strcmp/strcasecmp functions.

Credits:
========

To much people to name, I will sure forget people here.

Pimpinella : the original coder of the first bzflag 1vs1
             implementation.

Also thanks to all the people who helped me test it :
(Alphabetic order)

Birdie, Chestal, Grumpf, Karlik25, Kierra, Koziol, MasterYoda, 
Mr_Molez, Thonolan.

Changelog:
==========

 * OneVsOne 1.0.1 (October 2006)

   - Kierra pointed me to the fact that the plural from life is 
     lives and not lifes. Changed lifes to lives. :P

   - Fixed a small bug that occured when at the end of a game
     both players shoot eachother before the auto explode happens.
	 Because of that a ghost playerid was added to the map. Had
	 no impact on scores or something else whatsoever.

   - Added some ascii art banners that clearly state when someone
     wins or loses.

   - Added the /setlives command.

 * OneVsOne 1.0.0 (27 August 2006)

   - Initial release.
