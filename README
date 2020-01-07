The provided Nachos sources are a modified version from the (very old) original
nachos project. This is why there are some differences between the actual state
and some information in old documents.

Documentation
=============
- README.old is the original README. It is a bit outdated nowadays, in
  particular about the way to get and compile Nachos.
- nachos.ps.gz is the original Nachos documentation. Most of it is still valid,
  but the installation procedure and build system that have been totally
  modified.

Build System
============
The Nachos kernel has several components/features that can be enabled or not.
The build system allows one to compile different subset of features.

All code is build into the code/build/ subdirectory.

For each Nachos subset of features (this is called a flavor):
- the flavor is named (initialy, there are 5 provided flavors : filesys,
  network, threads, userprog and vm)
- for each flavor named 'name', you can find:
  - 'name'-'sourcefilename'.o: the object files used to create the kernel
  - nachos-'name': the (linux-executable) nachos kernel
- for each user program, you can find:
  - 'program'.o: the (cross-compiled) object file
  - 'program': the MIPS executable (must be run through a Nachos kernel)

More information about the build system is available in code/README.Makefiles
(how to create your own flavors, how to add a source file to a flavor, etc.)

Sources
=======
All source code is in the code/ subdirectory. Here are several subdirectories:
- bin/: helper programs for the build system. Should not be modified.
- build/: where build file are created
- machines/: this is the code that emulate the hardware. You must not modifie
  anything here but you want to modify the emulated machine. The classical only
  modification is the size of the physical RAM when you start to handle several
  threads and processus.
- filesys/
- network/
- threads/
- userprog/
- vm/
  These are the 5 subdirectories where source files for Nachos kernel are
  found. You will have to modify them and to create new ones.
  Depending on the chosen flavors, not all source files are used into the
  various nachos-'flavor' binaries (See "Build System" above)
    All these files are compiled with g++ natively (in x86 32 bits however)
- test/
  This is where all user programs are written. New files are automatically
  compiled as new programs.
    Makefile.define-user has support to define libraries (ie sources files
  compiled and linked to all user programs) or more complex programs (ie
  programs that link several object files, etc.)

Git
===
Nowadays, git is used to manage the sources.

  Before, no control version system where used and students were asked to put
all their modifications between "#ifdef CHANGED ... #endif" markers. You might
still found some remarks about this in the subjects. Just ignore it.
  You can use 'git diff official/master' to look at all your modifictions.

