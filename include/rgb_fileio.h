#ifndef __rgb_fileio_h__
#define __rgb_fileio_h__

/* 
    This file includes functions for file manipulations.
*/

#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

class rgb_fileio
{
public:
    rgb_fileio() {
        source_file_name.clear();
        temp_file_name.clear();
        temp_file_opened = false;
        STRING_error_layer = "RGB_FILEIO:";
    }

    void open(string const& source, bool temp_file_wanted = false);
    string read_word();
    char read_char();
    void write(string const &A);
    void overwrite();
    void close();
    void parse_node_config();
    void create_node_config();

private:
    string   source_file_name;
    ifstream source_file_stream;
    string   temp_file_name;
    ofstream temp_file_stream;

    bool temp_file_opened;

    string STRING_error_layer;
};


#endif
