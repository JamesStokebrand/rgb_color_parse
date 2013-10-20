#ifndef __rgb_node_h__
#define __rgb_node_h__
/*
##
## Copyright Oct 2013 James Stokebkrand
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
## http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## Purpose: 
##  This is a command line tool to extract, replace and rollback RGB nodes 
##   inside VRML V2.0 files.  (File extention WRL)
##
## Filename: rgb_node.h
##  This file defines the base rgb_node and several helper objects that are used
##   to extract, replace and rollback RGB nodes in VRML V2.0 files.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
##
*/
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;


// rgb_node defines
const float CONST_RGB_COLOR_VALUE_MIN = 0.0; // Minimum RGB color value
const float CONST_RGB_COLOR_VALUE_MAX = 1.0; // Maximum RGB color value

// rgb_fileio defines
const string CONST_STRING_DEFAULT_TEMP_FILE_EXTENTION = "_temp.txt"; // temp file string extention
const string CONST_STRING_DEFAULT_RGB_NODE_FILE_EXTENTION = "_rgb_nodes.txt"; // standard RGB node file extention
const string CONST_STRING_DEFAULT_ARGUMENT_ZERO = "./RGB_color_parse"; // standard RGB node file extention

// VRML file defines
const string CONST_STRING_VRML_KEYWORD = "#VRML";
const string CONST_STRING_VRML_VER_KEYWORD = "V2.0";
const string CONST_STRING_VRML_CHARSET_KEYWORD = "utf8";
const string CONST_STRING_DEF_KEYWORD = "DEF";
const string CONST_STRING_TRANSFORM_KEYWORD = "Transform";
const string CONST_STRING_DIFFUSECOLOR_KEYWORD = "diffuseColor";

// CONFIG FILE DEFINES
const string CONST_STRING_CONFIG_START_KEYWORD = "#START";
const string CONST_STRING_CONFIG_CURRENT_VERSION = "V001";
const string CONST_STRING_CONFIG_COMMENT_KEYWORD = "#COMMENT";
const string CONST_STRING_CONFIG_NUM_NODES_KEYWORD = "#NUM_NODES";
const string CONST_STRING_CONFIG_NODE_KEYWORD = "#NODE";
const string CONST_STRING_CONFIG_END_KEYWORD = "#END";

// EXCEPTION STRINGS
//  The enum values must match the strings defined
//  in the exception_string_response

enum EXCEPTION_STRING_ARRAY_LEVEL {
     ENUM_BRIEF=0
    ,ENUM_NORMAL=1
    ,ENUM_VERBOSE=2
};

enum EXCEPTION_STRING_ARRAY {
     ENUM_RGB_VALUE_BELOW_ZERO=0
    ,ENUM_RGB_VALUE_ABOVE_ONE
    ,ENUM_NO_RGB_VALUES_FOUND
    ,ENUM_UNABLE_TO_ALLOCATE_FILEIO
    ,ENUM_RGB_NODES_MATCH
    ,ENUM_NOTHING_TO_ROLLBACK // 5
    ,ENUM_NO_CMD_ARGUMENTS_FOUND
    ,ENUM_UNEXPECTED_COMMAND_PARAMETER
    ,ENUM_UNEXPECTED_COMMAND_SWITCH
    ,ENUM_MISSING_FIRST_PARAMETER
    ,ENUM_MISSING_SECOND_PARAMETER // 10
    ,ENUM_FILE_OR_DIRECTORY_NOT_FOUND
    ,ENUM_UNKNOWN_FILE_TYPE
    ,ENUM_NO_FILES_FOUND_IN_DIRECTORY
    ,ENUM_PARAM_TWO_IS_DIRECTORY
    ,ENUM_PARAM_TWO_NOT_FOUND // 15
    ,ENUM_SOURCE_FILE_ALREADY_OPEN
    ,ENUM_UNABLE_TO_OPEN_SOURCE
    ,ENUM_UNABLE_TO_OPEN_TEMP
    ,ENUM_UNABLE_TO_READ_SOURCE
    ,ENUM_UNABLE_TO_WRITE_SOURCE // 20
    ,ENUM_UNABLE_TO_RENAME_SOURCE
    ,ENUM_UNABLE_TO_RENAME_TEMP
    ,ENUM_UNABLE_TO_DELETE_BACKUP
    ,ENUM_UNABLE_TO_CLOSE_SOURCE
    ,ENUM_UNABLE_TO_READ_CONFIG // 25
    ,ENUM_UNABLE_TO_WRITE_CONFIG
    ,ENUM_PARSE_ERROR  // 27
    ,ENUM_NOTHING_TO_DO // 28

