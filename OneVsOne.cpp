// OneVsOne.cpp : Defines the entry point for the DLL application.
//

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <vector>

#include "bzfsAPI.h"
#include "UrlHandler.h"
#include "INIParser.h"

BZ_GET_PLUGIN_VERSION

#define ONEVSONE "1.0.3"

#define MAX_PLAYERS 2

#define DEBUG_TAG "DEBUG::OneVsOne PLUGIN::" 
#define DEBUG_LEVEL 2


typedef struct {
	int losses;
	std::string callsign;
	std::string matchType;
} OneVsOnePlayer;

/// HELPER STUFF ///

template <class T>
std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

bool replace(std::string& s,const char * orig,const char * rep )
{
  bool retval=false;
  while ( s.find(orig) != std::string::npos ) {
    s.replace(s.find(orig),std::string(orig).size(),rep);
    retval=true;
  }
  return retval;
}


class OneVsOne : public bz_EventHandler, public bz_CustomSlashCommandHandler
{
  public:
    OneVsOne();
    ~OneVsOne() {};

    bool readConfig(std::string fileName);
    bool isCompat();

    Parameters gameTypes;

    virtual void process ( bz_EventData *eventData );
    virtual bool handle ( int playerID, bzApiString, bzApiString, bzAPIStringList*);

  protected:
			
  private:

    std::map<int, OneVsOnePlayer> Players;
    bool recording;
    bool isMotd;
    bool isWelcome;
    std::string matchType;
    std::string serverName;
    std::string gameStyle;
    int maxLives;
    std::string logFile;
    double startTime;
    std::string httpUri;
    double motdLastRefreshTime; 
    double refreshInterval;
    double welcomeMessageLastRefreshTime; 
    bool cfgPerm;
    std::string cfgPermName;
    bool compatibility;
    bool isComm;

    // reporting/info handlers
    BaseUrlHandler playerInfoHandler;
    BaseUrlHandler topScoreHandler;
    BaseUrlHandler topZeloHandler;
    BaseUrlHandler registerHandler;
    BaseUrlHandler reportHandler;
    BaseUrlHandler motdHandler;
    BaseUrlHandler welcomeMessageHandler;

    // issers
    bool isMatch();

    void logRecordMatch(std::string mType, int winner, int loser);
    void registerPlayer(int p, bzAPIStringList*);
    void getPlayerInfo(int p, bzAPIStringList*);
    void getTopScore(int p, bzAPIStringList*);
    void getTopZelo(int p, bzAPIStringList*);
    void setMatch(int p, bzAPIStringList*);
    void handleMotd(int p, bzAPIStringList*);
    void setLives(int p, bzAPIStringList*);
    void showHelp(int p, bzApiString action = "all");

    void printScore(void);
    void printBanner(int winner, int loser);
    void addPlayer(int playerId, const std::string callsign);
    void delPlayer(int playerId); 
    void setLoss(int playerId);  
    int getHighestLoss(void);
    int getWinner(int playerId);
    void saveScores(char * scores);
    void showMotdBanner(int playerId, bool force=false);
    void showWelcomeMessage(int playerId, bool force=false);

};

OneVsOne oneVsOne;

OneVsOne::OneVsOne()
{
  // Initialize the default values

  // general section

  maxLives = 10;
  gameStyle = "classic";
  recording = false;
  compatibility = true;

  // commands
  gameTypes["official"] = "official";

  // communication
  httpUri = "";
  isMotd = false;
  isWelcome = false;
  // in seconds , default 1 hour 
  refreshInterval = 3600;

  // logging
  logFile = "none";

  // various initializations
  serverName = "n/a";
  motdLastRefreshTime = 0;
  welcomeMessageLastRefreshTime = 0;
  startTime = 0;
  
  motdHandler.setNoNoKNotify(true);
  cfgPerm =  false;
  cfgPermName = "OVSO_CFG";

  isComm = false;
}


