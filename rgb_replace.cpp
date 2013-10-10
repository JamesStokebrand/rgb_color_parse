/* this file defines the RGB Replace methods where the current
    RGB node values are replaced by ones from a config file.
   The existing RGB nodes are then placed in the comment section of
    the file.
*/

#ifndef __rgb_replace_h__
#include "include/rgb_replace.h"
#endif

#ifndef __rgb_extract_h__
#include "include/rgb_extract.h"
#endif

#ifndef __rgb_configio_h__
#include "include/rgb_configio.h"
#endif


void rgb_replace::replace(string const &rgb_file, string const &rgb_config_file)
{
    // Replace works in this sequence
    // 1) Extract existing nodes from current file.
    // 2) Extract nodes from config file.
    // 3) Compare them ... if they are different then...
    // 4) Open a temp file.
    // 5) Parse the source file until we get to the DEF keyword.
    // 6) Config format the existing nodes and write this before the DEF keyword.
    // 7) There is no rule 7.
    // 8) Parse the remaining file and replace all the RGB values with the 
    //     new values from the config file.
    // 9) Close the input and temp file.
    // 10) Overwrite the source file with the temp file.

    // Clear the existing variables.
    clear();

    // Need to allocate a fileIO object?
    if (!srcFileIO)
    {
        // This opens the source file.
        srcFileIO = new rgb_fileio();
    }

    // Object created?  
    if (!srcFileIO)
    {
        // Unable to allocate the rgb_fileio object.  This is an error.
        throw_exception(ENUM_UNABLE_TO_ALLOCATE_FILEIO,
            "Unable to allocate rgb_fileio object.", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    // Extract existing nodes from source file.
    rgb_extract rgbExtract;
    vector<rgb_node> source_node_vector;
    rgbExtract.extract_nodes(rgb_file, source_node_vector);

    // Create and parse a config string based on the RGB nodes from the source file.
    rgb_configio cnfgFileIO;
    cnfgFileIO.create_node_config(source_node_vector, rgb_file, existing_node_config);
    cnfgFileIO.parse_node_config(rgb_config_file, config_node_vector);

    // Compare the two node vectors .. are they the same?
    if (source_node_vector == config_node_vector)
    {
        // Nothing to do.  Source RGB nodes match the ones in the config file.
        //  This is an error.
        throw_exception(ENUM_RGB_NODES_MATCH, 
            "RGB nodes in \"" + rgb_file + "\" match the RGB nodes "
            + "in \"" + rgb_config_file + "\".  Nothing to do.", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

// Debug
//cout << "vector: source_node_vector" << endl << source_node_vector << endl;
//cout << "vector: config_node_vector" << endl << config_node_vector << endl;

    // Ok, so the RGB nodes are different .. then replace them.
    // Parse the source file and replace the exising nodes with the 
    //  nodes found in the config file.

    // Need to allocate a fileIO object?
    if (!srcFileIO)
    {
        // This opens the source file.
        srcFileIO = new rgb_fileio();
    }

    // Object created?  
    if (!srcFileIO)
    {
        // Unable to allocate the rgb_fileio object.  This is an error.
        throw_exception(ENUM_UNABLE_TO_ALLOCATE_FILEIO,
            "Unable to allocate rgb_fileio object.", 
            __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    srcFileIO->clear();
    srcFileIO->open(rgb_file, true);

    // Read the source file char by char
    char aChar;
    word_accumulate.clear();
    while (srcFileIO->read_char(aChar))
    {
//cout << "Char:" << aChar << endl; // DEBUG
        // Process the char's through the state machine
        process(aChar);
    }

    // Close open files
    srcFileIO->close();

    // Copy the temp file we created over the original file.
    srcFileIO->overwrite();
}

void rgb_replace::STATE_verify_VRML(const char &aChar)
{
    STATE_verify_required_word(STRING_error_layer,
            aChar,
            CONST_STRING_VRML_KEYWORD,
            (STATE)&rgb_replace::STATE_verify_VRML_VER);
}

void rgb_replace::STATE_verify_VRML_VER(const char &aChar)
{
    STATE_verify_required_word(STRING_error_layer,
        aChar,
        CONST_STRING_VRML_VER_KEYWORD,
        (STATE)&rgb_replace::STATE_verify_VRML_CHARSET);
}

void rgb_replace::STATE_verify_VRML_CHARSET(const char &aChar)
{
    STATE_verify_required_word(STRING_error_layer,
        aChar,
        CONST_STRING_VRML_CHARSET_KEYWORD,
        (STATE)&rgb_replace::STATE_seek_DEF);
}

void rgb_replace::STATE_seek_DEF(const char &aChar)
{
    if (isspace(aChar))
    {

        if (word_accumulate == CONST_STRING_DEF_KEYWORD)
        {
            // Add the existing RGB config nodes to the 
            //  temp file.
            temp_string << existing_node_config;

            // Transition to seeking the diffuseColor keyword.
            TRAN((STATE)&rgb_replace::STATE_seek_DIFFUSECOLOR);
        }

        // Store the accumulate and current char into the temp_string
        //   and write it to the temp file.
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

void rgb_replace::STATE_seek_DIFFUSECOLOR(const char &aChar)
{
    if (isspace(aChar))
    {
        if (word_accumulate == CONST_STRING_DIFFUSECOLOR_KEYWORD)
        {
            // Transition to replace the RED RGB value
            TRAN((STATE)&rgb_replace::STATE_get_RED);
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

void rgb_replace::STATE_get_RED(const char &aChar)
{
    if (isspace(aChar))
    {
        // Expect float value here
        // Substitute the new RED float value in place of the 
        //  existing value.
        temp_string << config_node_vector[0].get_red() << aChar;

        // Transition to replace the GREEN RGB value
        TRAN((STATE)&rgb_replace::STATE_get_GREEN);

        // Write the temp string to the temp file.
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_replace::STATE_get_GREEN(const char &aChar)
{
    if (isspace(aChar))
    {
        // Expect float value here
        // Substitute the new GREEN float value in place of the 
        //  existing value.
        temp_string << config_node_vector[0].get_green() << aChar;

        // Transition to replace the BLUE RGB value
        TRAN((STATE)&rgb_replace::STATE_get_BLUE);

        // Write the temp string to the temp file.
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_replace::STATE_get_BLUE(const char &aChar)
{
    if (isspace(aChar))
    {
        // Expect float value here
        // Substitute the new BLUE float value in place of the 
        //  existing value.
        temp_string << config_node_vector[0].get_blue() << aChar;
        config_node_vector.erase(config_node_vector.begin());

//cout << "config_node_vector.size() = " << config_node_vector.size() << endl;

        // What state do we transition to?
        if (config_node_vector.empty())
        {
            // There are no more config RGB nodes available to replace...
            // Transition to NOOP and finish the file.
            TRAN((STATE)&rgb_replace::STATE_NOOP);
        } else {
            // The config_node_vector is not empty.  
            // Seek the next diffuseColor keyword
            TRAN((STATE)&rgb_replace::STATE_seek_DIFFUSECOLOR);
        }

        // Write the temp string to the temp file.
        srcFileIO->write(temp_string.str());
        word_accumulate.clear();
        temp_string.str(string());
    }
    else
    {
        word_accumulate += aChar;
    }
}

void rgb_replace::STATE_NOOP(const char &aChar)
{
//cout << "NOOP" << endl;
    // No RGB nodes left to replace.  Write the 
    //  remaining chars to the temp file.
    srcFileIO->write(aChar);
}


#if 0
int main(int argc, char *argv[])
{
    rgb_replace  rgbReplace; 

    try {
        //rgbReplace.replace("ReadMore_11.5.wrl", "temp_config1.txt");
        rgbReplace.replace("ReadMore_11.5.wrl_temp.txt", "temp_config1.txt");
        rgbReplace.replace("ReadMore_11.5.wrl_temp.txt", "temp_config2.txt");
        rgbReplace.replace("ReadMore_11.5.wrl_temp.txt", "temp_config3.txt");
    }
    catch (ErrException& caught)
    {
        cerr << caught.what() << endl;
    }

    // Success
    return 0;
}
#endif


