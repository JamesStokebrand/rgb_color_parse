#ifndef __rgb_rollback_h__
#define __rgb_rollback_h__
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
## Filename: rgb_rollback.h
##  This file defines the object needed to go back to an earlier RGB node set placed
##   in this file by the "-replace" command.
##
*/
#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

#ifndef __rgb_fileio_h__
#include "rgb_fileio.h"
#endif

#ifndef __rgb_configio_h__
#include "rgb_configio.h"
#endif

class rgb_rollback
: rgb_base
, rgb_state_char
{
public:
    rgb_rollback()
        : rgb_base("RGB_ROLLBACK") 
        , rgb_state_char((STATE)&rgb_rollback::STATE_verify_VRML)
    {
        srcFileIO = NULL;
    }

    virtual ~rgb_rollback() {
        if (srcFileIO) delete srcFileIO;
    }

    void clear()
    {
        if (srcFileIO) srcFileIO->clear();
        source_file.clear();
        TRAN((STATE)&rgb_rollback::STATE_verify_VRML);
        aConfigListing.clear();
        config_listings.clear();
        word_accumulate.clear();
    }

    void rollback(const string &source);

private:
    // Verify its a VRML file
    void STATE_verify_VRML(const char &aChar);
    void STATE_verify_VRML_VER(const char &aChar);
    void STATE_verify_VRML_CHARSET(const char &aChar);

    // Seek the START and END keywords that contain the config listing
    void STATE_seek_START(const char &aChar);
    void STATE_seek_END(const char &aChar);

    // Seeking the RGB node colors
    void STATE_seek_DIFFUSECOLOR(const char &aChar);
    void STATE_get_RED(const char &aChar);
    void STATE_get_GREEN(const char &aChar);
    void STATE_get_BLUE(const char &aChar);

    // If there are no nodes left in the vector do noops till
    //  the file is exhausted.
    void STATE_NOOP(const char &aChar);

    void Process_Config_Listings();

    string source_file;
    vector<rgb_node> config_block_nodes;
    rgb_fileio* srcFileIO;

    string aConfigListing;
    vector<string> config_listings;
};

#endif
