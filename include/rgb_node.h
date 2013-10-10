#ifndef __rgb_node_h__
#define __rgb_node_h__

/* rgb node is the base unit.  It holds the RGB and name information of 
    the color node for maipulation by the other modules. 
*/


#include <string>
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

enum EXECTION_STRING_ARRAY_LEVEL {
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
,{"File or directory not found." ,"Parameter one file or directory does not exist." }
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

/* RGB Base includes the following helper methods:
    - Aid in throwing exceptions.

*/
class rgb_base 
{
public:
    rgb_base(const string &current_object) {
        STRING_error_layer = current_object;
        level = ENUM_BRIEF;
    };

    virtual ~rgb_base() {}

    void set_level(enum EXECTION_STRING_ARRAY_LEVEL x) {
        level = x;
    }

    void throw_exception(
            enum EXCEPTION_STRING_ARRAY x,
            string ss, 
            string debug_method_name,
            string debug_file_name, 
            int debug_line_number)
    {
        stringstream anError;
        if ((level == ENUM_BRIEF) || (level == ENUM_NORMAL))  {
            anError << exception_response[x][level];
        } else {
            anError << EXCEPTION_STRING_PREAMBLE << ss << endl;
        }
        throw_exception(anError.str());
    }

    void throw_exception(
            enum EXCEPTION_STRING_ARRAY x,
            stringstream ss,
            string debug_method_name,
            string debug_file_name, 
            int debug_line_number)
    {
        throw_exception(x, ss.str(), debug_method_name.c_str(), debug_file_name, debug_line_number);
    };

    void throw_exception(string ss)
    {
        throw ErrException(ss.c_str());
    }

    string STRING_error_layer;
    enum EXECTION_STRING_ARRAY_LEVEL level;
};

/* 
    RGB State WORD 
      This is a base class that helps creating state machines to 
      perform word based text processing.
*/
class rgb_state_word 
{
public:
    typedef void (rgb_state_word::*STATE)(const string &aWord);


    void STATE_verify_required_word(string const &ErrorLayer,
            const string &aWord,
            const string &ReqWord,
            STATE nextState)
    {
        if (aWord == ReqWord)
        {
            TRAN(nextState);
        }
        else
        {

            // This is a required word!
            // If it is not found as the first word it is an error.
            stringstream anError;
            if ((level == ENUM_BRIEF) || (level == ENUM_NORMAL))  {
                anError << exception_response[ENUM_PARSE_ERROR][level];
            } else {
                anError << __FILE__ << " at " << __LINE__ << " :" 
                    << ErrorLayer << ":" << __PRETTY_FUNCTION__ 
                    << ": " << "\"" << aWord << "\" found but expected \""
                    << ReqWord << "\".  Please verify the input file.";
            }
            throw ErrException(anError.str());
        }
    }

    // State machine defines.
    rgb_state_word(STATE init) : state(init), level(ENUM_BRIEF) {}
    virtual ~rgb_state_word() {};

    void set_level(enum EXECTION_STRING_ARRAY_LEVEL x) {
        level = x;
    }

    void TRAN(STATE target) { state = static_cast<STATE>(target);}
    void process(const string &aWord) { (this->*state)(aWord); }
    STATE state;
    enum EXECTION_STRING_ARRAY_LEVEL level;
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

    rgb_state_char(STATE init) : state(init), level(ENUM_BRIEF) { 
        word_accumulate.clear();
    }
    virtual ~rgb_state_char() {}

    void set_level(enum EXECTION_STRING_ARRAY_LEVEL x) {
        level = x;
    }

    void STATE_verify_required_word(string const &ErrorLayer,
            const char &aChar,
            const string &ReqWord,
            STATE nextState)
    {
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
                stringstream anError;
                if ((level == ENUM_BRIEF) || (level == ENUM_NORMAL))  {
                    anError << exception_response[ENUM_PARSE_ERROR][level];
                } else {
                    anError << __FILE__ << " at " << __LINE__ << " :"
                        << ErrorLayer << ":" << __PRETTY_FUNCTION__
                        << ": " << "\"" << word_accumulate << "\" found but expected \""
                        << ReqWord << "\".  Please verify the input file.";
                }
                throw ErrException(anError.str());
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
    enum EXECTION_STRING_ARRAY_LEVEL level;

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
class rgb_node : public rgb_base
{
public:
    rgb_node() : rgb_base("RGB_NODE") {
        clear(); 
    }

    virtual ~rgb_node() {}

    void clear() { 
        // Inits everything to zero and clears the node name
        red = blue = green = 0.0;
        name.clear();
    }

    void set_name(string aName) {
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

private:
    void set_color(float &A, float const &B)
    {
        if (B < CONST_RGB_COLOR_VALUE_MIN) 
        {
            stringstream anError;
            anError << "Value is below " << CONST_RGB_COLOR_VALUE_MIN << 
                    " and thus invalid.  Value not set.";
            throw_exception(ENUM_RGB_VALUE_BELOW_ZERO, 
                anError.str(), __PRETTY_FUNCTION__, __FILE__, __LINE__);
        } else if (B > CONST_RGB_COLOR_VALUE_MAX) {
            stringstream anError;
            anError << "Value is above " << CONST_RGB_COLOR_VALUE_MAX << 
                    " and thus invalid.  Value not set.";
            throw_exception(ENUM_RGB_VALUE_ABOVE_ONE, 
                anError.str(), __PRETTY_FUNCTION__, __FILE__, __LINE__);

        }
        A = B; // success
    }

    float red;
    float green;
    float blue;
    string name;

};

#endif
