#ifndef __rgb_replace_h__
#define __rgb_replace_h__

#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

#ifndef __rgb_fileio_h__
#include "rgb_fileio.h"
#endif

class rgb_replace 
: public rgb_base
, public rgb_state_char
{
public:
    rgb_replace() 
    : rgb_base("RGB_REPLACE")
    , rgb_state_char((STATE)&rgb_replace::STATE_verify_VRML)
    {}

    virtual ~rgb_replace() {}

    void replace(string const &rgb_file, string const &rgb_config_file);


private:
    // Verify its a VRML file
    void STATE_verify_VRML(const char &aChar);
    void STATE_verify_VRML_VER(const char &aChar);
    void STATE_verify_VRML_CHARSET(const char &aChar);

    // Seek the first DEF
    void STATE_seek_DEF(const char &aChar);
    void STATE_seek_Transform(const char &aChar);

    // Seeking the RGB node colors
    void STATE_get_RED(const char &aChar);
    void STATE_get_GREEN(const char &aChar);
    void STATE_get_BLUE(const char &aChar);

    void STATE_NOOP(const char &aChar);

};

#endif
