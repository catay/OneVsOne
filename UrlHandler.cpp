
#include "UrlHandler.h"

bool replace_v2(std::string& s,const char * orig,const char * rep )
{
  bool retval=false;
  while ( s.find(orig) != std::string::npos ) {
    s.replace(s.find(orig),std::string(orig).size(),rep);
    retval=true;
  }
  return retval;
}

void BaseUrlHandler::URLDone ( const char* /*URL*/, const void * data, unsigned int size, bool complete )
{
  int _playerId = _playerIds[0];
  _playerIds.erase(_playerIds.begin());

  if ( size > 1 && size < _max_data_size ) {
    std::string _data; 
    _data.append((char*)data, size);

    if (is_valid_status(_data))	{
      replace_v2(_data,"\r\n","\n");
      replace_v2(_data,"\r","\n");
      replace_v2(_data,"\n\n","\n \n");
      //dataList->tokenize(_data.c_str(), "\r\n");
      dataList->tokenize(_data.c_str(), "\n");
      showData(_playerId);

    }
    else {
      bz_sendTextMessage (BZ_SERVER, _playerId,"No valid data was received !");
      bz_sendTextMessage (BZ_SERVER, _playerId,"This points to a bug or misuse. Please contact the server admin.");
    }
  }
  else {
    bz_sendTextMessagef (BZ_SERVER, _playerId,"The received data size (%d) exceede the limit (%d)", size, _max_data_size);
    bz_sendTextMessage (BZ_SERVER, _playerId,"This points to a bug or misuse. Please contact the server admin.");
  }
}

void BaseUrlHandler::URLError ( const char* /*URL*/, int errorCode, const char * errorString )
{
  int _playerId = _playerIds[0];
  _playerIds.erase(_playerIds.begin());
   bz_sendTextMessagef(BZ_SERVER, _playerId,"Action failed with errorcode = %d - %s  !!",errorCode, errorString);
}

void BaseUrlHandler::showDataOK(int playerId)
{
  for ( unsigned int i = 1; i < dataList->size(); i++)
    bz_sendTextMessagef ( BZ_SERVER, playerId,"%s", dataList->get(i).c_str());
}

void BaseUrlHandler::showDataNOK(int playerId)
{
  for ( unsigned int i = 1; i < dataList->size(); i++)
    bz_sendTextMessagef ( BZ_SERVER, playerId,"%s", dataList->get(1).c_str());
}

void BaseUrlHandler::showData(int playerId) 
{
  if (dataList->get(0) == "OK") 
    showDataOK(playerId);

  if (dataList->get(0) == "NOK" && (! noNOKNotify) )
    showDataNOK(playerId);
}

void BaseUrlHandler::setPlayerId(int playerId)
{
  _playerIds.push_back(playerId);
}

bool BaseUrlHandler::is_valid_status(const std::string& data) 
{
  if (data.size() < 2)
    return false;

  if ( data.compare(0,2, "OK") == 0 || data.compare(0,3, "NOK") == 0)
    return true;

  return false;
}

bool BaseUrlHandler::setNoNoKNotify(bool notify)
{
 noNOKNotify = notify;
}

void PlayerInfo::showDataOK(int playerId)
{ 
  for ( unsigned int i = 1; i < dataList->size(); i++) {
    bz_APIStringList* playerList = bz_newStringList();
    playerList->tokenize(dataList->get(i).c_str(), "\t", 6, false);

    bz_sendTextMessagef ( BZ_SERVER, playerId,"Player info for %s", playerList->get(0).c_str());
    bz_sendTextMessage ( BZ_SERVER, playerId,"-------------------------------------------");
    bz_sendTextMessagef ( BZ_SERVER, playerId,"zelo         : %s", playerList->get(1).c_str());
    bz_sendTextMessagef ( BZ_SERVER, playerId,"score        : %s", playerList->get(5).c_str());
    bz_sendTextMessagef ( BZ_SERVER, playerId,"matches won  : %s", playerList->get(2).c_str());
    bz_sendTextMessagef ( BZ_SERVER, playerId,"matches lost : %s", playerList->get(3).c_str());
    bz_sendTextMessagef ( BZ_SERVER, playerId,"status       : %s", playerList->get(4).c_str());
    bz_sendTextMessage ( BZ_SERVER, playerId,"-------------------------------------------");
    bz_deleteStringList(playerList);
  }
}

void TopScore::showDataOK(int playerId)
{

  bz_sendTextMessage ( BZ_SERVER, playerId,"Monthly points ranking");
  bz_sendTextMessage ( BZ_SERVER, playerId,"-------------------------------------------");
  bz_sendTextMessagef ( BZ_SERVER, playerId,"%-4s %-32s %5s", "Pos", "Player", "Score");
  bz_sendTextMessagef ( BZ_SERVER, playerId,"%-4s %-32s %5s", "---", "------", "-----");

  for ( unsigned int i = 1; i < dataList->size(); i++)
  {
    bz_APIStringList* topScoreList = bz_newStringList();
    topScoreList->tokenize(dataList->get(i).c_str(), "\t", 3, false);

    bz_sendTextMessagef ( BZ_SERVER, playerId,"%-4s %-32s %5s", topScoreList->get(0).c_str(), 
    topScoreList->get(1).c_str(),topScoreList->get(2).c_str());

    bz_deleteStringList(topScoreList);
  }

  bz_sendTextMessage ( BZ_SERVER, playerId,"-------------------------------------------");
}

void TopZelo::showDataOK(int playerId)
{
  bz_sendTextMessage ( BZ_SERVER, playerId,"Zelo score ranking");
  bz_sendTextMessage ( BZ_SERVER, playerId,"-------------------------------------------");
  bz_sendTextMessagef ( BZ_SERVER, playerId,"%-4s %-32s %4s", "Pos", "Player", "Zelo");
  bz_sendTextMessagef ( BZ_SERVER, playerId,"%-4s %-32s %4s", "---", "------", "----");

  for ( unsigned int i = 1; i < dataList->size(); i++) {
    bz_APIStringList* topZeloList = bz_newStringList();
    topZeloList->tokenize(dataList->get(i).c_str(), "\t", 3, false);

    bz_sendTextMessagef ( BZ_SERVER, playerId,"%-4s %-32s %-4s", topZeloList->get(0).c_str(), topZeloList->get(1).c_str(),topZeloList->get(2).c_str());
    bz_deleteStringList(topZeloList);
  }

  bz_sendTextMessage ( BZ_SERVER, playerId,"-------------------------------------------");
}
