#ifndef __rgb_extract_h__
#define __rgb_extract_h__

/*  This file defines the rgb_extract methods for parsing and 
    extracting the rgb color nodes from VRML WRL files.
*/

#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

class rgb_extract : public rgb_base, public rgb_state_word
{
public:
    rgb_extract() 
    : rgb_base("RGB_PARSE")
    , rgb_state_word((STATE)&rgb_extract::STATE_verify_VRML)
    {
        clear();
    }; 

    void clear() {
        in_file_name.clear();
        last_word.clear();
        rgb_list.clear();
        temp_node.clear();
        TRAN((STATE)&rgb_extract::STATE_verify_VRML); // Set the initial state.
    }

    void extract(string const &file_name, string const &rgb_node_file_name="");
    void extract_nodes(string const &file, vector<rgb_node> &rgb_list);
    bool verify(string const &file_name, string const &rgb_node_file_name);

private:
    // Verify its a VRML file
    void STATE_verify_VRML(const string &aWord);
    void STATE_verify_VRML_VER(const string &aWord);
    void STATE_verify_VRML_CHARSET(const string &aWord);

    // Seek the first DEF
    void STATE_verify_seek_DEF(const string &aWord);
    void STATE_verify_seek_Transform(const string &aWord);

    // Seeking the RGB node colors
    void STATE_verify_get_RED(const string &aWord);
    void STATE_verify_get_GREEN(const string &aWord);
    void STATE_verify_get_BLUE(const string &aWord);


    string in_file_name;
    string last_word;
    vector<rgb_node> rgb_list;
    rgb_node temp_node;

};

#endif

