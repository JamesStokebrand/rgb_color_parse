/* 
    This class will provide the command line front end to the 
    RGB node parser.  


    This object will support several commands:

    -extract <a_single_wrl_file> [config_file]
    -extract <a_directory_containing_wrl_files> [config_file]

    -verify <a_single_wrl_file> <config_file>

    -replace <a_single_wrl_file> <config_file>
    -replace <a_directory_containing_wrl_files> <config_file>

    -rollback <a_single_wrl_file> <config_file>
    -rollback <a_directory_containing_wrl_fles> <config_file>

*/

#ifndef __rgb_cmdline_h__
#define __rgb_cmdline_h__

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

class rgb_cmdline : public rgb_base
{
public:
    class rgb_command;
    class rgb_param_pair;

    rgb_cmdline() : rgb_base("RGB_CMDLINE") {
    }

    virtual ~rgb_cmdline() {}

    void static print_usage();

    void setup(vector<string> &aCmdParams, vector<rgb_command *> &aListOfCmds);
    void parse_execute(const int &argc, char *argv[]);

    void cleanup(vector<rgb_command *> &aListOfCmds)
    {
        for (vector<rgb_command *>::iterator it = aListOfCmds.begin(); it != aListOfCmds.end(); it++)
        {
            // Delete the list of commands.
            delete *it;
        }
        aListOfCmds.clear();
    }
    
    // This is the generic command class
    //   This provides the needed infrastructure for each
    //   command to work.
    class rgb_command : public rgb_base
    {
    public:
        rgb_command(string const &aCmd,
                string const &rgb_base_name="RGB_COMMAND")
        : rgb_base(rgb_base_name)
        , STRING_command_text(aCmd) { 
            input_file_pairs.clear();
        }

        virtual ~rgb_command() {}

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

    // This class holds the path object and an optional config file name
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

};

#endif