bool OneVsOne::readConfig(std::string fileName)
{
  INIParser config = INIParser(fileName.c_str());
  int status = config.parse();

  if (status > 0 ) {
    bz_debugMessage(DEBUG_LEVEL,"OneVsOne plugin loading FAILED");
    bz_debugMessagef(DEBUG_LEVEL,"Error parsing config file at line %d", status);
    return false;
  }

  if (config.isSection("general") ) {

    if ( config.isValue("general","max_lives") ) 
	maxLives = atoi(config.getValue("general", "max_lives").c_str());

    if ( config.isValue("general","style") )
      gameStyle = config.getValue("general","style");

    if ( config.isValue("general","compatibility") ) {
	if ( config.getValue("general", "compatibility") == "false" ) {
	  compatibility = false;
	}
    }
  }

  gameTypes.clear();

  if (config.isSection("commands") )
    oneVsOne.gameTypes = config.getParameters("commands");

  if (config.isSection("communication") ) {
    if ( config.isValue("communication","httpuri") ) {
      httpUri = config.getValue("communication", "httpuri");
      isComm = true;

      if ( config.isValue("communication", "refresh_interval") ) {
	refreshInterval = atoi(config.getValue("communication", "refresh_interval").c_str());
      }

      if ( config.isValue("communication","enable_motd") ) {
	if ( config.getValue("communication", "enable_motd") == "true" ) {
	  isMotd = true;
	}
      }

      if ( config.getValue("communication", "enable_welcome") == "true" ) {
	isWelcome = true;
      }
    }
  }

  if (config.isSection("logging") ) {
    if ( config.isValue("logging","logfile") )
      logFile = config.getValue("logging", "logfile");
  }

}

bool OneVsOne::isCompat()
{
  return compatibility;
}

/*
 * Isser that returns true when a match is in progress, false
 * if not.
 */

bool OneVsOne::isMatch()
{
  if ( Players.size() != MAX_PLAYERS ) 
    return false;

  std::map<int,OneVsOnePlayer>::iterator it = Players.begin();

  std::string matchType = (*it).second.matchType;

  if ( matchType.size() == 0)
    return false;

  for( ; it != Players.end(); it++ ) {
    if ( (*it).second.matchType != matchType ) {
      return false;
    }
  }

  return true;
}  

void OneVsOne::registerPlayer(int playerID, bzAPIStringList* params)
{
  if ( params->size() == 2 ) {
    bz_PlayerRecord *playerRecord;

    playerRecord = bz_getPlayerByIndex ( playerID );

    // check if player is verified
    if ( playerRecord->verified) {
      std::string  registerdata;
      registerdata = std::string("action=register&callsign=") + std::string(bz_urlEncode(playerRecord->callsign.c_str())) + 
	std::string("&bzid=") + std::string(bz_urlEncode(playerRecord->bzID.c_str())) + 
	std::string("&email=") + std::string(bz_urlEncode(params->get(1).c_str())) + 
	std::string("&ip=") + std::string(bz_urlEncode(playerRecord->ipAddress.c_str()));

      registerHandler.setPlayerId(playerID);
					
      bz_addURLJob(httpUri.c_str(),&registerHandler, registerdata.c_str());
    } else 
      bz_sendTextMessagef ( BZ_SERVER, playerID,"You must be globally registered and identified to register here" );

    bz_freePlayerRecord ( playerRecord );
  } else 
    showHelp(playerID, params->get(0));
}

void OneVsOne::getPlayerInfo(int playerID, bzAPIStringList* params) 
{
  if ( params->size() >= 2 ) {
    bz_PlayerRecord *playerRecord;
    playerRecord = bz_getPlayerByIndex ( playerID );

    std::string  playerinfodata = "action=playerinfo";

    for (unsigned int i = 1; i < params->size(); i++)
      playerinfodata.append(std::string("&callsigns[]=") + std::string(bz_urlEncode(params->get(i).c_str())));

    playerInfoHandler.setPlayerId(playerID);
					
    bz_addURLJob(httpUri.c_str(),&playerInfoHandler, playerinfodata.c_str());

    bz_freePlayerRecord ( playerRecord );
  } else 
    showHelp(playerID, params->get(0));
}

