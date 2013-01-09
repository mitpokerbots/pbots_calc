pbots_calc: The Pokerbots Equity Calculator
==========================================

Abstract: pbots_calc is a ranged Equity Calculator for use by anyone, but
particularly poker bots in the MIT Pokerbots Competition! (mitpokerbots.com)

pbots_calc is a simple library for calculating the showdown equity for regular
texas hold'em hands, with limited support for pineapple variants. It supports
common 2-card hand range syntax (see documentation for full details), and
handles any number of players. It's built in C, using the poker-eval library
(http://pokersource.sourceforge.net/) for fast hand evaluation, and provides
wrappers for python and java. It has reasonable speed, but there are certainly
many areas for optimization and improvements.

The pbots_calc library is Free Software and is released under the
terms of the GNU General Public License. See the COPYING file in the
root directory of the distribution.

WARNING: Only tested on Ubuntu 12.04 - should work on 11.04 or later, but only
guessing

Linux Installation
==================

First, required other libraries.

Scons - "sudo apt-get install scons"

poker-eval
 - get source via "svn co http://svn.gna.org/svn/pokersource/tags/poker_eval_138_0_1/poker-eval poker-eval"
   - need to patch the source code: run "patch -d poker-eval -p0 < poker-eval_patch.diff"
 - install autoconf and libtool packages "sudo apt-get install autoconf libtool"
 - (rest is from poker-eval/README):
   - "autoreconf --install"
   - "./configure"
   - "make"
   - (assuming no errors) "sudo make install"

Now, you should be able to build and install all libraries in this library:

 - scons build
 - sudo scons install
 - sudo ldconfig

Windows Installation
====================

Software you will need to have installed:

Python 2.7: http://www.python.org/getit/ (don't bother with 64 bit version). Required by scons

Microsoft Visual C++ 2010 Express:
https://www.microsoft.com/visualstudio/eng/downloads Needed in order to compile calculator.
 - After installing, you will need to modify your PATH to reflect where the new
   tools are - see
   http://docs.oracle.com/javase/tutorial/essential/environment/paths.html for
   hints. You should ensure that __/VC/bin is now on your PATH variable by opening a command prompt
   and typing "set PATH"

Scons - get the latest windows installer for the production release (2.2.0 as of
last update) from http://www.scons.org/download.php

poker-eval
 - get source from http://download.gna.org/pokersource/sources/poker-eval-138.0.tar.gz
   - use 7-zip (http://www.7-zip.org/download.html) to extract source code
 - Open visual c++ express.
   - Go to Tools>Options>Projects and Solutions>Build and Run and set the "maximum number of parallel project builds" to 1.
   - Open poker-eval.sln
     - Open Includes>rules_std.h and change the line
            #include <pokereval_export.h>
         to
              #include "pokereval_export.h"
   - Right click on the poker-eval project and select "build" - the output console should not report any failures!
 - Go to the location where you downloaded the source to
   - navigate to the "Debug" directory and copy poker-eval.lib to __/VC/libs
   - copy the all the contents, including the "inline" folder, to a new folder called poker-eval in__/VC/includes

Now, you should be able to build and install all libraries in this library:

 - scons build
 - scons install

C example
=========

just run ./calculator from the examples/ for usage instructions

Example run: "./calculator 4qo:jts+ 4cqs9h" will run range of all 4-queen
 offsuit hands against all jack-ten or better suited hands (pokerstove style
 ranges!) with a board of 4 clubs, queen spades and 9 hearts.

Python example
==============

"python calculator.py" from the python/ dir

Java example
============

"./calculator.sh" from java/ dir