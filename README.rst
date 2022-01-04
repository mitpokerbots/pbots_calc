pbots_calc
==========

The Pokerbots Equity Calculator
-------------------------------

NOTE: this codebase is no longer maintained or supported by the MIT Pokerbots competition.

Introduction
############

Abstract: pbots_calc is a ranged Equity Calculator for use by anyone, but
particularly poker bots in the MIT Pokerbots Competition! (mitpokerbots.com)

pbots_calc is a simple library for calculating the showdown equity for regular
texas hold'em hands, with limited support for pineapple variants. It supports
common 2-card hand range syntax (see documentation for full details), and
handles any number of players. It's built in C, using the `poker-eval library`_
for fast hand evaluation, and provides wrappers for python and java. It has
reasonable speed, but there are certainly many areas for optimization and
improvements.

The pbots_calc library is Free Software and is released under the
terms of the GNU General Public License. See the COPYING file in the
root directory of the distribution.

.. _poker-eval library: http://pokersource.sourceforge.net/

Installation
############

The next several sections provide descriptions of how one can compile the source
code. Note that because this is still considered in alpha, the current
compilation will just install everything locally. I would encourage you to leave
it that way until we reach a more stable version.

After following the instructions for your OS, you will be left with the compiled
library in the pbots_calc directory, under export/*your OS type*/lib and include
folders.

