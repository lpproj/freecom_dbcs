# FreeCOM DBCS edition

a fork of FreeCOM to support:
* other platform(s) - like NEC PC-9801 series
* handle DBCS (double-byte character set) parameters correctly
* Japanese message

(yes, all tasks are work-in-progess and unstable...)

source repository of FreeCOM DBCS edition:
https://github.com/lpproj/freecom_dbcs/ (branch: dbcs)

original (upstream) source repository of (official) FreeCOM:
http://sourceforge.net/p/freedos/svn/HEAD/tree/freecom/trunk/


## How to build (DBCS edition)

You need:

* 100% DOS compatible environment (you will not build with DOSBox built-in command processor)
* Borland Turbo C++ 1.01 (I guess you can also build with Turbo C 2.0)
* Netwide assembler (DOS version) - for 16bit DOS platform, latest version is 0.98.39 on http://sourceforge.net/projects/nasm/

Build step is same as the original FreeCOM (copy config.b to config.bat, copy config.std to config.mak, modify config.mak and config.bat as you like, then invoke `build.bat`) but build.bat for DBCS editon has some extra options:

* dbcs - build as DBCS edition
* nec98 - build for NEC PC-98x1 platform

For example, to build FreeCOM DBCS edition for PC-98 with XMS-Swap, 
build command is:

  `build -r dbcs nec98 xms-swap japanese`


## CREDITS

For DBCS edition, some works of preceders are merged:

* suzu (http://dos.minashiro.net/freecomj.html)
    * codes around DBCS and PC-98
    * Japanese translation (and other contributors: guri, Silphire, etc...)
* akm (http://astra.digi2.jp/ , http://lsair.html.xdomain.jp/a/e/commandj-11.html)
    * Japanese translation

