#pragma once

/*
 * DataType: string
 *
 * A String is a char* that holds the data as a null terminated char*
 * and length is and unsigned int that holds the length of the string
 * */

typedef struct {
  char * const c_string;
  unsigned int length;

}String;

/*
 * Function: cleanString
 *
 * takes a string and releases its memory
 *
 * */
void cleanString(String *const);

/*
 * Function: getCommandString
 *
 * reads a string from the STDIN
 *
 * */
String getCommandString();

/*
 * Function: countRepetions
 *
 * takes a char and a String and counts how many times the
 * char appears in the String
 *
 * */
unsigned int countRepetions(const String *const, const char);

/*
 * DataType: ParsedCommand
 *
 * this struct holds a command and its arguments
 * */

typedef struct{
  char * executable;
  char ** arguments;
  unsigned int argc;
}ParsedCommand;

/*
 * Function: ParsedCommand
 *
 * Takes a string and returns its tokens properly parsed
 *
 * */
ParsedCommand ParseCommand(const String * const );

/*
 * Function: cleanParsedCommand
 *
 * takes a command and releases its resources
 *
 * */
void cleanParseCommand(ParsedCommand * const );



