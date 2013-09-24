/* 
    This file holds the functions for file manipulations.
*/

#ifndef __rgb_fileio_h__
#include "include/rgb_fileio.h"
#endif

void rgb_fileio::open(string const& source, bool temp_file_wanted)
{
    // Opens a WRL file for reading.  
    // If temp_file_wanted was TRUE will create a temp output file for writing.

    source_file_stream.open(source.c_str());
    if (!source_file_stream.is_open())
    {
        // Failed to open source file.  Return error.
        string anError(STRING_error_layer + __PRETTY_FUNCTION__ + " Unable to open input file \"" 
            + source + "\".");
        throw ErrException(anError);
    }

    source_file_name = source;
        
    if (temp_file_wanted) 
    {
        // Open the requested temp file for writing.
        temp_file_name = source + DEFAULT_TEMP_FILE_EXTENTION;
        temp_file_stream.open(temp_file_name.c_str());

        if (!temp_file_stream.is_open())
        {
            // Failed to open the temp file.  Return error.
            string anError(STRING_error_layer + __PRETTY_FUNCTION__ + " Unable to open temp file \"" 
            + temp_file_name + "\".");
            throw ErrException(anError);
        }
        temp_file_opened = true;
    }
}

string rgb_fileio::read_word()
{
    return "";
    // Will read one word from the original file.
}

char rgb_fileio::read_char()
{
    return 0;
    // Will read one char from the original file.
}

void rgb_fileio::write(string const &A)
{
    // Will write one word to the temp file.
    //  Fails if a temp file was not created at open()
}

void rgb_fileio::overwrite()
{   
    // Moves the original file to a bak status.
    // Changes the name of the temp file to the original file.
    // Deletes the original file because the above operation succeeded.
}

void rgb_fileio::close()
{
    // Close the temp and source file.
    source_file_stream.close();
    if (temp_file_opened) {
        temp_file_stream.close();
        temp_file_opened = false;
    }
}

void rgb_fileio::parse_node_config() 
{
}

void rgb_fileio::create_node_config()
{
}


