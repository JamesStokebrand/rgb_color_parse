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
## 'make depend' uses makedepend to automatically generate dependencies 
##               (dependencies are added to end of Makefile)
## 'make'        build executable file 'mycc'
## 'make clean'  removes all .o and executable files
##
## Purpose: 
##  This is a command line tool to extract, replace and rollback RGB nodes 
##   inside VRML V2.0 files.  (File extention WRL)
##

# define the C++ compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS = -g -std=c++11 -Wall -Werror -Wextra -pedantic
#CXXFLAGS = -O3 -std=c++11 -Wall -Werror -Wextra -pedantic

# define any directories containing header files other than /usr/include
INCLUDES =

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath
LFLAGS =

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option
LIBS = -lboost_system -lboost_filesystem

# define the CPP source files
SRCS =  rgb_node.cpp \
        rgb_extract.cpp \
        rgb_fileio.cpp \
        rgb_configio.cpp \
        rgb_replace.cpp \
        rgb_rollback.cpp \
        rgb_cmdline.cpp 

# define the CPP object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.cpp=.o)

# define the executable file 
MAIN = RGB_color_parse

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all: $(MAIN)
	@echo  Compile complete

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

rgb_node.o: include/rgb_node.h
rgb_extract.o: include/rgb_extract.h include/rgb_node.h include/rgb_fileio.h
rgb_extract.o: include/rgb_configio.h
rgb_fileio.o: include/rgb_fileio.h
rgb_configio.o: include/rgb_configio.h
rgb_replace.o: include/rgb_replace.h include/rgb_node.h include/rgb_fileio.h
rgb_replace.o: include/rgb_configio.h include/rgb_extract.h
rgb_rollback.o: include/rgb_rollback.h include/rgb_node.h
rgb_rollback.o: include/rgb_fileio.h include/rgb_configio.h
rgb_rollback.o: include/rgb_extract.h include/rgb_replace.h
rgb_cmdline.o: include/rgb_cmdline.h include/rgb_node.h include/rgb_extract.h
rgb_cmdline.o: include/rgb_replace.h include/rgb_fileio.h
rgb_cmdline.o: include/rgb_configio.h include/rgb_rollback.h
