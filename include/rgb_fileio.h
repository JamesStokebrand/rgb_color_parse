#ifndef __rgb_fileio_h__
#define __rgb_fileio_h__
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
## Filename: rgb_fileio.h
##  This file defines the object needed to open, read and write VRML files.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
##
*/
#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

class rgb_fileio
{
public:
    rgb_fileio() 
    : STRING_error_layer("RGB_FILEIO")
    {
        aLogger = LoggerLevel::getInstance();
        clear();
    }

    rgb_fileio(string const& source, bool temp_file_wanted = false)
    : STRING_error_layer("RGB_FILEIO")
    {
        aLogger = LoggerLevel::getInstance();
        clear();
        open(source, temp_file_wanted);
    }

    virtual ~rgb_fileio() {
        aLogger->releaseInstance();
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

    string STRING_error_layer;
    LoggerLevel *aLogger;
};


#endif
