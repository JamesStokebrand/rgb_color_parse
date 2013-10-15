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
## Filename: rgb_node.cpp
##  This file holds the "operator<<" for the rgb_node class and the 
##   init values for the singleton.
##
## Usage: 
##   -help  : Prints usage information
##   -info  : Prints usage information
##
*/
#include "include/rgb_node.h"

ostream& operator<<(ostream &out, const rgb_node &A)
{
    out << A.name << " "
        << A.red << " "
        << A.green << " "
        << A.blue;
    return out;
}

// Init the singleton
int LoggerLevel::_referenceCount = 0;  
LoggerLevel* LoggerLevel::_instance = NULL;



