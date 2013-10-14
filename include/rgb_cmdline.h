#ifndef __rgb_cmdline_h__
#define __rgb_cmdline_h__
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
## Filename: rgb_cmdline.h
##  This file defines the command line object that allows this program
##   to parse and execute commands from the command line.
##
*/
#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

#ifndef __rgb_extract_h__
#include "rgb_extract.h"
#endif

#ifndef __rgb_replace_h__
#include "rgb_replace.h"
#endif

#ifndef __rgb_rollback_h__
#include "rgb_rollback.h"
#endif

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

class rgb_cmdline
{
public:
    class rgb_command;
    class rgb_param_pair;

    rgb_cmdline() : STRING_error_layer("RGB_CMDLINE") {
        aLogger = LoggerLevel::getInstance();
    }

    virtual ~rgb_cmdline() {
        aLogger->releaseInstance();
    }

    void static print_usage();

    void setup(vector<string> &aCmdParams, vector<rgb_command *> &aListOfCmds);
    void parse_execute(const int &argc, char *argv[]);

    void cleanup(vector<rgb_command *> &aListOfCmds)
    {
        for (vector<rgb_command *>::iterator it = aListOfCmds.begin();
                it != aListOfCmds.end(); it++)
        {
            // Delete the list of commands.
            delete *it;
        }
        aListOfCmds.clear();
    }
    
    // This is the generic command class
    //   This provides the needed infrastructure for each
    //   command to work.
    class rgb_command
    {
    public:
        rgb_command(string const &aCmd,
                string const &rgb_base_name="RGB_COMMAND")
        : STRING_error_layer(rgb_base_name)
        , STRING_command_text(aCmd) { 
            aLogger = LoggerLevel::getInstance();
            input_file_pairs.clear();
        }

        virtual ~rgb_command() {
            aLogger->releaseInstance();
        }

        static bool match(string aParam)
        {
            return false;
        }

        void nothing_required(vector<string> &aCmdParam);
        void one_required(vector<string> &aCmdParam, string &p1);
        void one_required_one_optional(vector<string> &aCmdParam, string &p1, string &p2);
        void two_required(vector<string> &aCmdParam, string &p1, string &p2);

        void first_path_must_exist(string const &p1, vector<path> &path_listing);
        void first_path_must_exist_second_may_not_exist(string const &p1, string const &p2="");
        void both_paths_must_exist(string const &p1, string const &p2);

        virtual void init(vector<string> &aCmdParam) =0;
        virtual void process() =0;

        string STRING_command_text;
        string STRING_param_one;
        string STRING_param_two;

        // Holds either a pair of paths or a path and a filename.
        vector<rgb_param_pair> input_file_pairs;

        string STRING_error_layer;
        LoggerLevel *aLogger;
    };

    class rgb_command_help : public rgb_command
    {
    public:
        rgb_command_help()
        : rgb_command("-help", "RGB_CMD_HELP") {}

        virtual ~rgb_command_help() {}

        static bool match(string aParam)
        {
            if (aParam == "-help") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
             rgb_command::nothing_required(aCmdParam);
        }

        virtual void process();
    };

    class rgb_command_verbose : public rgb_command
    {
    public:
        rgb_command_verbose()
        : rgb_command("-verbose", "RGB_CMD_HELP") {}

        virtual ~rgb_command_verbose() {}

        static bool match(string aParam)
        {
            if (aParam == "-verbose") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
             rgb_command::nothing_required(aCmdParam);
             aLogger->setLevel(ENUM_VERBOSE);
        }

        virtual void process() { /* Do nothing */ };
    };

    class rgb_command_normal : public rgb_command
    {
    public:
        rgb_command_normal()
        : rgb_command("-normal", "RGB_CMD_HELP") {}

        virtual ~rgb_command_normal() {}

        static bool match(string aParam)
        {
            if (aParam == "-normal") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
             rgb_command::nothing_required(aCmdParam);
             aLogger->setLevel(ENUM_NORMAL);
        }

        virtual void process() { /* Do nothing */ };
    };

    class rgb_command_brief : public rgb_command
    {
    public:
        rgb_command_brief()
        : rgb_command("-brief", "RGB_CMD_HELP") {}
        
        virtual ~rgb_command_brief() {}

        static bool match(string aParam)
        {
            if (aParam == "-brief") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
             rgb_command::nothing_required(aCmdParam);
             aLogger->setLevel(ENUM_BRIEF);
        }

        virtual void process() { /* Do nothing */ };
    };

    class rgb_command_extract : public rgb_command
    {
    public:
        rgb_command_extract() 
        : rgb_command("-extract", "RGB_CMD_EXTRACT") {}

        virtual ~rgb_command_extract() {}

        static bool match(string aParam)
        {
            if (aParam == "-extract") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            one_required_one_optional(aCmdParam,STRING_param_one,STRING_param_two);
            first_path_must_exist_second_may_not_exist(STRING_param_one,STRING_param_two);
        }

        virtual void process();
    };

    class rgb_command_verify : public rgb_command
    {
    public:
        rgb_command_verify()
        : rgb_command("-verify", "RGB_CMD_VERIFY") {}

        virtual ~rgb_command_verify() {}

        static bool match(string aParam)
        {
            if (aParam == "-verify") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            two_required(aCmdParam,STRING_param_one,STRING_param_two);
            both_paths_must_exist(STRING_param_one,STRING_param_two);
        }

        virtual void process();
    };

    class rgb_command_replace : public rgb_command
    {
    public:
        rgb_command_replace()
        : rgb_command("-replace", "RGB_CMD_REPLACE") {}

        virtual ~rgb_command_replace() {}

        static bool match(string aParam)
        {
            if (aParam == "-replace") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            two_required(aCmdParam,STRING_param_one,STRING_param_two);
            both_paths_must_exist(STRING_param_one,STRING_param_two);
        }

        virtual void process();
    };

    class rgb_command_rollback : public rgb_command
    {
    public:
        rgb_command_rollback()
        : rgb_command("-rollback", "RGB_CMD_ROLLBACK") {}

        virtual ~rgb_command_rollback() {}

        static bool match(string aParam)
        {
            if (aParam == "-rollback") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            vector<path> file_paths;
            one_required(aCmdParam,STRING_param_one);
            first_path_must_exist(STRING_param_one, file_paths);

            // Push all the elements in path_listing into the file pair vector
            //  for use in the process() method.
            rgb_param_pair temp;
            for(vector<path>::const_iterator ii = file_paths.begin();
                ii != file_paths.end(); ii++)
            {
                // Place p1 in the file pair vector.
                temp.set(*ii);
                input_file_pairs.push_back(temp);
            }
        }

        virtual void process();
    };

    // This class holds the path object and a file name
    class rgb_param_pair
    {
    public:
        rgb_param_pair() { clear(); }
        rgb_param_pair(path const &A, string const &B) : path1(A) , path2(B) {}
        virtual ~rgb_param_pair() {}

        void clear() {
            path1.clear();
            path2.clear();
        }

        void set(path const &A) {
            path1 = A;
            path2.clear();
        }

        void set(path const &A, string const &B) {
            path1 = A;
            path2 = B;
        }

        path path1;
        string path2;
    };

private:

    string STRING_error_layer;
    LoggerLevel *aLogger;
};

#endif


