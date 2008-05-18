
#ifndef __INITPARSER_H__
#define __INITPARSER_H__

#include <iostream>
#include <fstream>
#include <map>

typedef std::map<std::string,std::string> Parameters;
typedef std::map<std::string,Parameters> Sections;

class INIParser
{
  public:
    INIParser(char * iniFile);
    ~INIParser() {};

    bool parse();
    Parameters & getParameters(char * section);
    std::string & getValue(char * section, char * name);

  private:

    typedef enum {
      NO_SECTION,
      START_SECTION,
      STOP_SECTION,
      DELIMITER,
      COMMENT,
      NEWLINE
    }   ParseState;

    std::string _iniFile;
    Sections sections;
};

#endif