void OneVsOne::getTopScore(int playerID, bzAPIStringList* params) 
{
  if ( params->size() >  2 ) 
    showHelp(playerID, params->get(0));
  else {
    bz_PlayerRecord *playerRecord;
    std::string items = "";

    playerRecord = bz_getPlayerByIndex ( playerID );

    if ( params->size() == 2  ) 
      items = bz_urlEncode(params->get(1).c_str());

    std::string  topscoredata = std::string("action=topscore&items=") + items;

    topScoreHandler.setPlayerId(playerID);
					
    bz_addURLJob(httpUri.c_str(),&topScoreHandler, topscoredata.c_str());

    bz_freePlayerRecord ( playerRecord );
  }
}

void OneVsOne::getTopZelo(int playerID, bzAPIStringList* params) 
{
  if ( params->size() >  2 ) 
    showHelp(playerID, params->get(0));
  else {
    bz_PlayerRecord *playerRecord;
    std::string items = "";

    playerRecord = bz_getPlayerByIndex ( playerID );

    if ( params->size() == 2  )
      items = bz_urlEncode(params->get(1).c_str());

    std::string  topzelodata = std::string("action=topzelo&items=") + items;

    topZeloHandler.setPlayerId(playerID);
					
    bz_addURLJob(httpUri.c_str(),&topZeloHandler, topzelodata.c_str());

    bz_freePlayerRecord ( playerRecord );
  }
}

void OneVsOne::setMatch(int playerID, bzAPIStringList* params) 
{
  Parameters::iterator matchTypeIt;

  if ( params->size() ==  2 ) {

    matchTypeIt = gameTypes.find(params->get(1).c_str());

    if ( matchTypeIt != gameTypes.end()) {
      bz_PlayerRecord *playerRecord;
      playerRecord = bz_getPlayerByIndex( playerID );

      if ( playerRecord->team == eObservers ) {
	bz_sendTextMessagef ( BZ_SERVER, playerID,"Observers obviously can't play matches ;)", (*matchTypeIt).first.c_str());
	return;
      }
		
      if ( ! playerRecord->globalUser ) {
	bz_sendTextMessagef ( BZ_SERVER, playerID,"You must be registered and identified to play matches" );
	return;
      }

      if ( Players[playerID].losses != 0 ) {
	bz_sendTextMessagef ( BZ_SERVER, playerID,"You have to declare the match type BEFORE you start playing" );
	return;
      }

      if ( ! isMatch() ) {
	matchType.clear();
	Players[playerID].matchType = (*matchTypeIt).first;
      } else {
	bz_sendTextMessagef ( BZ_SERVER, playerID,"There is already a match in progress ... [%s]", (*matchTypeIt).first.c_str());
	return;
      }

      bz_sendTextMessagef ( BZ_SERVER, BZ_ALLUSERS,"%s declared to play a match [%s]", playerRecord->callsign.c_str(), (*matchTypeIt).first.c_str());

      if ( isMatch () ) {
	bz_sendTextMessagef ( BZ_SERVER, BZ_ALLUSERS,"All current players agreed to play a match [%s]", (*matchTypeIt).first.c_str());
	matchType = (*matchTypeIt).first.c_str();
	startTime = bz_getCurrentTime();
      
        if ( recording )
	  bz_stopRecBuf ();

        recording = bz_startRecBuf ();
      }

      bz_freePlayerRecord ( playerRecord );

      return;
    }
  }

  // list all availabe types 
  bz_sendTextMessage( BZ_SERVER, playerID,"Available match types are : ");
  matchTypeIt = gameTypes.begin();
  for ( ; matchTypeIt != oneVsOne.gameTypes.end(); matchTypeIt++ )
    bz_sendTextMessagef( BZ_SERVER, playerID," * %s",(*matchTypeIt).first.c_str());

}

