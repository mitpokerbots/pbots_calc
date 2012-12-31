equity-calculator
=================

Ranged Equity Calculator

WARNING: Only tested on Ubuntu 12.04 - should work on 11.04 or later, but only guessing

installation
============

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