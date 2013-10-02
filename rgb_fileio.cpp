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

    if (source_file_stream.is_open())
    {
        // A file is already open! This is an error!
        throw_exception("Unable to open \"" + source + 
            "\".  File \"" + source_file_name + "\" has already been opened.",
            __PRETTY_FUNCTION__);
    }

    source_file_stream.open(source.c_str());
    if (!source_file_stream.is_open())
    {
        // Failed to open source file.  Return error.
        throw_exception("Unable to open input file \"" + source + "\".", __PRETTY_FUNCTION__);
    }

    source_file_name = source;
        
    if (temp_file_wanted) 
    {
        // Open the requested temp file for writing.
        temp_file_name = source + CONST_STRING_DEFAULT_TEMP_FILE_EXTENTION;
        temp_file_stream.open(temp_file_name.c_str());

        if (!temp_file_stream.is_open())
        {
            // Failed to open the temp file.  Return error.
            throw_exception("Unable to open temp file \"" + temp_file_name + "\".", __PRETTY_FUNCTION__);
        }
        temp_file_opened = true;
    }
}

bool rgb_fileio::read_word(string &aWord)
{

    if (!source_file_stream.is_open())
    {
        // There is no open file! ... This is an error.
        throw_exception("Unable to read.  There is no open file.", __PRETTY_FUNCTION__);
    }

    // Read one word from the original file.
    if((source_file_stream >> aWord) &&
        source_file_stream.good())
    {
        return true;
    }
    return false;
}

bool rgb_fileio::read_char(char &aChar)
{
    if (!source_file_stream.is_open())
    {
        // There is no open file! ... This is an error.
        throw_exception("Unable to read.  There is no open file.", __PRETTY_FUNCTION__); 
    }

    // Will read one char from the original file.
    if ( !source_file_stream.eof() && 
        (aChar = source_file_stream.get()) &&
         source_file_stream.good())
    {
        return true;
    }
    return false;
}

void rgb_fileio::write(string const &A)
{
    // Will write the string to the temp file.
    //  Fails if a temp file was not created at open()
    if (!temp_file_opened) {
        throw_exception("Unable to write.  Temp file was not created at open().", __PRETTY_FUNCTION__);
    } else {
        temp_file_stream << A;
    }
}

void rgb_fileio::write(char const &A)
{
    // Will write the string to the temp file.
    //  Fails if a temp file was not created at open()
    if (!temp_file_opened) {
        throw_exception("Unable to write.  Temp file was not created at open().", __PRETTY_FUNCTION__);
    } else {
        temp_file_stream << A;
    }
}

void rgb_fileio::overwrite()
{   
    // Moves the original file to a bak status.
    // Changes the name of the temp file to the original file.
    // Deletes the original file because the above operation succeeded.
/*
    if (!temp_file_opened)
    {
        throw_exception("Unable to overwrite.  Temp file was not created at open().", __PRETTY_FUNCTION__);
    }
*/
    string bak_file(source_file_name + ".bak");
    int result= rename( source_file_name.c_str(), bak_file.c_str());
    if ( result != 0 ) {
        throw_exception("Unable to overwrite \"" + source_file_name + 
            "\" with temp file \"" + temp_file_name + "\".", __PRETTY_FUNCTION__);
    }
    result = rename( temp_file_name.c_str(), source_file_name.c_str());
    if ( result != 0 ) {
        throw_exception("Unable to rename \"" + temp_file_name + 
            "\" with to \"" + source_file_name + "\".", __PRETTY_FUNCTION__);
    }
    result = remove(bak_file.c_str());
    if ( result != 0 ) {
        throw_exception("Unable to delete \"" + bak_file + "\".", __PRETTY_FUNCTION__);
    }

}

void rgb_fileio::close()
{
    if (!source_file_stream.is_open())
    {
        // There is no open file! ... This is an error.
        throw_exception("Unable to close.  There is no open file.", __PRETTY_FUNCTION__); 
    }

    // Close the temp and source file.
    source_file_stream.close();

    if (temp_file_opened) {
        temp_file_stream.close();
        temp_file_opened = false;
    }
}



