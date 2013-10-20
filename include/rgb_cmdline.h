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
## Filename: rgb_cmdline.h
##  This file defines the command line object that allows this program
##   to parse and execute commands from the command line.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
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

#if 0
#define DEBUG_METHOD_COUT cout << __PRETTY_FUNCTION__ << endl;
#else
#define DEBUG_METHOD_COUT //
#endif

class rgb_cmdline
{
public:
    class rgb_command;
    class rgb_param_pair;

    // Used to create a vector of rgb_command match and factory methods.
    typedef bool (*command_match_method)(string);
    typedef rgb_command *(*command_factory_method)();

    rgb_cmdline() : STRING_error_layer("RGB_CMDLINE") {
        aLogger = LoggerLevel::getInstance();

        // Make a list of the commands and the object to create when the command 
        //  is found.
        command_match temp;

        // -help command
        temp._match = rgb_command_help::match1;
        temp._factory = rgb_command_help::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -info command
        temp._match = rgb_command_help::match2;
        temp._factory = rgb_command_help::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -verbose command
        temp._match = rgb_command_logger_level::match1; 
        temp._factory = rgb_command_logger_level::factory;
        temp.immediate_delete = true;
        available_commands.push_back(temp);

        // -normal command
        temp._match = rgb_command_logger_level::match2;
        temp._factory = rgb_command_logger_level::factory;
        temp.immediate_delete = true;
        available_commands.push_back(temp);

        // -brief command
        temp._match = rgb_command_logger_level::match3;
        temp._factory = rgb_command_logger_level::factory;
        temp.immediate_delete = true;
        available_commands.push_back(temp);

        // -extract command
        temp._match = rgb_command_extract::match1;
        temp._factory = rgb_command_extract::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -e (extract) command
        temp._match = rgb_command_extract::match2;
        temp._factory = rgb_command_extract::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -verify command
        temp._match = rgb_command_verify::match1;
        temp._factory = rgb_command_verify::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -v (verify) command
        temp._match = rgb_command_verify::match2;
        temp._factory = rgb_command_verify::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -replace command
        temp._match = rgb_command_replace::match1;
        temp._factory = rgb_command_replace::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -r (replace) command
        temp._match = rgb_command_replace::match2;
        temp._factory = rgb_command_replace::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -rollback command
        temp._match = rgb_command_rollback::match1;
        temp._factory = rgb_command_rollback::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);

        // -roll (rollback) command
        temp._match = rgb_command_rollback::match2;
        temp._factory = rgb_command_rollback::factory;
        temp.immediate_delete = false;
        available_commands.push_back(temp);
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
        rgb_command(string const &rgb_base_name)
        : STRING_error_layer(rgb_base_name) {
            aLogger = LoggerLevel::getInstance();
            input_file_pairs.clear();
        }

        virtual ~rgb_command() {
            aLogger->releaseInstance();
        }

        static bool match(string) {
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
        static rgb_command *factory() { return NULL; }

        string STRING_command_text;
        string STRING_param_one;
        string STRING_param_two;

        // Holds either a pair of paths or a path and a filename.
        vector<rgb_param_pair> input_file_pairs;
        vector<string> commands_handled;

        string STRING_error_layer;
        LoggerLevel *aLogger;
    };

    class rgb_command_help : public rgb_command
    {
    public:
        rgb_command_help()
        : rgb_command("RGB_CMD_HELP") {
            STRING_command_text.clear();
            commands_handled.clear();
            commands_handled.push_back("-help");
            commands_handled.push_back("-info");
        }

        virtual ~rgb_command_help() {}

        static bool match1(string aParam) {
            if (aParam == "-help") {
                return true;
            }
            return false;
        }

        static bool match2(string aParam) {
            if (aParam == "-info") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
             rgb_command::nothing_required(aCmdParam);
        }

        virtual void process() { print_usage(); }
        static rgb_command *factory() { return new rgb_command_help; }
    };

    class rgb_command_logger_level : public rgb_command
    {
    public:
        rgb_command_logger_level()
        : rgb_command("RGB_CMD_HELP") {
            STRING_command_text.clear();
            commands_handled.clear();
            commands_handled.push_back("-verbose");
            commands_handled.push_back("-normal");
            commands_handled.push_back("-brief");
        }

        virtual ~rgb_command_logger_level() {}

        static bool match1(string aParam) {
            if (aParam == "-verbose") {
                return true;
            }
            return false;
        }

