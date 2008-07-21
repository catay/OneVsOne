
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

      noNOKNotify = false;
      dataList = bz_newStringList();
    };

    ~BaseUrlHandler() { bz_deleteStringList(dataList); };

    virtual void done ( const char* /*URL*/, void * data, unsigned int size, bool complete );
    virtual void error ( const char* /*URL*/, int /*errorCode*/, const char * /*errorString*/ );
    virtual void showDataOK(int playerId);
    virtual void showDataNOK(int playerId);
    void setPlayerId(int playerId);
    bool is_valid_status(const std::string& data);
    bool setNoNoKNotify(bool notify);

  private:
    std::vector<int> _playerIds;
    unsigned int _max_data_size;
    bool noNOKNotify;

  protected:
    bzAPIStringList* dataList;
};

/*** 
 * Class PlayerInfo
 *The PlayerInfo class handles player info queries
***/

class PlayerInfo : public BaseUrlHandler
{
  public:
    virtual void showDataOK(int playerId);
};

/***
 * Class TopScore
 * The TopScore class handles the topscore info queries
***/

class TopScore : public BaseUrlHandler
{
  public:
    virtual void showDataOK(int playerId);
};

/*** 
 * Class TopZelo
 * The TopZelo class handles the topzelo info queries
***/

class TopZelo : public BaseUrlHandler
{
  public:
    virtual void showDataOK(int playerId);
};


#endif