void OneVsOne::handleMotd(int playerID, bzAPIStringList* params) 
{

  if ( params->size() ==  2 && params->get(1) == "get" ) {
     showMotdBanner(playerID, true);
  }
  else if ( params->size() > 2 && params->get(1) == "set" ) { 
    std::string msg = "action=motd&msg=";

    msg += std::string(bz_urlEncode(params->get(2).c_str()));

    // Dirty hack that converts \n \t \r in the correct control char code
    // http://en.wikipedia.org/wiki/ASCII#ASCII_control_characters

    replace(msg,"%5Cn","%0a"); // \n
    replace(msg,"%5Ct","%09"); // \t
    replace(msg,"%5Cr","%0d"); // \r

    bz_PlayerRecord *playerRecord;
    playerRecord = bz_getPlayerByIndex(playerID);

    msg += "&bzid=" + std::string(playerRecord->bzID.c_str()) + "&callsign=" + std::string(bz_urlEncode(playerRecord->callsign.c_str())) +
      "&ip=" + std::string(bz_urlEncode(playerRecord->ipAddress.c_str()));

    bz_freePlayerRecord ( playerRecord );

    motdHandler.setPlayerId(playerID);
    bz_addURLJob(httpUri.c_str(), &motdHandler, msg.c_str());
    motdLastRefreshTime = time(NULL);
  }
  else { 
    showHelp(playerID, params->get(0));
  }

}

void OneVsOne::setLives(int playerID, bzAPIStringList* params)
{ 
  if ( params->size() ==  2 && params->get(1) == "get" ) {
    bz_sendTextMessagef ( BZ_SERVER, playerID,"Life count set to %d.", maxLives);
  }
  else if ( params->size() == 3 && params->get(1) == "set" ) {
    int lives = 0;
    std::istringstream iss(params->get(2).c_str());

    if (( iss >> lives)) { 
      bz_PlayerRecord *playerRecord; 
      playerRecord = bz_getPlayerByIndex ( playerID );

      if ( getHighestLoss() < lives && lives > 0 ) {
	maxLives=lives;
	bz_sendTextMessagef ( BZ_SERVER, BZ_ALLUSERS,"Life count set to %d by %s.", maxLives, playerRecord->callsign.c_str() );
      } else
	bz_sendTextMessagef ( BZ_SERVER, playerID,"Life count should be higher then highest player hit count.");
      
      bz_freePlayerRecord ( playerRecord );
    } else
      bz_sendTextMessagef ( BZ_SERVER, playerID,"Life count should be numerical value.");

  } else 
    showHelp(playerID, params->get(0));
}

void OneVsOne::showHelp(int playerID, bzApiString action)
{
  action.tolower();
	  
  if (action == "help")
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso help [<action>]" );
  else if (action == "match") 
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso match [<match type>]" );
  else if (action == "register" && isComm ) 
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso register <emailaddress>" );
  else if (action == "playerinfo" && isComm )
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso playerinfo <callsign> [<callsign> ...]" );
  else if (action == "topscore" && isComm )
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso topscore [<items>]" );
  else if (action == "topzelo" && isComm )
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso topzelo [<items>]" );
  else if (action == "motd" && cfgPerm && isComm )
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso motd <get>|<set [message]> " );
  else if (action == "lives" && cfgPerm && isComm )
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso lives <get>|<set [life count]> " );
  else if (action == "reload" && cfgPerm && isComm )
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso reload " );
  else {
    bz_sendTextMessage( BZ_SERVER, playerID,"Usage: /ovso <action> <params>" );
    bz_sendTextMessage( BZ_SERVER, playerID,"" );
    bz_sendTextMessage( BZ_SERVER, playerID,"action:" );
    bz_sendTextMessage( BZ_SERVER, playerID,"" );
    bz_sendTextMessage( BZ_SERVER, playerID," help [<action>] 1vs1 help" );
    bz_sendTextMessage( BZ_SERVER, playerID," match [<match type>] start a match of a certain type" );

    if ( isComm ) {
      bz_sendTextMessage( BZ_SERVER, playerID," register <valid emailaddress>  1vs1 league registration" );
      bz_sendTextMessage( BZ_SERVER, playerID," playerinfo <callsign> [<callsign> ...] show 1vs1 info of a player" );
      bz_sendTextMessage( BZ_SERVER, playerID," topscore [<items>]  show the monthly player score ranking" );
      bz_sendTextMessage( BZ_SERVER, playerID," topzelo [<items>]  show the player zelo ranking" );

      if ( cfgPerm ) 
	bz_sendTextMessage( BZ_SERVER, playerID," motd <get>|<set [message]>  get/set global message of the day" );

    }

    if ( cfgPerm ) {
      bz_sendTextMessage( BZ_SERVER, playerID," lives <get>|<set [life count]>  get/set maximum life count" );
      bz_sendTextMessage( BZ_SERVER, playerID," reload reloads the configuration file (not available)" );
    }

    bz_sendTextMessage( BZ_SERVER, playerID,"" );
  }
}