    // Must be last ... used in exception_response string array
    ,ENUM_LAST_ELEMENT
};

const string exception_response[ENUM_LAST_ELEMENT][2] = {
 {"Invalid RGB value - Below Zero", "Invalid RGB value.  Value is below zero." } // 0 
,{"Invalid RGB value - Above 1" , "Invalid RGB value.  Value is above one." }
,{"No RGB nodes found." , "No rgb nodes were extracted from source file." }
,{"Unable to allocate memory." , "Unable to allocate fileIO object." }
,{"RGB nodes match." , "RGB nodes in source match nodes in config file." }
,{"Nothing to rollback." , "No RGB nodes to rollback.  File is original." } // 5
,{"No commands found." , "No command line arguments found." }
,{"Unknown command parameter." ,"Unknown command line parameter found." }
,{"Unexpected command switch." ,"Unexpected command switch found." }
,{"Missing first parameter." ,"Missing first parameter." }
,{"Missing second parameter." ,"Missing second parameter." } // 10
,{"Parameter file or directory not found." ,"The parameter one file or directory does not exist." }
,{"Unknown file type." ,"Parameter one is of unknown file type." }
,{"No files found." ,"No files found in directory." }
,{"File expected but directory found." ,"Parameter two is a directory.  File expected." }
,{"File not found." ,"Parameter two file does not exist." } // 15
,{"Source file already open." ,"Source file is already open." }
,{"Unable to open source." ,"Unable to open source file." } 
,{"Unable to open temp." ,"Unable to open temp file." }
,{"Unable to read source." ,"Unable to read source file.  File not open." }
,{"Unable to write temp." ,"Unable to write temp file.  File not open." } // 20
,{"Unable to rename source." ,"Unable to rename source file." }
,{"Unable to rename temp." ,"Unable to rename temp file." }
,{"Unable to delete backup." ,"Unable to delete backup file." }
,{"Unable to close source." ,"No open file.  Unable to close source file." }
,{"Unable to read config." ,"Unable to read config file." } // 25
,{"Unable to write config." ,"Unable to write config file." }
,{"Not a VRML file." ,"Parse error.  Not a VRML file." }  // 27
,{"No commands to execute.", "No commands were found on command line.  Nothing to do." } // 28
};


#define EXCEPTION_STRING_PREAMBLE  \
endl << "FILE: " << debug_file_name << " line " << \
debug_line_number << endl <<  "METHOD: " << \
debug_method_name << endl << STRING_error_layer << ":ERROR_MSG: " 

#if 0
#define DEBUG_STATE cout << __FUNCTION__  << " " << word_accumulate <<  endl;
#else
#define DEBUG_STATE //
#endif

#if 0
#define DEBUG_LOGGER_REFGET cout << __FUNCTION__ << " GET REF: " << _referenceCount << endl;
#define DEBUG_LOGGER_REFRELEASE cout << __FUNCTION__ << " RELEASE REF: " << _referenceCount << endl;
#define DEBUG_LOGGER_LEVEL cout << "LOGGER LEVEL :" << level << " " << this << endl;
#else
#define DEBUG_LOGGER_REFGET //
#define DEBUG_LOGGER_REFRELEASE //
#define DEBUG_LOGGER_LEVEL //
#endif

/* Exception Handling */
class ErrException : public exception
{
public:
   ErrException(string ss) : s(ss) {}
   virtual ~ErrException() throw() {}
   const char* what() const throw() { return s.c_str(); }

private:
   string s;
};

// Singleton class to hold the logger level.
//  This allows it to be set and read by everyone.
class LoggerLevel { 
public: 
    static LoggerLevel* getInstance() { 
        if (NULL == _instance) { 
            _instance = new LoggerLevel(); 
        } 
        _referenceCount++; 
        DEBUG_LOGGER_REFGET
        return _instance; 
    }
 
    static void releaseInstance() { 
        _referenceCount--; 
        DEBUG_LOGGER_REFRELEASE
        if ((0 == _referenceCount) && (NULL != _instance)) { 
            delete _instance; 
            _instance = NULL; 
        } 
    }

