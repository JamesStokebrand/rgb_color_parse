#ifndef __rgb_fileio_h__
#define __rgb_fileio_h__

/* 
    This file includes functions for file manipulations.
*/

#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

class rgb_fileio : public rgb_base
{
public:
    rgb_fileio() 
    : rgb_base("RGB_FILEIO")
    {
        clear();
    }

    rgb_fileio(string const& source, bool temp_file_wanted = false)
    : rgb_base("RGB_FILEIO")
    {
        clear();
        open(source, temp_file_wanted);
    }

    void clear() {
        if (source_file_stream.is_open())
        {
            source_file_stream.close();
        }
        source_file_name.clear();

        if (temp_file_stream.is_open())
        {
            // Close and remove the temp file.
            temp_file_stream.close();
            remove(temp_file_name.c_str());
        }
        temp_file_name.clear();
        temp_file_opened = false;
    }

    void open(string const& source, bool temp_file_wanted = false);
    bool read_word(string &aWord);
    bool read_char(char &aChar);
    void write(string const &A);
    void write(char const &A);
    void overwrite();
    void close();
    void erase();

private:
    string   source_file_name;
    ifstream source_file_stream;
    string   temp_file_name;
    ofstream temp_file_stream;

    bool temp_file_opened;
};


#endif
