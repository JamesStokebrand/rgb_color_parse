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
## Filename: rgb_cmdline.cpp
##  This file contains main() and the command line methods that allow it
##   to recognize command switches and arguments.  
##
*/
#ifndef __rgb_cmdline_h__
#include "include/rgb_cmdline.h"
#endif

#if 0
#define DEBUG_METHOD_COUT cout << __PRETTY_FUNCTION__ << endl;
#else
#define DEBUG_METHOD_COUT //
#endif

void rgb_cmdline::parse_execute(const int &argc, char *argv[])
{
    // Do we have arguments to execute?
    if (argc <= 1)
    {
        print_usage();

        // No command line arguments found.  This is an error.
        aLogger->throw_exception(ENUM_NO_CMD_ARGUMENTS_FOUND,
                "No command line arguments found.  Nothing to do.",
                __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

    vector<string> list_of_commands;
    // ### NOTE must start this at 1.  argv[0] is the program name/path
    for (int i=1; i<argc; i++)
    {
        //temp = argv[i];
        list_of_commands.push_back(argv[i]);
    }

    // Setup a list of commands
    vector<rgb_command *> aListOfCmdObjects;
    setup(list_of_commands, aListOfCmdObjects);

    try
    {
        for (vector<rgb_command *>::iterator it = aListOfCmdObjects.begin(); it != aListOfCmdObjects.end(); it++)
        {
            //cout << *(*it) << endl;
            cout << endl
                 << "Executing : "
                 << (*it)->STRING_command_text << " "
                 << (*it)->STRING_param_one;
            if (!( (*it)->STRING_param_two.empty() ))
            {
                cout  << " " << (*it)->STRING_param_two;
            }
            cout << endl;
            (*it)->process();
        }
    }
    catch (ErrException& caught)
    {
        // Free allocated commands
        cleanup(aListOfCmdObjects);

        // Throw it again ...
        aLogger->throw_exception(caught.what());
    }

    // Free allocated commands
    cleanup(aListOfCmdObjects);
}

void rgb_cmdline::setup(vector<string> &aCmdParams, vector<rgb_command *> &aListOfCmds)
{
DEBUG_METHOD_COUT

    int pre_count = aCmdParams.size();

    // Loop through the list of command line arguments until one is unrecognized 
    //  or the list goes to zero.
    try
    {
        while (!aCmdParams.empty())
        {
//cout << "aCmdParam vector: " << aCmdParams << endl;
//cout << "aListOfCmds vector: " << aListOfCmds << endl;
            
            if (rgb_command_help::match(aCmdParams[0])) {
//cout << "Found a -help command" << endl;
                rgb_command_help *aCmd = new rgb_command_help;
                aListOfCmds.insert(aListOfCmds.begin(),aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_brief::match(aCmdParams[0])) {
//cout << "Found an -brief command" << endl;
                rgb_command_brief *aCmd = new rgb_command_brief;
                aCmd->init(aCmdParams);
                delete aCmd; // No further processing needed ...
            } else if (rgb_command_normal::match(aCmdParams[0])) {
//cout << "Found an -normal command" << endl;
                rgb_command_normal *aCmd = new rgb_command_normal;
                aCmd->init(aCmdParams);
                delete aCmd; // No further processing needed ...
            } else if (rgb_command_verbose::match(aCmdParams[0])) {
//cout << "Found an -verbose command" << endl;
                rgb_command_verbose *aCmd = new rgb_command_verbose;
                aCmd->init(aCmdParams);
                delete aCmd; // No further processing needed ...
            } else if (rgb_command_extract::match(aCmdParams[0])) {
//cout << "Found an -extract command" << endl;
                rgb_command_extract *aCmd = new rgb_command_extract;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_verify::match(aCmdParams[0])) {
//cout << "Found a -verify command" << endl;
                rgb_command_verify *aCmd = new rgb_command_verify;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_replace::match(aCmdParams[0])) {
//cout << "Found a -replace command" << endl;
                rgb_command_replace *aCmd = new rgb_command_replace;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_rollback::match(aCmdParams[0])) {
//cout << "Found a -rollback command" << endl;
                rgb_command_rollback *aCmd = new rgb_command_rollback;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            }

            // Did the command list size change?
            if (aCmdParams.size() == pre_count)
            {
                // Size didn't change.  Means that the command at the top is unrecognized.
                break; // This is an error.
            }

            pre_count = aCmdParams.size();
        }
    }
    catch (ErrException& caught) {
        cleanup(aListOfCmds);
        aLogger->throw_exception(caught.what());
    }

    // Were all the parameters consumed?
    if (!aCmdParams.empty())
    {
        // Nope... still have commands on the command line.  

        // Free allocated cmds.
        cleanup(aListOfCmds);

        // Unknown parameter found ... this is an error.
        aLogger->throw_exception(ENUM_NO_CMD_ARGUMENTS_FOUND
            ,"Found unknown command parameter \"" + aCmdParams[0] + "\"",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }
}

void rgb_cmdline::rgb_command::nothing_required(vector<string> &aCmdParam)
{
    // No parameters required ... verify the command line switch matches
    //  and remove it from the parameter list.

    // If the aCmdParam list isn't empty AND the first parameter starts with a dash.
    if ((!aCmdParam.empty()) &&
        (aCmdParam[0][0] == '-') &&
        (aCmdParam[0] == STRING_command_text))
    {
        // Found the command we were expecting.   Remove it from the list.
        aCmdParam.erase(aCmdParam.begin());
    }
    else
    {
        // Else its not the command switch we wanted ... this is an error.
        aLogger->throw_exception(ENUM_UNEXPECTED_COMMAND_SWITCH,
            "Unexpected command switch \"" + aCmdParam[0] + "\"",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }
}

void rgb_cmdline::rgb_command::one_required(vector<string> &aCmdParam, string &p1)
{
DEBUG_METHOD_COUT
    // Extract ONE required parameter

    // Verify the expected command switch.
    nothing_required(aCmdParam);

    // Extract required first parameter ...
    // IF the parameter list isnt empty 
    //   AND the first parameter doesnt start with a dash
    // THEN this is the first parameter
    if ((!aCmdParam.empty()) && (aCmdParam[0][0] != '-'))
    {
        // This first parameter is the input file name
        p1 = aCmdParam[0];
        aCmdParam.erase(aCmdParam.begin());
    }
    else
    {
        // ELSE this is an error 
        string anError(" \"" + STRING_command_text + "\"");
        if (aCmdParam.empty())
        {
            // List is empty ... no parameter found ...
            anError += " found but missing first parameter.";
        }
        else
        {
            // Else its a command switch ... 
            anError += " found but unexpected command switch \"" + aCmdParam[0] + "\" in place of first parameter.";
        }
        aLogger->throw_exception(ENUM_MISSING_FIRST_PARAMETER, 
            anError, __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }
}

void rgb_cmdline::rgb_command::one_required_one_optional(vector<string> &aCmdParam, string &p1, string &p2)
{
    // Extract ONE required and ONE optional parameters
    one_required(aCmdParam, p1);

    // Generate a second parameter... 
    //p2 = p1 + 
        //CONST_STRING_DEFAULT_RGB_NODE_FILE_EXTENTION;

    // Extract optional second parameter ...
    // If the parameter list ISN'T empty
    //   AND parameter doesn't start with a dash.
    // THEN this is the second (optional) parameter
    if ((!aCmdParam.empty()) && (aCmdParam[0][0] != '-'))
    {
            // This second parameter is the output config file name
            p2 = aCmdParam[0];
            aCmdParam.erase(aCmdParam.begin());
    }
}

void rgb_cmdline::rgb_command::two_required(vector<string> &aCmdParam, string &p1, string &p2)
{
    // Verify command switch is correct and extract first 
    //  required parameter
    one_required(aCmdParam, p1);

    // Extract required second parameter ...
    // IF the parameter list isnt empty 
    //   AND the parameter doesnt start with a dash
    // THEN this is the required second parameter
    if ((!aCmdParam.empty()) && (aCmdParam[0][0] != '-'))
    {
        // This second parameter is the RGB config file
        p2 = aCmdParam[0];
        aCmdParam.erase(aCmdParam.begin());
    }
    else
    {
        // ELSE this is an error 
        string anError(" \"" + STRING_command_text + "\"");
        if (aCmdParam.empty())
        {
            // Else its a command switch...
            anError += " found but missing second parameter.";
        }
        else
        {
            // Else its a command switch...
            anError += " found but unexpected command switch \"" + aCmdParam[0] + "\" in place of second parameter.";
        }
        aLogger->throw_exception(ENUM_MISSING_SECOND_PARAMETER,
            anError, __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }
}

void rgb_cmdline::rgb_command::first_path_must_exist(string const &p1, vector<path> &path_listing)
{
    // Clear the file pairs
    path_listing.clear();

    // Is p1 exist
    path path1(p1);
    if (!exists(path1))
    {
        // Input file does not exist.  This is an error.
        // File or path does not exist ... this is an error.
        aLogger->throw_exception(ENUM_FILE_OR_DIRECTORY_NOT_FOUND,
            "Parameter one: \"" + p1 + "\" does not exist.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

    // It exists ... what kind of element is it?
    if (is_regular_file(path1))
    {
        // Its a regular file. Push it into the file pair vector
        path_listing.push_back(path1);
    } 
    else if (is_directory(path1))
    {
        // Its a directory ... get a listing of everything
        vector<path> list_of_directory_elements;
        copy(directory_iterator(path1), directory_iterator(),
                back_inserter(list_of_directory_elements));

        // Now sort out the regular files from everything else.
        for(vector<path>::const_iterator ii = list_of_directory_elements.begin();
            ii != list_of_directory_elements.end(); ii++) {
            if (is_regular_file(*ii)) {
                // Push the listing of regular files into the 
                //  file pairs vector.
                path_listing.push_back(*ii);
            }
        }
    } else {
        // Don't know what this element is ... its not a file or a directory.
        // This is an error.
        aLogger->throw_exception(ENUM_UNKNOWN_FILE_TYPE 
            ,"Parameter one: \"" + p1 + 
            "\" is an unknown file type.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

    // Did we find any regular files in the P1 directory?
    if (path_listing.empty())
    {
        // No regular files found... nothing to do.
        aLogger->throw_exception(ENUM_NO_FILES_FOUND_IN_DIRECTORY,
            "No files found in \"" + p1 + "\".  Nothing to do.", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }


}

void rgb_cmdline::rgb_command::first_path_must_exist_second_may_not_exist(
    string const &p1, string const &p2)
{
    // Get the file listing from the first parameter
    vector<path> file_paths;
    first_path_must_exist(p1, file_paths);

    // NOTE we will only be using p2 if there was only a file input for p1
    //  This will be only in the case of the extract.

    path path2(p2);
    if (exists(path2) && is_directory(path2))
    {
        // P2 can't be a directory...  this is an error.
        aLogger->throw_exception(ENUM_PARAM_TWO_IS_DIRECTORY,
            "Parameter two: \"" + p2 + 
            "\" cannot be a directory.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

    if (file_paths.size() == 1)
    {
        // Only one file for p1 found ... go ahead and push 
        //  both parameters into the file pair vector
        rgb_param_pair temp(p1,p2);
        input_file_pairs.push_back(temp);
    }
    else if (file_paths.size() > 1)
    {
        // Else many files found in the directory
        rgb_param_pair temp;
        for(vector<path>::const_iterator ii = file_paths.begin();
            ii != file_paths.end(); ii++)
        {
            // Place p1 in the file pair vector.
            //  Don't bother putting p2 into the vector.
            temp.set(*ii);
            input_file_pairs.push_back(temp);
        }
    }
}

void rgb_cmdline::rgb_command::both_paths_must_exist(string const &p1, string const &p2)
{
    // Get the file listing from the first parameter
    vector<path> file_paths;
    first_path_must_exist(p1, file_paths);

    // Verify the p2 exists and is a file.
    path path2(p2);
    if (!exists(path2)) {
        // P2 doesn't exist!  This is an error.
        aLogger->throw_exception(ENUM_PARAM_TWO_NOT_FOUND,
            "Parameter two: \"" + p2 + 
            "\" does not exist.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    } else if (!is_regular_file(path2)) {
        aLogger->throw_exception(ENUM_PARAM_TWO_IS_DIRECTORY,
            "Parameter two: \"" + p2 + 
            "\" is a directory.  This is not a valid second parameter.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__, STRING_error_layer);
    }

    // P2 exists and is a regular file.  Push p1 an p2 into the file pair vector.
    rgb_param_pair temp;
    for(vector<path>::const_iterator ii = file_paths.begin();
        ii != file_paths.end(); ii++)
    {
        // Place p1 in the file pair vector.
        //  Don't bother putting p2 into the vector.
        temp.set(*ii, p2);
        input_file_pairs.push_back(temp);
    }
}

void rgb_cmdline::rgb_command_help::process()
{
    // Just print the usage ...
    print_usage();
}

void rgb_cmdline::rgb_command_extract::process()
{
    // Process each file
    rgb_extract anExtractObj;
    for(vector<rgb_param_pair>::const_iterator ii = input_file_pairs.begin();
        ii != input_file_pairs.end(); ii++) {

        cout << "Extract : " << ii->path1.filename().string() << " " << ii->path2;

        try
        {

#if 0
cout << endl << endl << "P1 : " << canonical(ii->path1).string() << endl;
cout << "P2 : " << ii->string1 << endl;
#endif
            anExtractObj.extract(canonical(ii->path1).string(), ii->path2);
            cout << " - SUCCESS" << endl;
        }
        catch (ErrException& e)
        {
            cout << " - " << e.what() << endl;
        }
        catch(const filesystem_error& e)
        {
            cout << " - " << e.what() << endl;
        }
    }
}

void rgb_cmdline::rgb_command_verify::process()
{
    // Verify
    rgb_extract anExtractObj;
    for(vector<rgb_param_pair>::const_iterator ii = input_file_pairs.begin();
        ii != input_file_pairs.end(); ii++) {

        cout << "Verify : " << ii->path1.filename().string() << " " << ii->path2;
        try
        {
#if 0
cout << endl << endl << "P1 : " << canonical(ii->path1).string() << endl;
cout << "P2 : " << ii->string1 << endl;
#endif
            if (anExtractObj.verify(canonical(ii->path1).string(), ii->path2)) {
                cout << " - MATCH" << endl;
            } else {
                cout << " - no match" << endl;
            }
        }
        catch (ErrException& e)
        {
            cout << " - " << e.what() << endl;
        }
        catch(const filesystem_error& e)
        {
            cout << " - " << e.what() << endl;
        }
    }
}

void rgb_cmdline::rgb_command_replace::process()
{
    // Replace
    rgb_replace aReplaceObj;
    for(vector<rgb_param_pair>::const_iterator ii = input_file_pairs.begin();
        ii != input_file_pairs.end(); ii++) {

        cout << "Replace : " << ii->path1.filename().string() << " " << ii->path2;
        try
        {
#if 0
cout << endl << endl << "P1 : " << canonical(ii->path1).string() << endl;
cout << "P2 : " << ii->path2 << endl;
#endif
            aReplaceObj.replace(canonical(ii->path1).string(), ii->path2);
            cout << " - SUCCESS" << endl;
        }
        catch (ErrException& e)
        {
            cout << " - " << e.what() << endl;
        }
        catch(const filesystem_error& e)
        {
            cout << " - " << e.what() << endl;
        }
    }
}

void rgb_cmdline::rgb_command_rollback::process()
{
    // Replace
    rgb_rollback aRollbackObj;
    for(vector<rgb_param_pair>::const_iterator ii = input_file_pairs.begin();
        ii != input_file_pairs.end(); ii++) {

        cout << "Rollback : " << ii->path1.filename().string();
        try
        {
#if 0
cout << endl << endl << "P1 : " << canonical(ii->path1).string() << endl;
#endif
            aRollbackObj.rollback(canonical(ii->path1).string());
            cout << " - SUCCESS" << endl;
        }
        catch (ErrException& e)
        {
            cout << " - " << e.what() << endl;
        }
        catch(const filesystem_error& e)
        {
            cout << " - " << e.what() << endl;
        }
    }
}

void rgb_cmdline::print_usage()
{
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -help" << endl;
cout << "  - Displays this usage information" << endl;
cout << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -extract <a_single_wrl_file> [optional_config_file]" << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -extract <a_directory_containing_wrl_files>" << endl;
cout << "  - Extracts RGB node information from a single VRML file or all the" << endl;
cout << "     VRML files in a directory." << endl;
cout << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -verify <a_single_wrl_file> <required_config_file>" << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -verify <a_directory_containing_wrl_files> <required_config_file>" << endl;
cout << "  - Verifies that the RGB nodes in a single VRML or all the files in a directory" << endl;
cout << "     match the ones found in a required RGB config file." << endl;
cout << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -replace <a_single_wrl_file> <required_config_file>" << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -replace <a_directory_containing_wrl_files> <required_config_file>" << endl;
cout << "  - Replaces the RGB nodes in a single VRML file or all the VRML files found in " << endl;
cout << "     a directory.  Requires a RGB config file." << endl;
cout << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -rollback <a_single_wrl_file>" << endl;
cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -rollback <a_directory_containing_wrl_fles>" << endl;
cout << "  - Rollsback the RGB nodes previously changed from the \"-replace\" command." << endl;
cout << "     Requires a single VRML file or all the VMRL files found in a directory." << endl; 
cout << endl;
}

int main(int argc, char* argv[])
{
    rgb_cmdline aCmdLine;
    try{
        aCmdLine.parse_execute(argc, argv);
    }
    catch (ErrException& caught) {
        cerr << caught.what() << endl;
    }
    catch(const filesystem_error& e)
    {
       // Just spew the message ... 
       cerr << e.code().message() << endl;
    }
    return 0;
}


