/* 
    This file holds the functions for file manipulations.
*/

#ifndef __rgb_fileio_h__
#include "include/rgb_fileio.h"
#endif

void rgb_fileio::open(string const& source, bool temp_file_wanted)
{
    // Opens a WRL file for reading.  
    // If temp_file_wanted was TRUE will create a temp output file for writing.

    source_file_stream.open(source.c_str());
    if (!source_file_stream.is_open())
    {
        // Failed to open source file.  Return error.
        throw_exception("Unable to open input file \"" + source + "\".");
    }

    source_file_name = source;
        
    if (temp_file_wanted) 
    {
        // Open the requested temp file for writing.
        temp_file_name = source + CONST_STRING_DEFAULT_TEMP_FILE_EXTENTION;
        temp_file_stream.open(temp_file_name.c_str());

        if (!temp_file_stream.is_open())
        {
            // Failed to open the temp file.  Return error.
            throw_exception("Unable to open temp file \"" + temp_file_name + "\".");
        }
        temp_file_opened = true;
    }
}

bool rgb_fileio::read_word(string &aWord)
{
    // Will read one word from the original file.

    if((source_file_stream >> aWord) &&
        source_file_stream.good())
    {
        return true;
    }
    return false;
}

bool rgb_fileio::read_char(char &aChar)
{
    // Will read one char from the original file.
    if ( !source_file_stream.eof() && 
        (aChar = source_file_stream.get()) &&
         source_file_stream.good())
    {
        return true;
    }
    return false;
}

void rgb_fileio::write(string const &A)
{
    // Will write the string to the temp file.
    //  Fails if a temp file was not created at open()
    if (!temp_file_opened) {
        throw_exception("Unable to write.  Temp file was not created at open().");
    } else {
        temp_file_stream << A;
    }
}

void rgb_fileio::overwrite()
{   
    // Moves the original file to a bak status.
    // Changes the name of the temp file to the original file.
    // Deletes the original file because the above operation succeeded.
}

void rgb_fileio::close()
{
    // Close the temp and source file.
    source_file_stream.close();

    if (temp_file_opened) {
        temp_file_stream.close();
        temp_file_opened = false;
    }
}

void rgb_fileio::parse_node_config(string const &aNodeConfig,
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

        // Seek to the beginning
        inputSS.seekp(0);

        // Process the string
        while (inputSS >> aWord)
        {
            process(aWord);
        }
    } 
    else // Attempt to open it as a filename.
    {
        open(aNodeConfig);
        while (read_word(aWord))
        {
            process(aWord);
        }
    }

//cout << "Expected: " << parse_config_number_of_nodes << " Read :"  << parse_config_vector.size() << endl;

    // Success
    node_vector = parse_config_vector;
    clear(); // Clear out existing code
    
}


void rgb_fileio::create_node_config(vector<rgb_node> const &node_vector,
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

// DEBUG
//cout << "Source file : " << source_file << endl;

    // Construct the config file 
    config << CONST_STRING_CONFIG_START_KEYWORD << " " << CONST_STRING_CONFIG_CURRENT_VERSION << "\n";
    config << CONST_STRING_CONFIG_COMMENT_KEYWORD << " source file : " << source_file << "\n";

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
        config << node_vector[ii].get_blue() << "\n";
    }
    config << CONST_STRING_CONFIG_END_KEYWORD << "\n" << "\n";

    aNodeConfig = config.str();
}

void rgb_fileio::write_node_config_file(vector<rgb_node> const &node_vector,
    string const &source_file,
    string const &output_file)
{
    string aConfig;
    create_node_config(node_vector, source_file, aConfig);

    // Write config file to the output_file
    ofstream out_file(output_file.c_str());

    out_file << aConfig;

    out_file.close();
    
}

void rgb_fileio::STATE_seek_START(string const &aWord)
{
    STATE_verify_required_word(
        STRING_error_layer,
        aWord,
        CONST_STRING_CONFIG_START_KEYWORD,
        (STATE)&rgb_fileio::STATE_seek_CONFIG_VERSION);
}

void rgb_fileio::STATE_seek_CONFIG_VERSION(string const &aWord)
{
    STATE_verify_required_word(
        STRING_error_layer,
        aWord,
        CONST_STRING_CONFIG_CURRENT_VERSION,
        (STATE)&rgb_fileio::STATE_seek_NUM_NODES_KEYWORD);
}

void rgb_fileio::STATE_seek_NUM_NODES_KEYWORD(string const &aWord)
{
    if (aWord == CONST_STRING_CONFIG_NUM_NODES_KEYWORD)
    {
        // #NUM_NODES keyword found ... read the number of
        //  nodes to find.
        TRAN((STATE)&rgb_fileio::STATE_read_NUM_NODES);
    }
}


void rgb_fileio::STATE_read_NUM_NODES(string const &aWord)
{
    parse_config_number_of_nodes = atoi(aWord.c_str());
    TRAN((STATE)&rgb_fileio::STATE_read_NODE_KEYWORD);
}

void rgb_fileio::STATE_read_NODE_KEYWORD(string const &aWord)
{
    if (aWord == CONST_STRING_CONFIG_NODE_KEYWORD)
    {
        // Start of a #NODE <NAME> <R> <G> <B> sequence
        TRAN((STATE)&rgb_fileio::STATE_read_NODE_NAME);
    }
    if (aWord == CONST_STRING_CONFIG_END_KEYWORD)
    {
        // #END keyword means this config is finished.
        // transition to NOOP and do nothing else
        TRAN((STATE)&rgb_fileio::STATE_NOOP);
    }
}

void rgb_fileio::STATE_read_NODE_NAME(string const &aWord)
{
    parse_temp_node.set_name(aWord);
    TRAN((STATE)&rgb_fileio::STATE_read_RED);
}

void rgb_fileio::STATE_read_RED(string const &aWord)
{
    parse_temp_node.set_red(atof(aWord.c_str()));
    TRAN((STATE)&rgb_fileio::STATE_read_GREEN);
}

void rgb_fileio::STATE_read_GREEN(string const &aWord)
{
    parse_temp_node.set_green(atof(aWord.c_str()));
    TRAN((STATE)&rgb_fileio::STATE_read_BLUE);
}

void rgb_fileio::STATE_read_BLUE(string const &aWord)
{
    parse_temp_node.set_blue(atof(aWord.c_str()));
    TRAN((STATE)&rgb_fileio::STATE_read_NODE_KEYWORD);

    // Save the completed RGB node in the vector
    parse_config_vector.push_back(parse_temp_node);
    parse_temp_node.clear();
}

void rgb_fileio::STATE_NOOP(string const &aWord)
{
    // Do nothing...
}
