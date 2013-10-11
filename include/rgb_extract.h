#ifndef __rgb_extract_h__
#define __rgb_extract_h__
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
## Filename: rgb_extract.h
##  This file defines the object needed to extract RGB nodes from VRML files.
##
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

    virtual ~rgb_extract() {}

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
    void STATE_seek_DEF(const string &aWord);
    void STATE_seek_Transform(const string &aWord);

    // Seeking the RGB node colors
    void STATE_get_RED(const string &aWord);
    void STATE_get_GREEN(const string &aWord);
    void STATE_get_BLUE(const string &aWord);


    string in_file_name;
    string last_word;
    vector<rgb_node> rgb_list;
    rgb_node temp_node;

};

#endif

