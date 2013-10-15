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
## Filename: rgb_fileio.cpp
##  This file defines the methods used to read and write files.  Abstraction layer
##   to help in file management and parsing.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
##
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
        aLogger->throw_exception(ENUM_SOURCE_FILE_ALREADY_OPEN,
            "Unable to open \"" + source + 
            "\".  File \"" + source_file_name + "\" has already been opened.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

    source_file_stream.open(source.c_str());
    if (!source_file_stream.is_open())
    {
        // Failed to open source file.  Return error.
        aLogger->throw_exception(ENUM_UNABLE_TO_OPEN_SOURCE,
            "Unable to open input file \"" + source + "\".", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
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
            aLogger->throw_exception(ENUM_UNABLE_TO_OPEN_TEMP,
                "Unable to open temp file \"" + temp_file_name + "\".", 
                __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
        }
        temp_file_opened = true;
    }
}

bool rgb_fileio::read_word(string &aWord)
{
    // This method reads the file one word at a time.

    if (!source_file_stream.is_open())
    {
        // There is no open file! ... This is an error.
        aLogger->throw_exception(ENUM_UNABLE_TO_READ_SOURCE ,
                "Unable to read.  There is no open file.", 
                __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
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
        aLogger->throw_exception(ENUM_UNABLE_TO_READ_SOURCE, 
            "Unable to read.  There is no open file.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer); 
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
        aLogger->throw_exception(ENUM_UNABLE_TO_WRITE_SOURCE,
            "Unable to write.  Temp file was not created at open().", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    } else {
        temp_file_stream << A;
    }
}

void rgb_fileio::write(char const &A)
{
    // Will write the string to the temp file.
    //  Fails if a temp file was not created at open()
    if (!temp_file_opened) {
        aLogger->throw_exception(ENUM_UNABLE_TO_WRITE_SOURCE,
            "Unable to write.  Temp file was not created at open().", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    } else {
        temp_file_stream << A;
    }
}

void rgb_fileio::overwrite()
{   
    // Moves the original file to a bak status.
    // Changes the name of the temp file to the original file.
    // Deletes the original file because the above operation succeeded.

    string bak_file(source_file_name + ".bak");
    int result= rename( source_file_name.c_str(), bak_file.c_str());
    if ( result != 0 ) {
        aLogger->throw_exception(ENUM_UNABLE_TO_RENAME_SOURCE,
            "Unable to rename \"" + source_file_name + 
            "\" to \"" + bak_file + "\".", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }
    result = rename( temp_file_name.c_str(), source_file_name.c_str());
    if ( result != 0 ) {
        aLogger->throw_exception(ENUM_UNABLE_TO_RENAME_TEMP,
            "Unable to rename \"" + temp_file_name + 
            "\" with to \"" + source_file_name + "\".", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }
    result = remove(bak_file.c_str());
    if ( result != 0 ) {
        aLogger->throw_exception(ENUM_UNABLE_TO_DELETE_BACKUP,
            "Unable to delete \"" + bak_file + "\".", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

}

void rgb_fileio::close()
{
    if (!source_file_stream.is_open())
    {
        // There is no open file! ... This is an error.
        aLogger->throw_exception(ENUM_UNABLE_TO_CLOSE_SOURCE,
            "Unable to close.  There is no open file.", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer); 
    }

    // Close the temp and source file.
    source_file_stream.close();

    if (temp_file_opened) {
        temp_file_stream.close();
        temp_file_opened = false;
    }
}

void rgb_fileio::erase()
{
    // Close and erase the unwanted temp file.
    if (temp_file_opened) {
        temp_file_stream.close();
        temp_file_opened = false;

        remove(temp_file_name.c_str());
    }
}