The final note regarding 32-bit vs 64-bit platforms and this library. I have
only really tested this extensively as a 32-bit library, for use with 32-bit
python and 32-bit java. So I would say that if you're in doubt or not sure what
you're doing, just stick with 32-bit installs (even if you have a 64-bit
system - it will still work!). However, there's nothing prohibiting this from
working with 64-bit tools (we've confirmed it worked on a mac), so feel free to
try. However, these instructions were written for 32-bit tools (c compiler,
java, and python).

.. _python: http://www.python.org/getit/
.. _scons: http://www.scons.org/download.php

Linux Installation
^^^^^^^^^^^^^^^^^^

First, make sure that you have gcc, python and scons installed. Installation
should be as simple as ``sudo apt-get install gcc python scons``

Now you'll need to download, patch, and install the `poker-eval library`_:

1. Download the source_ and extract it.

   a. You need to patch the source code: You can find the patch in this
      directory, called ``poker-eval_patch.diff``. If you're in the poker-eval
      directory, you can just ``patch -p0 <
      path/to/poker-eval_patch.diff``. This will fix an ``#include`` typo and
      remove a bunch of unneeded folders from the build path.

2. If not already installed, you'll need to grab the autoconf and libtool
   packages: ``sudo apt-get install autoconf libtool``

3. Proceed according to the instructions in poker-eval/README. (summarized below
   for reference.

   a. ``autoreconf --install``
   b. ``./configure``
   c. ``make``
   d. (assuming no errors) ``sudo make install``

Finally, to compile the pbots_calc library, just run ``scons`` from the
pbots_calc directory. This will compile the pbots_calc library, the c example
code, and the java example code. It will also create calculator.sh scripts for
each of the examples (more below).

.. _source: http://deb.debian.org/debian/pool/main/p/poker-eval/poker-eval_138.0.orig.tar.gz

Mac OSX Installation
^^^^^^^^^^^^^^^^^^^^

1. Follow step 1 from the Linux installation.

2.
   a. Install `homebrew <http://mxcl.github.com/homebrew/>`_.
   b. If not already installed, you need to grab the autoconf and libtool
      packages: ``brew install autoconf libtool``

3. Follow step 3 and onwards from the Linux installation

Windows Installation
^^^^^^^^^^^^^^^^^^^^

Software you will need to have installed:

1. `Python 2.7`_: Don't bother with 64 bit version. Required by scons.

2. `Microsoft Visual C++ 2010 Express`_: Needed in order to compile
   calculator. After installing, you will need to add ``C:/Program Files
   (x86)/Microsoft Visual Studio 10.0/VC/bin`` to your PATH (get help here_). You
   can check you did this right by opening a command prompt and typing "set
   PATH"

3. Scons_ - get the latest windows installer for the production release (2.2.0 as
   of last update)

.. _`Python 2.7`: http://www.python.org/getit/
.. _here: http://docs.oracle.com/javase/tutorial/essential/environment/paths.html
.. _`Microsoft Visual C++ 2010 Express`: https://www.microsoft.com/visualstudio/eng/downloads
.. _Scons: http://www.scons.org/download.php

Next, you'll need download, modify, compile, and install the `poker-eval library`_

1. Download the source_. You'll need to use 7-zip_ or similar to extract source code

2. Use Microsoft Visual C++ 2010 Express to open the ``poker-eval.sln`` found in
   the source.

   a. You'll be prompted to convert the "old" solution to a newer version - just
      do what it wants and click "Finish".
   #. Go to *Tools>Options>Projects and Solutions>Build and Run* and set the
      *maximum number of parallel project builds* to 1.
   #. Open *poker-eval/Header Files/rules_std.h* in the project and change ``#include
      <pokereval_export.h>`` to ``#include "pokereval_export.h"`` and save it.
   #. Ensure you are set to build a "Release" version (and not "Debug") by
      selecting "Release" from the drop down menu next to the "run" button
      (should be right below the help menu).
   #. Finally, right-click on the poker-eval project and select "build" - the
      output console should not report any failures!

3. Open a file explorer and navigate to the location of the poker-eval library
   you downloaded (and just built).

   a. Navigate to the "Release" directory and copy the file "poker-eval.lib" to
      ``C:/Program Files (x86)/Microsof Visual Studio 10.0/VC/lib``

   b. Also copy the all the contents from the ``include`` folder in the poker-eval library, including the "inline" folder, to a new
      folder called ``poker-eval`` in ``C:/Program Files (x86)/Microsof Visual
      Studio 10.0/VC/include``

Now, you should be able to build and install the pbots_calc library:

1. Open a command prompt
2. Navigate, by using the ``cd`` command, to the pbots_calc library folder.
3. Run ``scons``.

.. _7-zip: http://www.7-zip.org/download.html

Running the Demos
#################

Included in the library are 3 demo applications, showing how to use the library
from C, Python, and Java. The demo is the same in all languages, and is just a
command-line tool for running the library. Use standard hand range notation for
specifying cards (see examples below for clues). In each folder, you should find
a calculator.sh (or calculator.bat on Windows platforms). You can execute the
demos using this script (you may need to set the script to be executable).

Specifying Hands
^^^^^^^^^^^^^^^^

Each hand can be represented by one or more hand ranges, specified in a
comma-separated list. Multiple hands are separated by colons. Board cards are
optional and must either be 0, 3, 4 or 5 cards. You can specify 0 or more dead
(discarded) cards.

Sample 2-card hand ranges:

* 8sTd - single hand range
* 8sTd, 8sTc - 2 hand range
* 8Ts - 4 hand range: [8sTs, 8cTc, 8dTd, 8hTh]
* 8To = [8sTc, 8sTd, 8sTh, 8cTs, ...]
* 88 = [8s8c 8s8d 8s8h 8c8d 8c8h 8d8h]
* 8T = [8Ts, 8To]
* JJ+ = [JJ, QQ, KK, AA]
* 88-TT = [88, 99, TT]
* xx = random (all possible 2-card hands)

3-card hand ranges are heard to specify. Currently you can only enumerate all
the hands you mean, or specify random:

* 8s9cTd,8s9cTh,8s9sTd,...
* xxx = random (all possible 3-card hands)

Examples
^^^^^^^^

* ``calculator.sh 4hqd:js9h askcqh`` will calculate the post-flop equities for a
  hand of 4 hearts and queen diamonds against the jack spades and 9 hearts, with a
  board of ace spades, king clubs, and queen hearts.
* ``calculator.sh 4qo:jts+`` will calculate pre-flop equities for the range of
  all 4-queen offsuit hands against all jack-ten or better suited hands.
* ``calculator.sh 6d8h:xxx 4c9d7ctskd 2c`` will calculate the post-river equity
  of a hand of 6 diamonds 8 hearts vs a random (unknown) 2-card hand with a full
  board and the 2 clubs discarded.
