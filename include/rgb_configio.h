#ifndef __rgb_configio_h__
#define __rgb_configio_h__

/* 
    This file includes functions for config file manipulations.
*/

#ifndef __rgb_node_h__
#include "rgb_node.h"
#endif

class rgb_configio : public rgb_base , public rgb_state_word
{
public:
    rgb_configio()
    : rgb_base("RGB_CONFIGIO")
    , rgb_state_word((STATE)&rgb_configio::STATE_seek_START)
    {
        clear();
    }

    virtual ~rgb_configio() {}

    void clear() {
        parse_temp_node.clear();
        parse_config_vector.clear();
        parse_config_number_of_nodes = 0;
    }

    // Takes in a string containing the node config and returns a filled rgb node vector
    //  from it.
    void parse_node_config(string const &aNodeConfig, vector<rgb_node> &node_vector);

    // Takes in a full rgb node vector and returns a node config string.
    void create_node_config(vector<rgb_node> const &node_vector, 
            string const &source_file,
            string &aNodeConfig);

    // Takes in a rgb node vector and writes the node config to a file.
    void write_node_config_file(vector<rgb_node> const &node_vector, 
            string const &source_file, 
            string const &output_file);

private:
    bool read_word(string &aWord);

    void STATE_seek_START(string const &aWord);
    void STATE_seek_CONFIG_VERSION(string const &aWord);
    void STATE_seek_NUM_NODES_KEYWORD(string const &aWord);
    void STATE_read_NUM_NODES(string const &aWord);
    void STATE_read_NODE_KEYWORD(string const &aWord);
    void STATE_read_NODE_NAME(string const &aWord);
    void STATE_read_RED(string const &aWord);
    void STATE_read_GREEN(string const &aWord);
    void STATE_read_BLUE(string const &aWord);
    void STATE_NOOP(string const &aWord);

    rgb_node parse_temp_node;
    vector<rgb_node> parse_config_vector;
    unsigned int parse_config_number_of_nodes;
};

#endif

