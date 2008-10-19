
#include "INIParser.h"

INIParser::INIParser(const char * iniFile)
{
  _iniFile = iniFile;
}

int INIParser::parse()
{
  
  char ch;
  char buffer[100];
  int i = 0;
  int line = 1;

  std::string section = "__NO_SECTION__";
  std::string name;
  parseState = NO_SECTION;

  // open ini file
  std::ifstream in(_iniFile.c_str(), std::ios::in | std::ios::binary);

  while ( in.get(ch) ) {
    switch (ch) {
      case '[' :
	if (isStartSection())
	  return line;

	if ( i == 0 ) 
	  parseState = START_SECTION;
	else 
	  return line;

	break;
      case ']' :
	if (isStopSection())
	  return line;

	if (isStartSection() && i > 0) {
	  buffer[i] = '\0';
	  section = buffer;
	  //std::cout << "section => " << buffer << std::endl;
	  i=0;
	} else { // else error , empty section [] or ] not the end of start section
	  return line;
	}

	parseState = STOP_SECTION;
	break;
      case '=' :

	if ( isStartSection() )
	  return line;

	if ( isNoSection() && i == 0 )
	  return line;

	if ((isNewline() || isNoSection()) && i > 0) {
	  buffer[i] = '\0';
	  name = buffer;
	  //std::cout << "key => " << buffer << std::endl;
	  i=0;
	} else if (isNewline() && i == 0)
	  	return line;

	parseState = DELIMITER;
	
	break;
      case '\n' :

	if ( isNewline() && i > 0 )
	  return line;

	if ( isNoSection() && i > 0  )
	  return line;

	if ( isStartSection() )
	  return line;

	if ( isDelimiter() && i > 0 ) {
	  buffer[i] = '\0';
	  sections[section][name] = buffer;
	  //std::cout << "value => " << buffer << std::endl;
	  i=0;
	} else if ( isDelimiter() && i == 0 )
		return line;

	//if ( parseState != DELIMITER || parseState != STOP_SECTION || parseState != COMMENT )
	 // return line;

	parseState = NEWLINE;
	line++;
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

	if ( isNewline() || isNoSection() ) {
	  buffer[i] = ch;
	  i++;
	}

	if ( isDelimiter() ) {
	  buffer[i] = ch;
	  i++;
	}

	if ( isStartSection() ) {
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
  std::string *value = new std::string("__no_value__");

  Sections::iterator it_s = sections.find(section);

  if ( it_s != sections.end() ) {
    Parameters::iterator it_p =  it_s->second.find(name);
    if ( it_p != it_s->second.end() ) {
      return it_p->second;
    }
  }

  return *value;
}

Parameters & INIParser::getParameters(char * section)
{

  Sections::iterator it_s = sections.find(section);

//  if ( it_s != sections.end() ) {

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

bool INIParser::isNoSection()
{
  if ( parseState == NO_SECTION )
    return true;

  return false;
}

bool INIParser::isStartSection()
{
  if ( parseState == START_SECTION )
    return true;

  return false;
}

bool INIParser::isStopSection()
{
  if ( parseState == STOP_SECTION )
    return true;

  return false;
}

bool INIParser::isDelimiter()
{
  if ( parseState == DELIMITER )
    return true;

  return false;
}

bool INIParser::isComment()
{
  if ( parseState == COMMENT )
    return true;

  return false;
}

bool INIParser::isNewline()
{
  if ( parseState == NEWLINE )
    return true;

  return false;
}
