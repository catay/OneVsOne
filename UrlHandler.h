
#ifndef __URLHANDLER_H__
#define __URLHANDLER_H__

#include <iostream>

#include "bzfsAPI.h"


/***
 * class BaseUrlHandler 
***/

class BaseUrlHandler : public bz_URLHandler
{
  public:
    BaseUrlHandler() 
    {
      _playerIds.clear();
      // max data size in bytes (1MB)
      _max_data_size = 1048576;
    };

    ~BaseUrlHandler() {};

    virtual void done ( const char* /*URL*/, void * data, unsigned int size, bool complete );
    virtual void error ( const char* /*URL*/, int /*errorCode*/, const char * /*errorString*/ );
    virtual void showDataOK(int playerId, bzAPIStringList* data);
    virtual void showDataNOK(int playerId, bzAPIStringList* data);
    void setPlayerId(int playerId);
    bool is_valid_status(const std::string& data);

  private:
    std::vector<int> _playerIds;
    unsigned int _max_data_size;
};

/*** 
 * Class PlayerInfo
 *The PlayerInfo class handles player info queries
***/

class PlayerInfo : public BaseUrlHandler
{
  public:
    virtual void showDataOK(int playerId, bzAPIStringList* data);
};

/***
 * Class TopScore
 * The TopScore class handles the topscore info queries
***/

class TopScore : public BaseUrlHandler
{
  public:
    virtual void showDataOK(int playerId, bzAPIStringList* data);
};

/*** 
 * Class TopZelo
 * The TopZelo class handles the topzelo info queries
***/

class TopZelo : public BaseUrlHandler
{
  public:
    virtual void showDataOK(int playerId, bzAPIStringList* data);
};


#endif



