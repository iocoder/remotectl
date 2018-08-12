# RemoteCtl

Thanks for using RemoteCtl. Check install section below for information
on how to install RemoteCtl.

## Dependencies

  - qt >= 5.11.1
  - qtermwidget5 >= 0.9.0
  - libssh2 >= 1.5.0
  - ssh
  - sshpass
  - scp
  - vim
  - pkg-config
  - g++
  - qmake

You might want to configure libssh2 with the command:
`./configure --prefix=/usr --enable-debug --enable-gex-new`.

## Install

Simply create a build directory inside the repository tree, then
execute `cmake ..` command, which will create the Makefiles. Next,
you need to execute `make` to compile the program and `sudo make install`
to install the binaries to your system.

## Running

Execute `remotectl` command to run the program.
