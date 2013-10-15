#ifndef __rgb_replace_h__
#define __rgb_replace_h__
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
## Filename: rgb_replace.h
##  This file defines the object needed to replace RGB nodes in a VRML file.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
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


class rgb_replace 
: public rgb_state_char
{
public:
    rgb_replace() 
    : STRING_error_layer("RGB_REPLACE")
    , rgb_state_char((STATE)&rgb_replace::STATE_verify_VRML) {
        aLogger = LoggerLevel::getInstance();
        srcFileIO = NULL;
    }

    virtual ~rgb_replace() { 
        if (srcFileIO) delete srcFileIO;
        aLogger->releaseInstance();
    }

    void clear()
    {
        word_accumulate.clear();
        srcFileIO = NULL;
        existing_node_config.clear();
        config_node_vector.clear();
        TRAN((STATE)&rgb_replace::STATE_verify_VRML);
    }

    void replace(string const &rgb_file, string const &rgb_config_file);

private:
    // Verify its a VRML file
    void STATE_verify_VRML(const char &aChar);
    void STATE_verify_VRML_VER(const char &aChar);
    void STATE_verify_VRML_CHARSET(const char &aChar);

    // Seek the first DEF
    void STATE_seek_DEF(const char &aChar);

    // Seeking the RGB node colors
    void STATE_seek_DIFFUSECOLOR(const char &aChar);
    void STATE_get_RED(const char &aChar);
    void STATE_get_GREEN(const char &aChar);
    void STATE_get_BLUE(const char &aChar);

    // If there are no nodes left in the vector do noops till
    //  the file is exhausted.
    void STATE_NOOP(const char &aChar);

    rgb_fileio *srcFileIO;
    string existing_node_config;
    vector<rgb_node> config_node_vector;

    string STRING_error_layer;
    LoggerLevel *aLogger;
};

#endif
