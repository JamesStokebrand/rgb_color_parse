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

#define EXCEPTION_STRING_PREAMBLE  \
endl << "FILE: " << debug_file_name << " line " << \
debug_line_number << endl <<  "METHOD: " << \
debug_method_name << endl << STRING_error_layer << ":ERROR_MSG: " 


/* Exception Handling
// Call them similar to this ...

string anError(STRING_error_layer + __PRETTY_FUNCTION__ + ":" + "BLAH");
throw ErrException(anError);

stringstream anError;
anError <<  STRING_error_layer <<  __PRETTY_FUNCTION__ << ":" << "BLAH";
throw ErrException(anError.str());


*/
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
    };

    void throw_exception(string ss, 
            string debug_method_name,
            string debug_file_name = __FILE__, 
            int debug_line_number = __LINE__) {
        stringstream anError;
        anError << EXCEPTION_STRING_PREAMBLE << ss << endl;
        throw ErrException(anError.str());        
    }

    void throw_exception(stringstream ss, string debug_method_name) {
        throw_exception(ss.str(), debug_method_name.c_str());
    };

    string STRING_error_layer;

};

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

            anError << __FILE__ << " at " << __LINE__ << " :" 
                << ErrorLayer << ":" << __PRETTY_FUNCTION__ 
                << ": " << "\"" << aWord << "\" found but expected \""
                << ReqWord << "\".  Please verify the input file.";

/* // DOESNT WORK!! DANG!
            anError << EXCEPTION_STRING_PREAMBLE << "\"" << aWord << 
                    "\" found but expected \"" << ReqWord << 
                    "\".  Please verify the input file.";
*/

            throw ErrException(anError.str());
        }
    }


    // State machine defines.
    rgb_state_word(STATE init) : state(init) {}
    virtual ~rgb_state_word() {};

    void TRAN(STATE target) { state = static_cast<STATE>(target);}
    void process(const string &aWord) { (this->*state)(aWord); }
    STATE state;
};

class rgb_state_char
{
public:
    typedef void (rgb_state_char::*STATE)(const char &aChar);

    rgb_state_char(STATE init) : state(init) { word_accumulate.clear(); }
    virtual ~rgb_state_char() {};

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
                anError << __FILE__ << " at " << __LINE__ << " :"
                    << ErrorLayer << ":" << __PRETTY_FUNCTION__
                    << ": " << "\"" << aChar << "\" found but expected \""
                    << ReqWord << "\".  Please verify the input file.";

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

    string word_accumulate;
    stringstream temp_string;
};



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

class rgb_node : public rgb_base
{
public:
    rgb_node() : rgb_base("RGB_NODE") {
        clear(); 
    }

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
            throw_exception(anError.str(), __PRETTY_FUNCTION__);
        } else if (B > CONST_RGB_COLOR_VALUE_MAX) {
            stringstream anError;
            anError << "Value is above " << CONST_RGB_COLOR_VALUE_MAX << 
                    " and thus invalid.  Value not set.";
            throw_exception(anError.str(), __PRETTY_FUNCTION__);

        }
        A = B; // success
    }

    float red;
    float green;
    float blue;
    string name;

};

#endif