        static bool match2(string aParam) {
            if (aParam == "-normal") {
                return true;
            }
            return false;
        }

        static bool match3(string aParam) {
            if (aParam == "-brief") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            rgb_command::nothing_required(aCmdParam);
            if (STRING_command_text == "-verbose") {
                aLogger->setLevel(ENUM_VERBOSE);
//cout << "LOGGER Setting VERBOSE" << endl;
            } else if (STRING_command_text == "-normal") {
                aLogger->setLevel(ENUM_NORMAL);
//cout << "LOGGER Setting NORMAL" << endl;
            } else if (STRING_command_text == "-brief") {
                aLogger->setLevel(ENUM_BRIEF);
//cout << "LOGGER Setting BRIEF" << endl;
            } else {
                // Dont recognize the command??
                //  Shouldn't get here ... 
                //  Set ENUM_VERBOSE
                aLogger->setLevel(ENUM_VERBOSE);
            }
        }

        virtual void process() { /* Do nothing */ };
        static rgb_command *factory() { return new rgb_command_logger_level; }
    };

    class rgb_command_extract : public rgb_command
    {
    public:
        rgb_command_extract() 
        : rgb_command("RGB_CMD_EXTRACT") {
            STRING_command_text.clear();
            commands_handled.clear();
            commands_handled.push_back("-extract");
            commands_handled.push_back("-e");
        }

        virtual ~rgb_command_extract() {}

        static bool match1(string aParam) {
            if (aParam == "-extract") {
                return true;
            }
            return false;
        }

        static bool match2(string aParam) {
            if (aParam == "-e") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            one_required_one_optional(aCmdParam,STRING_param_one,STRING_param_two);
            first_path_must_exist_second_may_not_exist(STRING_param_one,STRING_param_two);
        }

        virtual void process();
        static rgb_command *factory() { return new rgb_command_extract; }
    };

    class rgb_command_verify : public rgb_command
    {
    public:
        rgb_command_verify()
        : rgb_command("RGB_CMD_VERIFY") {
            STRING_command_text.clear();
            commands_handled.clear();
            commands_handled.push_back("-verify");
            commands_handled.push_back("-v");
        }

        virtual ~rgb_command_verify() {}

        static bool match1(string aParam) {
            if (aParam == "-verify") {
                return true;
            }
            return false;
        }

        static bool match2(string aParam) {
            if (aParam == "-v") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            two_required(aCmdParam,STRING_param_one,STRING_param_two);
            both_paths_must_exist(STRING_param_one,STRING_param_two);
        }

        virtual void process();
        static rgb_command *factory() { return new rgb_command_verify; }
    };

    class rgb_command_replace : public rgb_command
    {
    public:
        rgb_command_replace()
        : rgb_command("RGB_CMD_REPLACE") {
            STRING_command_text.clear();
            commands_handled.clear();
            commands_handled.push_back("-replace");
            commands_handled.push_back("-r");
        }

        virtual ~rgb_command_replace() {}

        static bool match1(string aParam) {
            if (aParam == "-replace") {
                return true;
            }
            return false;
        }

        static bool match2(string aParam) {
            if (aParam == "-r") {
                return true;
            }
            return false;
        }

        virtual void init(vector<string> &aCmdParam) {
            two_required(aCmdParam,STRING_param_one,STRING_param_two);
            both_paths_must_exist(STRING_param_one,STRING_param_two);
        }

        virtual void process();
        static rgb_command *factory() { return new rgb_command_replace; }
    };

    class rgb_command_rollback : public rgb_command
    {
    public:
        rgb_command_rollback()
        : rgb_command("RGB_CMD_ROLLBACK") {
            STRING_command_text.clear();
            commands_handled.clear();
            commands_handled.push_back("-rollback");
            commands_handled.push_back("-roll");
        }

        virtual ~rgb_command_rollback() {}

        static bool match1(string aParam) {
            if (aParam == "-rollback") {
                return true;
            }
            return false;
        }

        static bool match2(string aParam) {
            if (aParam == "-roll") {
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
        static rgb_command *factory() { return new rgb_command_rollback; }
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

    struct command_match {
        command_match_method _match;
        command_factory_method _factory;
        bool immediate_delete;
    };

    vector<command_match> available_commands;


    string STRING_error_layer;
    LoggerLevel *aLogger;
};

#endif


