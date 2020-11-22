#!/bin/sh
# Written by Tomas Smetana, 28th October 2004

# these files are _required_ bu automake
# but aren't being created automtically
touch NEWS README AUTHORS ChangeLog;

echo "Running aclocal..."
aclocal
echo "Running autoheader..."
autoheader
echo "Running automake..."
automake -ac
echo "Running autoconf..."
autoconf
echo "Running configure..."
./configure;
echo;
echo "Now type 'make' to build the project.";
echo;
