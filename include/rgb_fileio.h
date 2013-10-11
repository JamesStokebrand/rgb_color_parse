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
## Usage:
## ./RGB_color_parse -help
##   - Displays this usage information
## 
## ./RGB_color_parse -extract <a_single_wrl_file> [optional_config_file]
## ./RGB_color_parse -extract <a_directory_containing_wrl_files>
##   - Extracts RGB node information from a single VRML file or all the 
##      VRML files in a directory.
## 
## ./RGB_color_parse -verify <a_single_wrl_file> <required_config_file>
## ./RGB_color_parse -verify <a_directory_containing_wrl_files> <required_config_file>
##   - Verifies that the RGB nodes in a single VRML or all the files in a directory
##      match the ones found in a required RGB config file.
## 
## ./RGB_color_parse -replace <a_single_wrl_file> <required_config_file>
## ./RGB_color_parse -replace <a_directory_containing_wrl_files> <required_config_file>
##   - Replaces the RGB nodes in a single VRML file or all the VRML files found in 
##      a directory.  Requires a RGB config file.
## 
## ./RGB_color_parse -rollback <a_single_wrl_file>
## ./RGB_color_parse -rollback <a_directory_containing_wrl_fles>
##   - Rollsback the RGB nodes previously changed from the "-replace" command.
##      Requires a single VRML file or all the VMRL files found in a directory.
##
## Filename: rgb_fileio.h
##  This file defines the object needed to open, read and write VRML files.
##
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

    virtual ~rgb_fileio() {}

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
