#ifndef __rgb_node_h__
#define __rgb_node_h__

/* rgb node is the base unit.  It holds the RGB and name information of 
    the color node for maipulation by the other modules. 
*/


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


/* Exception Handling
// CALL INFORMATION:

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


const float CONST_RGB_COLOR_VALUE_MIN = 0.0; // Minimum RGB color value
const float CONST_RGB_COLOR_VALUE_MAX = 1.0; // Maximum RGB color value
const string DEFAULT_TEMP_FILE_EXTENTION = "_temp.txt"; // temp file string extention


class rgb_node 
{
public:
    rgb_node() {
        clear(); 
        STRING_error_layer = "RGB_NODE:";
    }

    void clear() { 
        // Inits everything to zero and clears the node name
        red = blue = green = 0.0;
        name.clear();
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

    float get_red() {
        return red;
    }

    float get_green() {
        return green;
    }

    float get_blue() {
        return blue;
    }

    friend ostream& operator<<(ostream &out, const rgb_node &A);

    bool operator == (rgb_node const &A) {
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
            anError <<  STRING_error_layer <<  __PRETTY_FUNCTION__ << ":Value is below " 
                << CONST_RGB_COLOR_VALUE_MIN << " and thus invalid.  Value not set.";
            throw ErrException(anError.str());
        } else if (B > CONST_RGB_COLOR_VALUE_MAX) {
            stringstream anError;
            anError <<  STRING_error_layer <<  __PRETTY_FUNCTION__ << ":Value is above " 
                << CONST_RGB_COLOR_VALUE_MAX << " and thus invalid.  Value not set.";
            throw ErrException(anError.str());

        }
        A = B; // success
    }


    float red;
    float green;
    float blue;
    string name;

    string STRING_error_layer;
};

#endif
