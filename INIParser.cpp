
#include "INIParser.h"

INIParser::INIParser(const char * iniFile)
{
  _iniFile = iniFile;
}

bool INIParser::parse()
{
  
  char ch;
  char buffer[100];
  int i = 0;

  ParseState parseState;
  std::string section = "__NO_SECTION__";
  std::string name;

  // open ini file
  std::ifstream in(_iniFile.c_str(), std::ios::in | std::ios::binary);

  while ( in.get(ch) ) {
    switch (ch) {
      case '[' :
	parseState = START_SECTION;
	break;
      case ']' :
	if (parseState == START_SECTION && i > 0) {
	  buffer[i] = '\0';
	  section = buffer;
	  std::cout << "section => " << buffer << std::endl;

	  i=0;
	}
	// else error

	parseState = STOP_SECTION;
	break;
      case '=' :
	if (parseState == NEWLINE || parseState == NO_SECTION && i > 0) {
	  buffer[i] = '\0';
	  name = buffer;
	  std::cout << "key => " << buffer << std::endl;
	  i=0;
	}
	parseState = DELIMITER;
	
	break;
      case '\n' :
	if ( parseState == DELIMITER && i > 0 ) {
	  buffer[i] = '\0';
	  sections[section][name] = buffer;
	  std::cout << "value => " << buffer << std::endl;
	  i=0;
	}

	parseState = NEWLINE;
	break;
      case ';' :
	parseState = COMMENT;
	break;
      case '#' :
	parseState = COMMENT;
	break;
      case ' ' : 
	break;
      default : 

	if ( parseState == NEWLINE || parseState == NO_SECTION ) {
	  buffer[i] = ch;
	  i++;
	}

	if ( parseState == DELIMITER ) {
	  buffer[i] = ch;
	  i++;
	}

	if ( parseState == START_SECTION ) {
	  buffer[i] = ch;
	  i++;
	}
	// else error

	break;
    }
  }

  in.close();

  return 0;
}

std::string & INIParser::getValue(char * section, char * name)
{
  return sections[section][name];
}

Parameters & INIParser::getParameters(char * section)
{
  return sections[section];
}

bool INIParser::isSection(char * section)
{
  return ( sections.count(section) > 0 );
}

bool INIParser::isValue(char * section, char * name)
{
  if ( isSection(section) > 0 )
    if ( sections[section].count(name) > 0 )
      return true;

  return false;
}


/*
int main () 
{
  INIParser ini = INIParser("ovso.ini");

  ini.parse();

  std::cout << "NOSECTION => " << ini.getValue("__NO_SECTION__","version") << std::endl;
  std::cout << "REPORTING => " << ini.getValue("reporting","uri") << std::endl;
  std::cout << "GENERAL => " << ini.getValue("general","max_lives") << std::endl;

  Parameters::iterator it = ini.getParameters("commands").begin();

  for (; it != ini.getParameters("commands").end(); it++)
    std::cout << "name : " << it->first << " value = " << it->second << std::endl;

  return 0;
} */