    enum EXCEPTION_STRING_ARRAY_LEVEL getLevel() { return level; }
    void setLevel(enum EXCEPTION_STRING_ARRAY_LEVEL new_level) {
        level = new_level;
    }

    void throw_exception(
            enum EXCEPTION_STRING_ARRAY const &x,
            string const &ss,
            string const &debug_method_name,
            string const &debug_file_name,
            int const &debug_line_number,
            string const &STRING_error_layer)
    {
        DEBUG_LOGGER_LEVEL
        stringstream anError;
        if ((level == ENUM_BRIEF) ||
            (level == ENUM_NORMAL))  {
            anError << exception_response[x][level];
        } else {
            anError << EXCEPTION_STRING_PREAMBLE << ss << endl;
        }
        throw_exception(anError.str());
    }

    void throw_exception(
            enum EXCEPTION_STRING_ARRAY const &x,
            stringstream const &ss,
            string const &debug_method_name,
            string const &debug_file_name,
            int const &debug_line_number,
            string const &STRING_error_layer)
    {
        throw_exception(x,
                ss.str(), 
                debug_method_name.c_str(),
                debug_file_name,
                debug_line_number,
                STRING_error_layer);
    };

    void throw_exception(string ss)
    {
        throw ErrException(ss.c_str());
    }
 
private: 
    LoggerLevel(): level(ENUM_BRIEF) { }
    virtual ~LoggerLevel() {}
    LoggerLevel(const LoggerLevel&) {}
    LoggerLevel& operator=(const LoggerLevel&) { 
        return *this; 
    }
 
    static int _referenceCount; 
    static LoggerLevel* _instance;
    enum EXCEPTION_STRING_ARRAY_LEVEL level;
};
 

/* 
    RGB State WORD 
      This is a base class that helps create state machines.  This
      aids in perform word based text processing.
*/
class rgb_state_word 
{
public:
    //  STATE method pointer typedef
    typedef void (rgb_state_word::*STATE)(const string &aWord);

    void STATE_verify_required_word(string const &ErrorLayer,
            const string &aWord,
            const string &ReqWord,
            STATE nextState) {
        if (aWord == ReqWord)
        {
            TRAN(nextState);
        }
        else
        {
            // This is a required word!
            // If it is not found as the first word it is an error.
            string anError = "";
            enum EXCEPTION_STRING_ARRAY x = ENUM_PARSE_ERROR;
            if ((aLogger->getLevel() == ENUM_BRIEF) || 
                (aLogger->getLevel() == ENUM_NORMAL))  {
                anError = exception_response[x][aLogger->getLevel()];
            } else {
                // Is this word printable?
                bool breakout = false;
                for (unsigned int ll=0; ll < aWord.size(); ll++)
                {
                    if (!isprint(aWord[ll]))
                    {
                        anError = "Binary data found but expected \""
                            + ReqWord + "\".  Please verify the input file.";
                        breakout=true;
                    }
                    if (breakout) break;
                }

                if (!breakout) {
                    anError = "\"" + aWord + "\" found but expected \""
                        + ReqWord + "\".  Please verify the input file.";
                }

                // Throw it
                aLogger->throw_exception(x, anError, __PRETTY_FUNCTION__,
                    __FILE__, __LINE__, ErrorLayer);
            }
        }
    }

    // State machine defines.
    rgb_state_word(STATE init) : state(init) {
        aLogger = LoggerLevel::getInstance();
    }
    virtual ~rgb_state_word() {
        aLogger->releaseInstance();
    };

    void TRAN(STATE target) { state = static_cast<STATE>(target);}
    void process(const string &aWord) { (this->*state)(aWord); }
    STATE state;

    LoggerLevel* aLogger;
};

/* 
    RGB State CHAR
      This is a base class that helps creating state machines to 
      perform char based text processing.
*/
class rgb_state_char
{
public:
    typedef void (rgb_state_char::*STATE)(const char &aChar);

    rgb_state_char(STATE init)
    : state(init) {
        word_accumulate.clear();
        aLogger = LoggerLevel::getInstance();
    }
    virtual ~rgb_state_char() {
        aLogger->releaseInstance();
    }

