rgb_color_parse
===============
Purpose: 
  This is a command line tool to extract, replace and rollback RGB nodes 
   inside VRML V2.0 files.  (File extention WRL)


Usage:
 ./RGB_color_parse -help
   - Displays this usage information
 
 ./RGB_color_parse -extract <a_single_wrl_file> [optional_config_file]
 ./RGB_color_parse -extract <a_directory_containing_wrl_files>
   - Extracts RGB node information from a single VRML file or all the 
      VRML files in a directory.
 
 ./RGB_color_parse -verify <a_single_wrl_file> <required_config_file>
 ./RGB_color_parse -verify <a_directory_containing_wrl_files> <required_config_file>
   - Verifies that the RGB nodes in a single VRML or all the files in a directory
      match the ones found in a required RGB config file.
 
 ./RGB_color_parse -replace <a_single_wrl_file> <required_config_file>
 ./RGB_color_parse -replace <a_directory_containing_wrl_files> <required_config_file>
   - Replaces the RGB nodes in a single VRML file or all the VRML files found in 
      a directory.  Requires a RGB config file.
 
 ./RGB_color_parse -rollback <a_single_wrl_file>
 ./RGB_color_parse -rollback <a_directory_containing_wrl_fles>
   - Rollsback the RGB nodes previously changed from the "-replace" command.
    Requires a single VRML file or all the VMRL files found in a directory.

This tool was written to help me alter RGB color nodes inside 3D printed files.  I
needed tools to extract, verify, replace and rollback RGB node information for multiple
files.

It requires BOOST::FILESYSTEM to work.

TODO:  Updated code with a singleton to change logger levels.
TODO:  Write a -diff command to show differences between config and VRML files.

