# Purpose
This doc covers setting up visual studio and cygwin to create a `.exe` file for windows. 

# Create a separate repository
For now, at least, we think it's best to use separate repository for the windows compilation and linux compilation. 
Create a new reposityory with `git pull` or `git clone`.

# Install Cygwin
Cygwin is "a large collection of GNU and Open Source tools which provide functionality similar to a Linux distribution on Windows and 
a DLL (cygwin1.dll) which provides substantial POSIX API functionality", which we'll need.

From https://www.cygwin.com/, select `Install Cygwin by running setup-x86_64.exe`, download, and run the executable to start the install wizard.

# Configuring Cygwin Installation
Under 'Choose a download source', select 'install from internet'