void OneVsOne::printScore ( void )
{
  std::map<int,OneVsOnePlayer>::iterator it = Players.begin();
  std::string timeMsg;
  std::string lifeMsg;
  int hits, lives = 0;

  for ( ; it != Players.end(); it++ ) { 
	hits = (*it).second.losses;
	lives = maxLives - (*it).second.losses;

  if ( hits == 1 )
    timeMsg = "time"; 
  else timeMsg = "times";

  if ( lives == 1 )
    lifeMsg = "life"; 
  else lifeMsg = "lives";

  bz_sendTextMessagef ( BZ_SERVER, BZ_ALLUSERS,"%s got hit %d %s, %d %s left",
      (*it).second.callsign.c_str(), hits, timeMsg.c_str(), lives, lifeMsg.c_str());
  }
}

void OneVsOne::printBanner ( int winner, int loser )
{
  // this makes sure that if only 1 player is playing, that player
  // always gets the 'you lose' banner.

  if ( Players.size() == MAX_PLAYERS ) {
    bz_sendTextMessagef ( BZ_SERVER, winner,"                              .__ ");
    bz_sendTextMessagef ( BZ_SERVER, winner," ___.__. ____  __ __  __  _  _|__| ____ ");
    bz_sendTextMessagef ( BZ_SERVER, winner,"<   |  |/  _ \\|  |  \\ \\ \\/ \\/ /  |/    \\ ");
    bz_sendTextMessagef ( BZ_SERVER, winner," \\___  (  <_> )  |  /  \\     /|  |   |  \\ ");
    bz_sendTextMessagef ( BZ_SERVER, winner," / ____|\\____/|____/    \\/\\_/ |__|___|  / ");
    bz_sendTextMessagef ( BZ_SERVER, winner," \\/                                   \\/ ");
  }

  bz_sendTextMessagef ( BZ_SERVER, loser,"                      .__");
  bz_sendTextMessagef ( BZ_SERVER, loser," ___.__. ____  __ __  |  |   ____  ______ ____");
  bz_sendTextMessagef ( BZ_SERVER, loser,"<   |  |/  _ \\|  |  \\ |  |  /  _ \\/  ___// __ \\ ");
  bz_sendTextMessagef ( BZ_SERVER, loser," \\___  (  <_> )  |  / |  |_(  <_> )___ \\\\  ___/");
  bz_sendTextMessagef ( BZ_SERVER, loser," / ____|\\____/|____/  |____/\\____/____  >\\___  >");
  bz_sendTextMessagef ( BZ_SERVER, loser," \\/                                   \\/     \\/");
}

void OneVsOne::addPlayer ( int playerId, const std::string callsign ) 
{
  Players[playerId].callsign = callsign;
  Players[playerId].losses=0;
}	

void OneVsOne::delPlayer(int playerId) 
{ 
  Players.erase(playerId);
}

void OneVsOne::setLoss( int playerId ) 
{
  Players[playerId].losses++;
}	

