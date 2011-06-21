
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
    INIParser(const char * iniFile);
    ~INIParser() {};

    int parse();
    Parameters & getParameters(const char * section);
    const std::string getValue(const char * section, const char * name);
    bool isSection(const char * section);
    bool isValue(const char * section, const char * name);

    bool isNoSection();
    bool isStartSection();
    bool isStopSection();
    bool isDelimiter();
    bool isComment();
    bool isNewline();

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
    ParseState parseState;
    Sections sections;
    Sections::iterator it_s;
};

#endif
