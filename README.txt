OneVsOne plug-in version 1.0.3  (Dec 15th, 2009)
------------------------------------------------

Introduction
============

 The OneVsOne plug-in is a bzfs plug-in specially created for one of the
 finest BZFlag leagues:

 Zongo's 1vs1 BZFlag league: http://1vs1.bzleague.com

 The plug-in enhances player vs player gameplay in BZFlag, allows in-game 
 communication with the league site and provides real-time match reporting.

 The plug-in was initially heavily based on a bzfs 1vs1 patch from BZFlag 
 developer Pimpinella. During the years a lot of extra features were added 
 to make the league/game more attractive.

 The plug-in does NOT contain any 1vs1 league site specific code which 
 interacts with the plug-in. This code is developed by strayer.

 To use the full feature set of the plug-in the league code is mandatory.
 It is ofcourse always possible to write this from scratch.

License
=======

The plug-in is released under the following license:

GNU LESSER GENERAL PUBLIC LICENSE
Version 2.1, February 1999 

Requirements
============

No extra libraries are necessary to get it compiled, it all uses standard stuff.

The plug-in is compatible with the latest stable BZFLag release (2.0.12).

Plug-in command Line
=====================

The plug-in takes a ini based configuration file as a parameter.  In
case no INI file is provided it falls back to basic mode and all the 
1vs1 league integration code is deactivated.

 -loadplugin PLUGINNAME[,<path to INI file>]

  ex.
     -loadplugin /myplug-indir/OneVsOne.so,/myconfigdir/ovso.ini
  
The INI file
============

A little more info about the plug-in and the ini configuration file.

Example INI files for hix, classic, fancy 1vs1 styles can be found in the 
example directory.

The main purpose of the ini file is to make the plug-in more customizable 
and to avoid hardcoded uglyness.

A INI configuration file has 4 sections which each hold different
parameters and values :

[general]

  Contain's the most obvious stuff like 

  > max_lives

    The life count each player has. For the 1vs1 league this has to be set 
    to 10.   

  > style 

    Which style gets reported to the 1vs1 league site after an official
    match was played. 

  > compatibility 

    Is a flag that when true still allows the use of the /official
    command. Mainly to stay compatible with the old version and to not
    annoy players. The /official command got superseded by the command 
    /ovso match official.

[commands]

  It is possible to define your own 'match' commands like this :

  syntax :
  <command> = <keyword>
 
  example :
  official = official
  contest = contest2009
  
  For now we only need 'official'. After an official match the keyword
  also wil be send to the 1vs1 league site. That way we know what kind
  of match was played.  When there is a new contest for example , we can 
  add a contest command with different keyword and distinguish that way 
  what kind of match it was.

  To issue a match, we use the in-game  /ovso match <command> .

[communication]

  This part mainly deals with plug-in vs 1vs1 league site interaction.

  > httpuri

  Is the uri at which the plug-in communicates with the 1vs1 league site. 
  All communication happens over http.

  > enable_motd

  When set to true the motd at the 1vs1 league site will be displayed
  when a player joins. It is also possible the change the motd from
  within the game if you have the OVSO_CFG permission set.  
  This is possible with /ovso motd set <message>

  > enable_welcome

  When set to true the welcome message set at the 1vs1 league site will
  be displayed when a player joins.

  Both the motd & welcome message make centralized management possible
  from league site. It ensures that welcome messages across 1vs1 are all 
  the same. We don't have to annoy server owners when we want to change 
  them.

  > refresh_interval

  The interval at which the plug-in will refresh his cached version of
  the welcome and motd message. The default is 1 hour and ONLY when there 
  is a player on the server.

[logging]

  > logfile

  The logfile will store all the official played matches. The plug-in
  will send the scores and a lot more of info to the league site in
  realtime. When this fails , it is handy to have a backup system in
  place. If we store the info also to file we can later mail it to the
  league site. The mail will be automatically processed and checks will
  occuer to see if the match already was recorded.

  In the example directory the script 'report_matches.sh' is provided 
  which can be placed in crontab to mail on a daily basis the match
  reports.

In-game commands
================

