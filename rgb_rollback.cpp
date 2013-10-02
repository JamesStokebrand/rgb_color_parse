/*
    This file contains methods that will allow the user to 
      go back to the previous RGB node settings by using the
      last config listing found in the comments near the 
      beginning of the file.

      Something in the form of:

            #VRML v2.0 utf8 

            # Comment Section
            # Comment Section

            DEF blah blah 
            defuseColor R G B
            defuseColor R G B
            defuseColor R G B
            defuseColor R G B

*/

#ifndef __rgb_rollback_h__
#include "include/rgb_rollback.h"
#endif

#ifndef __rgb_replace_h__
#include "include/rgb_replace.h"
#endif


void rgb_rollback::rollback(const string &source)
{
    if (!srcFileIO){
        srcFileIO = new rgb_fileio;

        if (!srcFileIO)
        {
            // Unable to alocate RGB fileIO object
            throw_exception("Unable to allocate rgb_fileio.", 
                __PRETTY_FUNCTION__, __FILE__, __LINE__);
        }
    }

    clear();

    // Open file
    // State machine sequence:
    //  Verify the file is a VRML file.
    //  Parse out the historic config listing(s)
    //  Search for DEF
    //  Replace the RGB nodes with the last config listing
    // close open files
    // overwrite existing file with temp file


    srcFileIO->open(source,true);
    source_file = source;
    char aChar;
    while (srcFileIO->read_char(aChar)) {
        process(aChar);
    }

    srcFileIO->close();

    srcFileIO->overwrite(); // TODO do this as the final step.
}

void rgb_rollback::STATE_verify_VRML(const char &aChar)
{
    STATE_verify_required_word(STRING_error_layer,
            aChar,
            CONST_STRING_VRML_KEYWORD,
            (STATE)&rgb_rollback::STATE_verify_VRML_VER);
}

void rgb_rollback::STATE_verify_VRML_VER(const char &aChar)
{
    STATE_verify_required_word(STRING_error_layer,
        aChar,
        CONST_STRING_VRML_VER_KEYWORD,
        (STATE)&rgb_rollback::STATE_verify_VRML_CHARSET);
}

void rgb_rollback::STATE_verify_VRML_CHARSET(const char &aChar)
{
    STATE_verify_required_word(STRING_error_layer,
        aChar,
        CONST_STRING_VRML_CHARSET_KEYWORD,
        (STATE)&rgb_rollback::STATE_seek_START);
}

