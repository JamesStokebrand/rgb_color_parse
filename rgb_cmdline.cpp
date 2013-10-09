/*

    This file defines the methods for the RGB command line tool
    
*/
#ifndef __rgb_cmdline_h__
#include "include/rgb_cmdline.h"
#endif

#if 1
#define DEBUG_METHOD_COUT cout << __PRETTY_FUNCTION__ << endl;
#else
#define //
#endif

void rgb_cmdline::parse_execute(const int &argc, char *argv[])
{
    // Do we have arguments to execute?
    if (argc <= 1)
    {
        print_usage();

        // No command line arguments found.  This is an error.
        throw_exception("No command line arguments found.  Nothing to do.",
                __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    vector<string> list_of_commands;
    // ### NOTE must start this at 1.  argv[0] is the program name/path
    for (int i=1; i<argc; i++)
    {
        //temp = argv[i];
        list_of_commands.push_back(argv[i]);
    }
//cout << "Cmds: " << list_of_commands << endl;

    vector<rgb_command *> aListOfCmdObjects;
    // Setup a list of commands
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
        throw_exception(caught.what(), __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
cout << "Found a -help command" << endl;
                rgb_command_help *aCmd = new rgb_command_help;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_extract::match(aCmdParams[0])) {
cout << "Found an -extract command" << endl;
                rgb_command_extract *aCmd = new rgb_command_extract;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_verify::match(aCmdParams[0])) {
cout << "Found a -verify command" << endl;
                rgb_command_verify *aCmd = new rgb_command_verify;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_replace::match(aCmdParams[0])) {
cout << "Found a -replace command" << endl;
                rgb_command_replace *aCmd = new rgb_command_replace;
                aListOfCmds.push_back(aCmd);
                aCmd->init(aCmdParams);
            } else if (rgb_command_rollback::match(aCmdParams[0])) {
cout << "Found a -rollback command" << endl;
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
        throw_exception(caught.what(), __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    // Were all the parameters consumed?
    if (!aCmdParams.empty())
    {
        // Nope... still have commands on the command line.  

        // Free allocated cmds.
        cleanup(aListOfCmds);

        // Unknown parameter found ... this is an error.
        throw_exception("Found unknown command parameter \"" + aCmdParams[0] + "\"",
                        __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
        throw_exception("Unexpected command switch \"" + aCmdParam[0] + "\"",__PRETTY_FUNCTION__, __FILE__, __LINE__);
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
        throw_exception(anError, __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }
}

void rgb_cmdline::rgb_command::one_required_one_optional(vector<string> &aCmdParam, string &p1, string &p2)
{
    // Extract ONE required and ONE optional parameters
    one_required(aCmdParam, p1);

    // Generate a second parameter... 
    p2 = p1 + 
        CONST_STRING_DEFAULT_RGB_NODE_FILE_EXTENTION;

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
        throw_exception(anError, __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
        throw_exception(p1 + " does not exist.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
        throw_exception("Parameter one: \"" + p1 + "\" is an unknown file type.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    // Did we find any regular files in the P1 directory?
    if (path_listing.empty())
    {
        // No regular files found... nothing to do.
        throw_exception("No files found in \"" + p1 + "\".  Nothing to do.", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
        throw_exception("Parameter two: \"" + p2 + "\" cannot be a directory.  Check the path or filename.",
         __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
        throw_exception("Parameter two: \"" + p2 + "\" does not exist.  Check the path or filename.",
         __PRETTY_FUNCTION__, __FILE__, __LINE__);
    } else if (!is_regular_file(path2)) {
        throw_exception("Parameter two: \"" + p2 + "\" is a directory.  This is not a valid second parameter.  Check the path or filename.",
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
//cerr << e.what() << endl;
            cout << " - failure" << endl;
        }
        catch(const filesystem_error& e)
        {
//cerr << e.what() << endl;
            cout << " - failure" << endl;
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
            //cerr << e.what() << endl;
            cout << " - error" << endl;
        }
        catch(const filesystem_error& e)
        {
            //cerr << e.what() << endl;
            cout << " - error" << endl;
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
cout << "P2 : " << ii->string1 << endl;
#endif
            aReplaceObj.replace(canonical(ii->path1).string(), ii->path2);
            cout << " - SUCCESS" << endl;
        }
        catch (ErrException& e)
        {
            //cerr << e.what() << endl;
            cout << " - failure" << endl;
        }
        catch(const filesystem_error& e)
        {
            //cerr << e.what() << endl;
            cout << " - failure" << endl;
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
            //cerr << e.what() << endl;
            cout << " - failure" << endl;
        }
        catch(const filesystem_error& e)
        {
            //cerr << e.what() << endl;
            cout << " - failure" << endl;
        }
    }
}

void rgb_cmdline::print_usage()
{
    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " provides support for the following commands:" << endl << endl;
    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -extract <a_single_wrl_file> [optional_config_file]" << endl;
    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -extract <a_directory_containing_wrl_files> [optional_config_file]" << endl << endl;

    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -verify <a_single_wrl_file> <required_config_file>" << endl << endl;

    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -replace <a_single_wrl_file> <required_config_file>" << endl;
    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -replace <a_directory_containing_wrl_files> <required_config_file>" << endl << endl;

    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -rollback <a_single_wrl_file>" << endl;
    cout << CONST_STRING_DEFAULT_ARGUMENT_ZERO << " -rollback <a_directory_containing_wrl_fles>" << endl << endl;
}

int main(int argc, char* argv[])
{
#if 1
    rgb_cmdline aCmdLine;

    try{
        //aCmdLine.print_usage(argv[0]);
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
#endif
#if 0
    //current working directory
    path full_path(argv[0]);

//    cout << full_path << endl;
//    cout << full_path.c_str() << endl;
//    cout << full_path.root_directory() << endl;

cout << "root_name(): " << full_path.root_name() << endl;
cout << "root_directory(): " << full_path.root_directory() << endl;
cout << "root_path(): " << full_path.root_path() << endl;
cout << "relative_path(): " << full_path.relative_path() << endl;
cout << "parent_path(): " << full_path.parent_path() << endl;
cout << "filename(): " << full_path.filename() << endl;
cout << "filename().string(): " << full_path.filename().string() << endl;
cout << "stem(): " << full_path.stem() << endl;
cout << "extention(): " << full_path.extension() << endl;

cout << "absolute(path): " << absolute(full_path).c_str() << endl;
cout << "absolute(path).string(): " << absolute(full_path).string() << endl;
cout << "canonical(path): " << canonical(full_path).c_str() << endl;
cout << "current_path(): " << current_path() << endl;
//        path& make_absolute(const path& base);
//        path& make_preferred();  // ISO/IEC 9945: no effect. Windows: convert slashes to backslashes
//        path& remove_filename();
//        path& replace_extension(const path& new_extension = path());
//        void  swap(path& rhs);
#endif

    return 0;
}
