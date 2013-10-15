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
## Filename: rgb_configio.cpp
##  This file defines the methods used to create and parse RGB config files.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
##
*/
#ifndef __rgb_configio_h__
#include "include/rgb_configio.h"
#endif

#if 0
#define DEBUG_BLAH cout << __PRETTY_FUNCTION__ << aWord << endl;
#else
#define DEBUG_BLAH //
#endif


void rgb_configio::parse_node_config(string const &aNodeConfig,
    vector<rgb_node> &node_vector) 
{
    // Determine if the aNodeConfig is a long string that contains the
    //  RGB config information or a filename which can be opened
    stringstream inputSS(aNodeConfig);
    string aWord;

    // Grab one word from the input string
    inputSS >> aWord;

    if (aWord == CONST_STRING_CONFIG_START_KEYWORD)
    {
        // First word is the #START keyword.  This is a string containing
        //  a RGB node config.

        // Send this first word to the state machine.
        process(aWord);

        // Process the rest of the string
        while (inputSS >> aWord)
        {
            process(aWord);
        }
    } 
    else // Attempt to open it as a filename.
    {

        ifstream node_config_file(aNodeConfig.c_str());

        if (!node_config_file)
        {
            // Unable to open the config file for reading....
            //  This is an error.
            aLogger->throw_exception(ENUM_UNABLE_TO_READ_CONFIG,
                "Unable to open node config for reading.  Check the input config file.",
                __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
        }

        while ((node_config_file >> aWord) &&
                node_config_file.good())
        {
            process(aWord);
        }
    }

//cout << "Expected: " << parse_config_number_of_nodes << " Read :"  << parse_config_vector.size() << endl;

    // Success
    node_vector = parse_config_vector;
    clear(); // Clear out existing code
    
}


void rgb_configio::create_node_config(vector<rgb_node> const &node_vector,
    string const &source_file,
    string &aNodeConfig)
{
    // This assumes that the node_vector is not empty.

    // Config file will be in the form:
    // #START V001
    // #COMMENT __COMMENT_HERE__
    // #NODES __NUMBER_OF_NODES__  
    // #NODE node_name RED GREEN BLUE
    // #END 
    // it will include all following whitespace

    stringstream config;
    config.clear();

    // Construct the config file 
    config << CONST_STRING_CONFIG_START_KEYWORD << " " << CONST_STRING_CONFIG_CURRENT_VERSION << "\n";
    config << CONST_STRING_CONFIG_COMMENT_KEYWORD << " source file : " << source_file << "\n";

    // Create and add a timestamp
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    config << CONST_STRING_CONFIG_COMMENT_KEYWORD << " created : " << asctime (timeinfo);

    config << CONST_STRING_CONFIG_NUM_NODES_KEYWORD << " " << node_vector.size() << "\n";
    //for (vector<rgb_node>::iterator it = node_vector.begin(); it < node_vector.end(); it++ ) {
    for (int ii = 0; ii < node_vector.size(); ii++) {
        config << CONST_STRING_CONFIG_NODE_KEYWORD << " ";
        config << node_vector[ii].get_name() << " ";
        config << node_vector[ii].get_red() << " ";
        config << node_vector[ii].get_green() << " ";
        config << node_vector[ii].get_blue() << endl;
    }
    config << CONST_STRING_CONFIG_END_KEYWORD << endl << endl;

    aNodeConfig = config.str();
}

void rgb_configio::write_node_config_file(vector<rgb_node> const &node_vector,
    string const &source_file,
    string const &output_file)
{
    string aConfig;
    create_node_config(node_vector, source_file, aConfig);

    // Write config file to the output_file
    ofstream out_file(output_file.c_str(), ios::out | ios::trunc);

    if (out_file.is_open()) {
        out_file << aConfig;
        out_file.close();
    }
    else 
    {
        aLogger->throw_exception(ENUM_UNABLE_TO_WRITE_CONFIG,
            "Unable to open node config file for writing.  Is the directory full?", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    } 
}

void rgb_configio::STATE_seek_START(string const &aWord)
{
DEBUG_BLAH
    STATE_verify_required_word(
        STRING_error_layer,
        aWord,
        CONST_STRING_CONFIG_START_KEYWORD,
        (STATE)&rgb_configio::STATE_seek_CONFIG_VERSION);
}

void rgb_configio::STATE_seek_CONFIG_VERSION(string const &aWord)
{
DEBUG_BLAH
    STATE_verify_required_word(
        STRING_error_layer,
        aWord,
        CONST_STRING_CONFIG_CURRENT_VERSION,
        (STATE)&rgb_configio::STATE_seek_NUM_NODES_KEYWORD);
}

void rgb_configio::STATE_seek_NUM_NODES_KEYWORD(string const &aWord)
{
DEBUG_BLAH
    if (aWord == CONST_STRING_CONFIG_NUM_NODES_KEYWORD)
    {
        // #NUM_NODES keyword found ... read the number of
        //  nodes to find.
        TRAN((STATE)&rgb_configio::STATE_read_NUM_NODES);
    }
}


void rgb_configio::STATE_read_NUM_NODES(string const &aWord)
{
    parse_config_number_of_nodes = atoi(aWord.c_str());
    TRAN((STATE)&rgb_configio::STATE_read_NODE_KEYWORD);
}

void rgb_configio::STATE_read_NODE_KEYWORD(string const &aWord)
{
    if (aWord == CONST_STRING_CONFIG_NODE_KEYWORD)
    {
        // Start of a #NODE <NAME> <R> <G> <B> sequence
        TRAN((STATE)&rgb_configio::STATE_read_NODE_NAME);
    }
    if (aWord == CONST_STRING_CONFIG_END_KEYWORD)
    {
        // #END keyword means this config is finished.
        // transition to NOOP and do nothing else
        TRAN((STATE)&rgb_configio::STATE_NOOP);
    }
}

void rgb_configio::STATE_read_NODE_NAME(string const &aWord)
{
    parse_temp_node.set_name(aWord);
    TRAN((STATE)&rgb_configio::STATE_read_RED);
}

void rgb_configio::STATE_read_RED(string const &aWord)
{
    parse_temp_node.set_red(atof(aWord.c_str()));
    TRAN((STATE)&rgb_configio::STATE_read_GREEN);
}

void rgb_configio::STATE_read_GREEN(string const &aWord)
{
    parse_temp_node.set_green(atof(aWord.c_str()));
    TRAN((STATE)&rgb_configio::STATE_read_BLUE);
}

void rgb_configio::STATE_read_BLUE(string const &aWord)
{
    parse_temp_node.set_blue(atof(aWord.c_str()));
    TRAN((STATE)&rgb_configio::STATE_read_NODE_KEYWORD);

    // Save the completed RGB node in the vector
    parse_config_vector.push_back(parse_temp_node);
    parse_temp_node.clear();
}

void rgb_configio::STATE_NOOP(string const &aWord)
{
    // Do nothing...
}