int OneVsOne::getHighestLoss(void) 
{
  int highestLoss=-1;

  std::map<int,OneVsOnePlayer>::iterator it = Players.begin(), stop = Players.end();
  for( ; it != stop; it++ ) { 
    if ( (*it).second.losses > highestLoss ) 
      highestLoss = (*it).second.losses;
  }
  return highestLoss;
}

int OneVsOne::getWinner( int playerId )
{

  int winner=-1;

  if ( Players[playerId].losses == maxLives ) {
    if ( Players.size() == 1 ) {
      winner=playerId;
    } else {
      std::map<int,OneVsOnePlayer>::iterator it = Players.begin(), stop = Players.end();
      for( ; it != stop; it++ ) { 
	if ( (*it).second.losses != maxLives ) { 
	  winner = (*it).first;
	  break;
	}
      }
    }
  }

  return winner;
}

void OneVsOne::saveScores(char * scores)
{
  // always log the score to bzfs debug channel 
  bz_debugMessagef ( 2,"%s SCORES :: %s", DEBUG_TAG, scores );	

  // log the score also to a seperate file if configured
  if ( logFile != "none" )  {
    std::ofstream myfile;	
    myfile.open(logFile.c_str(), std::ios::out | std::ios::app | std::ios::binary);
    myfile << scores;
    myfile.close();
  }  
}

void OneVsOne::showMotdBanner(int playerId, bool force)
{
  // only get the motd from the remote server when the interval has exceeded
  // else get the cached motd
  if ( ((time(NULL) - motdLastRefreshTime) > refreshInterval) || force ) {
    motdHandler.setPlayerId(playerId);
    bz_addURLJob(httpUri.c_str(), &motdHandler, "action=motd");
    motdLastRefreshTime = time(NULL);
  }
  else motdHandler.showData(playerId);
}

void OneVsOne::showWelcomeMessage(int playerId, bool force)
{
  // only get the welcome message from the remote server when the interval has exceeded
  // else get the cached welcome message
  if ( ((time(NULL) - welcomeMessageLastRefreshTime) > refreshInterval) || force ) {
    welcomeMessageHandler.setPlayerId(playerId);
    bz_addURLJob(httpUri.c_str(), &welcomeMessageHandler, "action=welcome_msg");
    welcomeMessageLastRefreshTime = time(NULL);
  }
  else welcomeMessageHandler.showData(playerId);
}

