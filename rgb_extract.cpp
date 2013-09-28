/*
    This node defines the parsing and extraction if rgb color nodes from a WRL VRML file.  
*/

#ifndef __rgb_extract_h__
#include "include/rgb_extract.h"
#endif

#ifndef __rgb_fileio_h__
#include "include/rgb_fileio.h"
#endif

void rgb_extract::extract_nodes(string const &in_file, vector<rgb_node> &rgb_list_vector)
{
    // This will throw an exception if it cannot open the source file.
    rgb_fileio input_file(in_file.c_str());

    // Init the state machine and our temp variables
    clear();

    // Parse the file.
    //  - Verify its a VRML file
    //  - Seek the node name
    //  Loop:
    //      - seek the rgb color node keyword
    //      - grab the three RGB colors
    // 

    string aWord;
    while (input_file.read_word(aWord)) {
        process(aWord);
    }

#if 0
    cout << "Number of nodes found: " << rgb_list.size() << endl;
    for (vector<rgb_node>::iterator it = rgb_list.begin(); it < rgb_list.end(); it++)
    {
        cout << *it << endl;
    }
#endif

    if (rgb_list.size() == 0) 
    {
        // No RGB nodes extracted ... this is an error.
        string anError("No rgb nodes were extracted from \"" + in_file
            + "\".  Please check your input file.");
        throw_exception(anError);
    }

    // Success.  Copy the vector list to the input parameter.
    rgb_list_vector = rgb_list;

    // Close the opened input file.
    input_file.close(); 

}

void rgb_extract::extract(string const &file_name, string const &rgb_node_file_name) 
{
    vector<rgb_node> aVector;
    extract_nodes(file_name, aVector);

    // aVector should have some nodes.  Format and write them to an output node file.
    rgb_fileio fileIO;
    string temp_node_file_name;
    if (rgb_node_file_name == "")
    {
        temp_node_file_name = file_name + CONST_STRING_DEFAULT_RGB_NODE_FILE_EXTENTION;
    }
    else {
        temp_node_file_name = rgb_node_file_name;
    }

    fileIO.write_node_config_file(aVector, file_name, temp_node_file_name);
}

bool rgb_extract::verify(string const &file_name, string const &rgb_node_file_name)
{
    vector<rgb_node> source_file_rgb_nodes;
    vector<rgb_node> config_file_rgb_nodes;
    extract_nodes(file_name, source_file_rgb_nodes);

    rgb_fileio fileIO;
    fileIO.parse_node_config(rgb_node_file_name, config_file_rgb_nodes);


#if 0
// Debug code

for (vector<rgb_node>::const_iterator it = source_file_rgb_nodes.begin();
        it < source_file_rgb_nodes.end(); it++)
{
    cout << *it << endl;
}
cout<< endl;

for (vector<rgb_node>::const_iterator it = config_file_rgb_nodes.begin();
        it < config_file_rgb_nodes.end(); it++)
{
    cout << *it << endl;
}
cout << endl;

cout << "Source file size: " << source_file_rgb_nodes.size() << endl;
cout << "Config file size: " << config_file_rgb_nodes.size() << endl;
#endif

    // Extracted RGB nodes from both the source file and the 
    // config file. 

    // Determine if they are equal
    if (source_file_rgb_nodes == config_file_rgb_nodes)
    {
        return true;
    }
    return false;
}

void rgb_extract::STATE_verify_VRML(const string &aWord)
{
    STATE_verify_required_word(
        STRING_error_layer,
        aWord, 
        CONST_STRING_VRML_KEYWORD, 
        (STATE)&rgb_extract::STATE_verify_VRML_VER);
}

void rgb_extract::STATE_verify_VRML_VER(const string &aWord)
{
    STATE_verify_required_word(
        STRING_error_layer,
        aWord,
        CONST_STRING_VRML_VER_KEYWORD, 
        (STATE)&rgb_extract::STATE_verify_VRML_CHARSET);
}

void rgb_extract::STATE_verify_VRML_CHARSET(const string &aWord)
{
    STATE_verify_required_word(
        STRING_error_layer,
        aWord, 
        CONST_STRING_VRML_CHARSET_KEYWORD, 
        (STATE)&rgb_extract::STATE_verify_seek_DEF);
}

void rgb_extract::STATE_verify_seek_DEF(const string &aWord)
{
    if (aWord == CONST_STRING_DEF_KEYWORD)
    {
        temp_node.clear();
        last_word.clear();
        TRAN((STATE)&rgb_extract::STATE_verify_seek_Transform);
    }
}

void rgb_extract::STATE_verify_seek_Transform(const string &aWord)
{
    if (aWord == CONST_STRING_TRANSFORM_KEYWORD)
    {
        // Save the last word as the node name

        // NOTE this will happen several times.  
        // We want the word before the TRANSFORM keyword followed by the
        // diffuseColor keyword.
        temp_node.set_name(last_word);
    } else if (aWord == CONST_STRING_DIFFUSECOLOR_KEYWORD) {
        // The next word is the RED RGB value.
        TRAN((STATE)&rgb_extract::STATE_verify_get_RED);
    } else {
        last_word = aWord;
    }
}

void rgb_extract::STATE_verify_get_RED(const string &aWord)
{
    temp_node.set_red(atof(aWord.c_str()));
    TRAN((STATE)&rgb_extract::STATE_verify_get_GREEN);
}

void rgb_extract::STATE_verify_get_GREEN(const string &aWord)
{
    temp_node.set_green(atof(aWord.c_str()));
    TRAN((STATE)&rgb_extract::STATE_verify_get_BLUE);
}

void rgb_extract::STATE_verify_get_BLUE(const string &aWord)
{
    temp_node.set_blue(atof(aWord.c_str()));
    rgb_list.push_back(temp_node); // Store the extracted RGB node in the vector.
    TRAN((STATE)&rgb_extract::STATE_verify_seek_Transform); // Go back to seeking the node name.
}


int main(int argc, char *argv[])
{
    rgb_extract RGB_extract;
    vector<rgb_node> rgb_listing;

    try {
        //RGB_extract.extract_nodes("test_file.txt", rgb_listing); // Should give an error.
        //RGB_extract.extract_nodes("ReadMore_11.5.wrl", rgb_listing); // Should work...
        RGB_extract.extract("ReadMore_11.5.wrl"); // Should work...
        if (RGB_extract.verify("ReadMore_11.5.wrl", 
            "ReadMore_11.5.wrl_rgb_nodes.txt"))
        {
            cout << "Config file ReadMore_11.5.wrl_rgb_nodes.txt matches the nodes in ReadMore_11.5.wrl" << endl;
        } else {
        
            cout << "Config file ReadMore_11.5.wrl_rgb_nodes.txt DOES NOT MATCH the nodes in ReadMore_11.5.wrl" << endl;
        }

    }
    catch (ErrException& caught)
    {
        cerr << "ERROR:" << caught.what() << endl;
    }

    return 0; // success
}