void rgb_rollback::STATE_seek_START(const char &aChar)
{
    // Useless whitespace? 
    if (isspace(aChar) && word_accumulate.empty() && !aConfigListing.empty())
    {
        // Throw it away
        return;
    }

    if (isspace(aChar))
    {
        if (word_accumulate == CONST_STRING_CONFIG_START_KEYWORD)
        {
            // Found the #START keyword

            // Store these chars in the temp config listing string
            aConfigListing.clear();
            aConfigListing += word_accumulate + aChar;
            word_accumulate.clear();

            // Transition to seek the #END keyword
            TRAN((STATE)&rgb_rollback::STATE_seek_END);
        }
        else if (word_accumulate == CONST_STRING_DEF_KEYWORD)
        {
            // Found a DEF keyword. 

            // Incomplete config listing?  Clear what has been accumulated.
            aConfigListing.clear();

            // Process the config listings found.
            Process_Config_Listings();

            // Write the DEF keyword 
            temp_string << word_accumulate << aChar;
            srcFileIO->write(temp_string.str());
            word_accumulate.clear();
            temp_string.str(string());

            // Transition to seeking the diffuseColor keyword.
            TRAN((STATE)&rgb_rollback::STATE_seek_DIFFUSECOLOR);
        }
        else
        {
            // Write everything to the the temp file.
            temp_string << word_accumulate << aChar;
            srcFileIO->write(temp_string.str());
            word_accumulate.clear();
            temp_string.str(string());
        }
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_rollback::STATE_seek_END(const char &aChar)
{
    if (isspace(aChar))
    {
        if (word_accumulate == CONST_STRING_CONFIG_END_KEYWORD)
        {
            // Found the #END keyword

            // Store these chars in the temp config listing string
            aConfigListing += word_accumulate;
            word_accumulate.clear();

            // This is a complete config with a start/end keywords.
            //  Store it in the vector
            config_listings.push_back(aConfigListing);

            // Transition to seek the #START keyword
            TRAN((STATE)&rgb_rollback::STATE_seek_START);
        }
        else if (word_accumulate == CONST_STRING_DEF_KEYWORD)
        {
            // Found a DEF keyword. 

            // Incomplete config listing?  Clear what has been accumulated.
            aConfigListing.clear();

            // Process the config listings found.
            Process_Config_Listings();

            // Write the DEF keyword 
            temp_string << word_accumulate << aChar;
            srcFileIO->write(temp_string.str());
            word_accumulate.clear();
            temp_string.str(string());

            // Transition to seeking the diffuseColor keyword.
            TRAN((STATE)&rgb_rollback::STATE_seek_DIFFUSECOLOR);
        }
        else
        {
            // Save everything to the the temp config listing string.
            aConfigListing += word_accumulate + aChar;
            word_accumulate.clear();
        }
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_rollback::STATE_seek_DIFFUSECOLOR(const char &aChar)
{
    if (isspace(aChar))
    {
        if (word_accumulate == CONST_STRING_DIFFUSECOLOR_KEYWORD)
        {
            // Transition to replace the RED RGB value
            TRAN((STATE)&rgb_rollback::STATE_get_RED);
        }

        // Write everything to the the temp file.
        temp_string << word_accumulate << aChar;
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_rollback::STATE_get_RED(const char &aChar)
{
    if (isspace(aChar))
    {
        // Replace this word with the RED value
        temp_string << config_block_nodes[0].get_red() << aChar;
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());

        // Transition to replace the GREEN RGB value
        TRAN((STATE)&rgb_rollback::STATE_get_GREEN);
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_rollback::STATE_get_GREEN(const char &aChar)
{
    if (isspace(aChar))
    {
        // Replace this word with the GREEN value
        temp_string << config_block_nodes[0].get_green() << aChar;
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());

        // Transition to replace the BLUE RGB value
        TRAN((STATE)&rgb_rollback::STATE_get_BLUE);
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_rollback::STATE_get_BLUE(const char &aChar)
{
    if (isspace(aChar))
    {
        // Replace this word with the BLUE value
        temp_string << config_block_nodes[0].get_blue() << aChar;
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());

        // Erase the first node.  It is now written to the
        //  file.
        config_block_nodes.erase(config_block_nodes.begin());

        if (config_block_nodes.empty())
        {
            // No more RGB nodes to replace .. go to NOOP
            //  and write out the remainder of the file.
            TRAN((STATE)&rgb_rollback::STATE_NOOP);
        }
        else {
            // Transition to seeking diffuseColor keyword
            TRAN((STATE)&rgb_rollback::STATE_seek_DIFFUSECOLOR);
        }
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_rollback::STATE_NOOP(const char &aChar)
{
    srcFileIO->write(aChar);
}

void rgb_rollback::Process_Config_Listings()
{
//cout << __PRETTY_FUNCTION__ << endl;
//cout << "Config Listing Size : " << config_listings.size() << endl;

    // If there are no configs in the vector ... this is an 
    //  an error.  Throw an exception.
    if (config_listings.size() == 0)
    {

        // Clean up the temp file before throwing
        //  the exception
        srcFileIO->erase();
        throw_exception("No previous config listings found in \"" + source_file + 
                "\".  Nothing to do.", __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    // If there is more than one config in the vector
    //  write all but the last one into the source file.
    if (config_listings.size() > 1)
    {
        stringstream temp;
        while (config_listings.size() > 1)
        {
            temp << config_listings[0] << endl;
            srcFileIO->write(temp.str());
            temp.str(string());
            config_listings.erase(config_listings.begin());
        }
    }

    // At this point we have one item in the config_listing vector.
    //  Process it into a config vector for replacement of the existing 
    //  RGB nodes.
    rgb_configio aConfigIO;

    aConfigIO.parse_node_config(config_listings[0],config_block_nodes);
}


#if 1
int main(int argc, char *argv[])
{
    rgb_replace rgbReplace;
    rgb_rollback aRollback;
    try {

#if 1
        // Replace multiple times ...
        int ii = 0;
        while(ii<=9)
        {
            cout << "Replace loop:" << ii++ << endl;
            rgbReplace.replace("ReadMore_11.5.wrl_temp_blah.txt", "temp_config1.txt");
            cout << "Replace loop:" << ii++ << endl;
            rgbReplace.replace("ReadMore_11.5.wrl_temp_blah.txt", "temp_config2.txt");
            cout << "Replace loop:" << ii++ << endl;
            rgbReplace.replace("ReadMore_11.5.wrl_temp_blah.txt", "temp_config3.txt");
        }

#if 1
        // Rollback until back to the original file.
        int jj = 0;
        while (1)
        {
            cout << "Rollback loop:" << jj++ << endl;
            aRollback.rollback("ReadMore_11.5.wrl_temp_blah.txt");
        }
        cout << "Done!" << endl;
#endif
#endif
    }
    catch (ErrException& caught) {
        cerr << caught.what() << endl;
    }

    return 0; // Success
}
#endif