void OneVsOne::logRecordMatch(std::string mType, int winner, int loser)
{
  time_t t = time(NULL);
  tm * now = gmtime(&t);
  int duration = (int) (bz_getCurrentTime() - startTime);
		  
  char scores[200];
  char match_date[20];

  std::string reportData;

  // save recording
  if ( recording ) {		
    char filename[512];
    snprintf( filename,512,"%s[%d]_%s[%d]_%02d%02d%02d_%02d%02d%02d.bzr",
	Players[winner].callsign.c_str(),Players[loser].losses,Players[loser].callsign.c_str(),
	Players[winner].losses, now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
    bz_saveRecBuf( filename );
    bz_stopRecBuf();
    bz_sendTextMessagef ( BZ_SERVER, BZ_ALLUSERS,"Match has been recorded as %s ", filename );
  } 

  sprintf(match_date, "%02d-%02d-%02d %02d:%02d:%02d", now->tm_year+1900, 
      now->tm_mon+1, 
      now->tm_mday, 
      now->tm_hour, 
      now->tm_min, 
      now->tm_sec);

  // get bzid's ... dirty

  bz_PlayerRecord *playerRecord;

  playerRecord = bz_getPlayerByIndex ( winner );
  bzApiString wbzid = playerRecord->bzID;
  bz_freePlayerRecord ( playerRecord );

  playerRecord = bz_getPlayerByIndex ( loser );
  bzApiString lbzid = playerRecord->bzID;
  bz_freePlayerRecord ( playerRecord );

  if (bz_getPublic())
    serverName = bz_getPublicAddr().c_str();

  // format scores 
  sprintf( scores,"%s\t%s\t%s\t-\t%s\t%d\t-\t%d\t%s\t-\t%s\t%s\t%d\t%s\n", gameTypes[mType].c_str(), match_date, 
      Players[winner].callsign.c_str(), Players[loser].callsign.c_str(), Players[loser].losses, Players[winner].losses,
      wbzid.c_str(), lbzid.c_str(), gameStyle.c_str(), duration, serverName.c_str());

  reportData = std::string ("action=report") + std::string("&server=") + std::string(bz_urlEncode(serverName.c_str()))
    + std::string("&style=") + std::string(bz_urlEncode(gameStyle.c_str())) + 
    std::string("&type=") + std::string(bz_urlEncode(gameTypes[mType].c_str())) + 
    std::string("&date=") + std::string(bz_urlEncode(match_date)) + std::string("&winner=") + 
    std::string(bz_urlEncode(Players[winner].callsign.c_str())) + std::string("&loser=") + 
    std::string(bz_urlEncode(Players[loser].callsign.c_str())) + std::string("&winner_score=") + 
    to_string(Players[loser].losses) + std::string("&loser_score=") + to_string(Players[winner].losses) + 
    std::string("&wbzid=") + std::string(wbzid.c_str()) + std::string("&lbzid=") + 
    std::string(lbzid.c_str() + std::string("&duration=") + to_string(duration));

  saveScores( scores );

  // do reporting over http if enabled

  if ( isComm ) {
    reportHandler.setPlayerId(BZ_ALLUSERS);
    bz_addURLJob(httpUri.c_str(), &reportHandler, reportData.c_str());
  }

}

void OneVsOne::process ( bz_EventData *eventData )
{

  if (eventData->eventType == bz_ePlayerJoinEvent) {
    bz_PlayerJoinPartEventData *joinData = (bz_PlayerJoinPartEventData*)eventData;

    if ( isWelcome )
      showWelcomeMessage(joinData->playerID);

    if ( isMotd )
      showMotdBanner(joinData->playerID);
			
    // revoking superkill perms makes sure admin/cops don't abuse it
    // when a match is in progress ;) 
    if ( bz_hasPerm ( joinData->playerID, "superkill" ))
      bz_revokePerm ( joinData->playerID, "superkill" );

    if (joinData->team != eObservers)
      addPlayer(joinData->playerID,joinData->callsign.c_str());
  }

  if (eventData->eventType == bz_ePlayerPartEvent) {
    bz_PlayerJoinPartEventData *partData = (bz_PlayerJoinPartEventData*)eventData;

    if ( partData->team != eObservers ) {
      delPlayer ( partData->playerID );
      if ( recording )
       	bz_stopRecBuf ();
    }
  }

  if (eventData->eventType == bz_ePlayerDieEvent) {

    if ( !Players.empty () ) {
      bz_PlayerDieEventData *dieData = (bz_PlayerDieEventData*)eventData;	
      setLoss(dieData->playerID);

      printScore();

      if (getWinner(dieData->playerID) != -1) {
	int loser=dieData->playerID;
	int winner=getWinner(dieData->playerID);

	bz_gameOver (winner, eNoTeam);

	printBanner(winner, loser);

	if (isMatch()) 
	  logRecordMatch(matchType,winner,loser);

	// remove all players from the list
	delPlayer ( loser );
	delPlayer ( winner );
      }
    }
  }

  if (eventData->eventType == bz_eSlashCommandEvent) {
    bz_SlashCommandEventData *slashCommandData = (bz_SlashCommandEventData*)eventData;	

    if (strcasecmp(slashCommandData->message.c_str(),"/superkill") == 0) {
      if ( isMatch() )  {
	bz_sendTextMessagef ( BZ_SERVER,slashCommandData->from, 
	    "You can't perform /superkill when a match is in progress" );
      } else 
	bz_superkill(); 
    }
  }
}

bool OneVsOne::handle ( int playerID, bzApiString cmd, bzApiString msg, bzAPIStringList* cmdParams )
{
  // transfrom to lowercase
  cmd.tolower();
  //msg.tolower();

  if ( cmd == "official" ) {
	bz_sendTextMessagef ( BZ_SERVER, playerID, 
	    "The '/official' command is DEPRECATED, use '/ovso match official' from now on.");
	cmd = "ovso";
	cmdParams->push_back(std::string("match"));
	cmdParams->push_back(std::string("official"));
  }

  cfgPerm = bz_hasPerm(playerID,cfgPermName.c_str());

  if ( cmd == "ovso") {
    if ( cmdParams->size() >= 1 ) {
      bzApiString action = cmdParams->get(0);
      action.tolower();

      if ( action == "help") {
	if ( cmdParams->size() == 2 )
	  showHelp(playerID,cmdParams->get(1));
       	else showHelp(playerID); 

	return true;
      } 

      if ( action == "register" && isComm ) {
	registerPlayer(playerID, cmdParams);
       	return true;
      }

      if ( action == "playerinfo" && isComm ) {
	bzAPIStringList *p = bz_newStringList();
       	p->tokenize(msg.c_str(), " ", 0, true);
	getPlayerInfo(playerID, p);
	bz_deleteStringList(p);
	return true;
      } 

      if ( action == "topscore" && isComm ) {
	getTopScore(playerID, cmdParams);
	return true;
      }

      if ( action == "topzelo" && isComm ) {
	getTopZelo(playerID, cmdParams);
	return true;
      } 
     
      if ( action == "match") {
	setMatch(playerID, cmdParams);
	return true;
      } 

      if ( action == "motd" && cfgPerm && isComm  ) {
	bzAPIStringList *m = bz_newStringList();
       	m->tokenize(msg.c_str(), " ", 3, false);
	handleMotd(playerID, m);
	bz_deleteStringList(m);
	return true;
      } 
      
      if ( action == "lives" && cfgPerm ) {
	setLives(playerID, cmdParams);
       	return true;
      }

      if ( action == "reload" && cfgPerm ) {
       	return true;
      }
    } 
    showHelp(playerID);
  } 
  return true;
}

BZF_PLUGIN_CALL int bz_Load ( const char* commandLine )
{
  std::string cmdLine = commandLine;

  if (cmdLine.size())
    if ( ! oneVsOne.readConfig(cmdLine) )
      bz_debugMessage( DEBUG_LEVEL, "OneVsOne reading config file failed .. falling back to defaults" );

 
  bz_registerCustomSlashCommand ("ovso", &oneVsOne);

  // compatibility stuff 
  if (oneVsOne.isCompat()) {
    Parameters::iterator it = oneVsOne.gameTypes.find("official"); 
    if ( it != oneVsOne.gameTypes.end() )
      bz_registerCustomSlashCommand ((*it).first.c_str(), &oneVsOne);
  }
 
  bz_registerEvent(bz_ePlayerJoinEvent, &oneVsOne);
  bz_registerEvent(bz_ePlayerPartEvent, &oneVsOne);
  bz_registerEvent(bz_ePlayerDieEvent, &oneVsOne);
  bz_registerEvent(bz_eSlashCommandEvent,&oneVsOne); 

  bz_debugMessage( DEBUG_LEVEL, "OneVsOne plugin loaded" );

  return 0;
}

BZF_PLUGIN_CALL int bz_Unload ( void )
{  
  
  // compatibility stuff 
  if (oneVsOne.isCompat()) {
    Parameters::iterator it = oneVsOne.gameTypes.find("official");
    if ( it != oneVsOne.gameTypes.end() )
      bz_removeCustomSlashCommand ((*it).first.c_str());
  }

  bz_removeCustomSlashCommand ("ovso");

  bz_removeEvent(bz_ePlayerJoinEvent, &oneVsOne);
  bz_removeEvent(bz_ePlayerPartEvent, &oneVsOne);
  bz_removeEvent (bz_ePlayerDieEvent, &oneVsOne);
  bz_removeEvent(bz_eSlashCommandEvent, &oneVsOne); 

  bz_debugMessage( DEBUG_LEVEL, "OneVsOne plugin unloaded" );

  return 0;
}

// Local Variables: ***
// mode:C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8
