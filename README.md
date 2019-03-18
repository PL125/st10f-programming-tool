# Programming tool for ST10F family of microcontrollers

This is my bachelor's thesis at Faculty of Information Technology at
Czech Technical University in Prague. It is publicly available in the
[CTU Digital
Library](https://dspace.cvut.cz/handle/10467/69320?locale-attribute=en). It's
written in Slovak language. The programming languages and main
technologies used were C, C++, Assembler, CMake and CTest.

## Abstract

Aim of this bachelor's thesis is a creation of cross-platform tool
allowing to write, read and erase a program memory of microcontrollers
of ST10F family and its testing. Based on analysis of existing means
for microcontroller programming, literature about these
microcontrollers and documentation of Microsoft Windows and GNU/Linux
operating systems, a new application is designed and
implemented. Result of the thesis is a cross-platform programming tool
for writing, reading and erasing of a program memory of ST10F168 and
ST10F269 microcontrollers.

The programming tool is used by [AŽD Praha s.r.o.](https://www.azd.cz)
company for maintenance of electronic units of railway electric
circuits. It is beneficial to technicians and developers, who deal
with maintenance and development of a systems which have these
programmable devices planted.

In attachment of the thesis, source codes of the programming tool can
be found.

## Results of the work

The aim of this thesis was analysis, design and implementation of
application for erasing, writing and reading internal program FLASH
memory of ST10F168 and ST10F269 microcontrollers and testing the
resulting application under supported operating systems. During the
work I encountered errors of ST10F269 microcontroller which I resolved
thanks to the information from literature and my own experiments. The
application can erase, write and read internal program FLASH memory of
ST10F168 and ST10F269 microcontrollers and identify a connected
microcontroller. Application can be built as executable for MS Windows
7 to MS Windows 10, GNU/Linux and FreeBSD. It was tested in the
environment of these operating systems. It has command-line user
interface, built-in help message and it can list supported serial line
speeds and operate on arbitrary serial port name. It has an option for
showing information messages and operation progress.

User can erase whole FLASH memory or only selected blocks of it. For
reading, user can specify how many bytes from address 0 will be read
and name of the output file to which the bytes will be saved. For
writing, user can specify if the whole memory or only blocks affected
by write operation will be erased, and the name of file containing
data for writing from address 0. Result of the write operation can be
checked by reading back content of the memory. Input and output files
are treated as binary files.

In the future the application could be extended by adding Intel Hex
file format support for input and output files, possibility of
user-specified start memory address for reading and writing, and
support for other models of microcontrollers.