The OneVsOne plug-in provides the following in-game commands.
Commands with (*) are not avaible in basic mode (without INI file).

 > /ovso help [<action>]

   Lists a brief help section about all the available actions.


 > /ovso match [<match type>]  (*)

   Declare a match of a certain match type. Executed without the match
   type paramater it provides an overview of the available types.

 > /ovso register <emailaddress>  (*)

   A player can subscribe to the 1vs1 league with this action.    
   A mail will with activation link  be send to the provided e-mail
   address.

 > /ovso playerinfo <callsign> [<callsign> ...]  (*)

   Queries the 1vs1 league site and displays in-game the player(s)
   league info.

 > /ovso topscore [<items>]  (*)

   Queries the 1vs1 league site and displays the monthly player ranking.
   The amount of items to return can be provided, default is 20. 

 > /ovso topzelo [<items>]  (*)

   Queries the 1vs1 league site and displays the players zelo ranking.
   The amount of items to return can be provided, default is 20. 

 > /ovso motd <get>|<set [message]>  (*)

   Update the Message Of The Day on the 1vs1 league site.
   This requires the OVSO_CFG permission in the groupdb file.

 > /ovso lives <get>|<set [life count]>

   Set/get the maximum life count.
   This requires the OVSO_CFG permission in the groupdb file.


Other features
==============

 > superkill

  Players are also able to use the /superkill command. 
  Using /superkill is not possible (even not by an admin/cop) when
  a official/... match is in progress or when a player himself declared 
  a match.

 > recording
 
   All the official/... matches are also recorded. For recording you 
   have to specify the -recdir <dirname> in your bzfs config file otherwise 
   recording won't work.
 
   Also don't put the -recbuf option in your config file, recording won't 
   work if you do.
 
   The recording is named as follows : 
 
   winner[score]_loser[score]_yyyymmdd_hhmmss.bzr
 
   Ex:
 
     Koziol[10]_karlik25[9]_20060827_123456.bzr
 
 > scores logged to debug level 2

   All the scores of official/... matches is also logged by bzfs at 
   debug level 2.
   
   Search for the pattern "DEBUG::OneVsOne PLUGIN:: SCORES" .


Todo
====

 > The plug-in probably doesn't work like it should when load/unloading it 
   manually.

 > Let the plug-in generate the random worlds.

 > Make sure the plug-in handles it well when bzfs config allows more 
   then 2 users. Now you have to make sure your bzfs config is correct.
   It is possible to let the plug-in handle this.

 > No idea if the code builds on Windows. 
   This code was only tested on Linux.

 > Suppress the error message for the Welcome & Motd message when the
   league site is not reachable. (Add a disableError(bool) method to
   the URL handler class)

 > A lot of other stuff I forget.


Credits:
========

To much people to name, I will sure forget people here.

Pimpinella : 

 The original coder of the first bzflag 1vs1 implementation.

Strayer: 

 For the nifty php scripts that send/receive the data from/to 
 the plug-in and for all the code that powers the 1vs1 league site.

Also thanks to all the people who helped me test it :
(Alphabetic order)

Birdie, Chestal, Grumpf, Karlik25, Kierra, Koziol, MasterYoda, 
Mr_Molez, Romfis, Thonolan, Brad, ... so many others. 

Special thanks also to all the 1vs1 league players, they make this
league so attractive:

 http://1vs1.bzleague.com/allplayers.php


Changelog:
==========

 * OneVsOne 1.0.3 (15 Dec 2009)

   - To much to name and to lazy to tell :P

 * OneVsOne 1.0.2 (3 December 2007)

   - Used a tab character as a seperator for the match reporting
     as requested by Strayer & Zongo.

   - First implementation of real time match reporting through
     http.

 * OneVsOne 1.0.1 (2 November 2006)

   - Kierra pointed me to the fact that the plural from life is 
     lives and not lifes. Changed lifes to lives. :P

   - Fixed a small bug that occured when at the end of a game
     both players shoot eachother before the auto explode happens.
	 Because of that, a ghost playerid was added to the map. Had
	 no impact on scores or something else whatsoever.

   - Added some ascii art banners that clearly state when someone
     wins or loses.

   - Added the /setlives command.

 * OneVsOne 1.0.0 (27 August 2006)

   - Initial release.