    void STATE_verify_required_word(string const &ErrorLayer,
            const char &aChar,
            const string &ReqWord,
            STATE nextState)
    {
        // Have we found a spacer char or is the accumulated word greater
        //  than the required word?
        if (isspace(aChar))
        {
            if (word_accumulate == ReqWord)
            {
                TRAN(nextState);
                temp_string << word_accumulate << aChar;
                word_accumulate.clear();
            }
            else
            {
                // This is a required word to verify the source file
                // is a VRML file.  NOT finding this word is an error.
                string anError = "";
                enum EXCEPTION_STRING_ARRAY x = ENUM_PARSE_ERROR;
                if ((aLogger->getLevel() == ENUM_BRIEF) ||
                    (aLogger->getLevel() == ENUM_NORMAL))  {
                    anError = exception_response[ENUM_PARSE_ERROR][aLogger->getLevel()];
                } else {

                    // Is this word printable?
                    bool breakout=false;

                    for (unsigned int ll=0; ll < word_accumulate.size(); ll++)
                    {
                        if (!isprint(word_accumulate[ll]))
                        {
                            word_accumulate = "BINARY";
                            breakout = true;
                            break;
                        }
                        if (breakout) break;
                    }

                        anError = "\"" + word_accumulate + "\" found but expected \"" + 
                            ReqWord + "\".  Please verify the input file.";
                    }

                    aLogger->throw_exception(x, anError, __PRETTY_FUNCTION__,
                        __FILE__, __LINE__, ErrorLayer);
                }
            }
            else
            {
                word_accumulate += aChar;
            }
        }

    // State machine defines.
    void TRAN(STATE target) { state = static_cast<STATE>(target);}
    void process(const char &aChar) { (this->*state)(aChar); }
    STATE state;

    LoggerLevel* aLogger;

    string word_accumulate;
    stringstream temp_string;
};


// This is a template class for printing out vector tables
template < class T >
std::ostream& operator << (std::ostream& os, const std::vector<T>& v) 
{
    os << "[";
    for (typename std::vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << endl << " " << *ii;
    }
    os << endl << "]" << endl;
    return os;
}

// This class holds the RGB and node name values found in the
//  VRML files.
class rgb_node
{
public:
    rgb_node() : STRING_error_layer("RGB_NODE") {
        aLogger = LoggerLevel::getInstance();
        clear(); 
    }

    virtual ~rgb_node() {
        aLogger->releaseInstance();
    }

    void clear() { 
        // Inits everything to zero and clears the node name
        red = blue = green = 0.0;
        name.clear();
    }

    void set_name(string aName) {
        // NOTE that the name is not used in text or state machine processing.
        name = aName;
    }

    void set_red(float const &A) {
        set_color(red,A);
    }

    void set_green(float const &A) {
        set_color(green,A);
    }

    void set_blue(float const &A) {
        set_color(blue,A);
    }

    string get_name() const {
        return name;
    }

    float get_red() const {
        return red;
    }

    float get_green() const {
        return green;
    }

    float get_blue() const {
        return blue;
    }

    friend ostream& operator<<(ostream &out, const rgb_node &A);

    bool operator == (rgb_node const &A) const {
        if ((A.red == red) && 
            (A.green == green) && 
            (A.blue == blue))
        {
            return true;
        }
        return false;
    }

    rgb_node& operator = (rgb_node const &A) {
        if (this != &A)
        {
            name = A.name;
            red = A.red;
            green = A.green;
            blue = A.blue;
        }
        return *this;
    }

    // Copy constructor
    rgb_node( const rgb_node& other )
    : STRING_error_layer("RGB_NODE")
    , red(other.red)
    , green(other.green)
    , blue(other.blue) 
    , name(other.name) {
        aLogger->getInstance();
    }

private:
    void set_color(float &A, float const &B)
    {
        if (B < CONST_RGB_COLOR_VALUE_MIN) 
        {
            stringstream anError;
            anError << "Value is below " << CONST_RGB_COLOR_VALUE_MIN << 
                    " and thus invalid.  Value not set.";
            aLogger->throw_exception(ENUM_RGB_VALUE_BELOW_ZERO, anError.str(),
                __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
        } else if (B > CONST_RGB_COLOR_VALUE_MAX) {
            stringstream anError;
            anError << "Value is above " << CONST_RGB_COLOR_VALUE_MAX << 
                    " and thus invalid.  Value not set.";
            aLogger->throw_exception(ENUM_RGB_VALUE_ABOVE_ONE, anError.str(), 
                __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);

        }
        A = B; // success
    }

    string STRING_error_layer;

    float red;
    float green;
    float blue;
    string name;

    LoggerLevel *aLogger;
};

#endif

