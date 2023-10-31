\page page-docs-developer_setup Developer Environment Setup

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes how to set up the development environment for Sygaldry,
as well as the convenience scripts provided to facilitate development.

# Important Note

If you have any trouble setting up your development environment, please create
an issue on the github repository.

\warning Don't clone the repo yet unless you're sure you know how to
recursively clone all its submodules. Otherwise, we'll get to that step in a
minute.

# Overview

The process, in brief, is as follows:

1. Install `nix` according to the upstream documentation
[here](https://nixos.org/download), or that of your operating system
distribution.
2. Obtain a copy of the Sygaldry source code repository *and its submodules, recursively*,
using `git`, e.g. `git clone --recurse-submodules url-or-ssh-path/to/DocSunset/sygaldry`.
3. Use `nix-shell` to automatically install all dependencies, run the test-suite,
and compile instrument firmware.

This may be enough information for experienced developers to get set up. If
any uncertainty lingers, then read on.

# Basic Development Environment

## macOS

Run the `terminal` app and try to run `git --version`. If you have not already
installed developer tools, follow the instructions to do so.

## Linux

Most likely there is nothing required for you to do here.

## Windows

Windows support is still a work in progress. Please try installing a VirtualBox
VM with your preferred Linux distribution.

# Installing Nix

## Linux

Your distribution may have specific instructions for installing `nix`. Check the
documentation.

## Otherwise

Navigate to https://nixos.org/download and follow the upstream install instructions.

### What's Nix?

We use `nix` in this project because it is a convenient way of allowing all
users to quickly set up an identical development environment. Don't worry too
much about how this works, as the use of `nix` in this project is very limited
and simple to achieve. But if you're curious anyways...

Nix is a package manager, like `homebrew`, `apt`, `pacman`, or `pip`, but
rather than installing everything directly in your system directories, `nix`
installs things in "the Nix store", with every package in its own
content-hashed isolated directory. Commands like `nix-shell` can then be used
to draw these isolated directories together into a highly controlled
development environment that can be easily replicated across different
machines. You may think of it as a cross between a package manager and python
`venv`. Alternatively, if a container system like `docker` is a more
lightweight version of a virtual machine, you could think of `nix` as providing
a more lightweight version of a container system.

# Cloning Sygaldry

While Sygaldry is still a private repository, this step can be somewhat
involved, especially for less experienced developers. You will require a github
user account and permission to access the repository (ask Travis).

Once you have access to the repository, you can clone it.

If you are familiar with the use of git, make sure to clone the repository
*with all its submodules*, e.g. `git clone --recurse-submodules
https://github.com/DocSunset/sygaldry.git`. Once the repo is cloned, continue with
the next step.

If you are unfamiliar with the use of git, open your terminal and run
`nix-shell -p github-cli`. Nix will install the github command line interface
and drop you into a `nix` virtual environment in which it is accessible.

You now have a few options:

- use the github command line interface (aka `gh`) with https
- use the github command line interface with ssh
- use `git` with ssh

## Using the github command line interface with https

Run `gh auth login` from the `nix-shell` you enabled in the previous section.
Choose `HTTPS` as your preferred protocol for git operations, and then follow
the instructions to log in.

Once you are logged in, run `gh repo clone DocSunset/sygaldry -- --recurse-submodules`
to clone the repository and its submodules.

Continue with the convenience scripts.

## Using the github command line interface with ssh authentication

In the `nix-shell` that you enabled in the previous section, run `ssh-keygen -t
ed25519` and follow the onscreen instructions to create an ssh key pair. This
will create a private key and a public key. You should never share your private
key, but it is normal to upload your public key to someone else's server.

Run `gh auth login` from the `nix-shell` you enabled in the previous section.
Choose `SSH` as your preferred protocol for git operations, and then follow
the instructions to log in.

Once you are logged in, run `gh repo clone DocSunset/sygaldry -- --recurse-submodules`
to clone the repository and its submodules.

## Using git with ssh authentication

In the `nix-shell` that you enabled in the previous section, run `ssh-keygen -t
ed25519` and follow the onscreen instructions to create an ssh key pair. Use the
default name and path for the new keys, unless you know what you are doing. This
will create a private key and a public key. You should never share your private
key, but it is normal to upload your public key to someone else's server.

Use `cat` to display the public key, e.g. `cat /home/myuser/.ssh/id_ed25519.pub`.
Copy the public key to your clipboard.

Log in to github in a web browser and navigate to Settings, then "SSH and GPG Keys".
Click the green button to add a "New SSH key". Give it a title such as the hostname
of your computer. Select "Authentication Key" as the key type. In the "Key" text field,
paste the public key. Click "Add SSH key".

In the terminal, run `git clone --recurse-submodules
git<AT>github.com:DocSunset/Sygaldry.git`, with an at-sign where it says `<AT>`
to clone the repository and its submodules.

# Ready to go!

`cd` your terminal to the root of the cloned repository and run `nix-shell
--pure`. This may take a little while the first time, while `nix` sets up the
development environment. Once you reach the command prompt, you are now ready
to go!

# Convenience Scripts

Several convenience scripts are provided to facilitate development. When the
nix shell environment is activated by running `nix-shell --pure`, these can be
run directly, e.g. `test.sh`, since the `sh` directory is prepended to the
binary search path. Alternatively, you may run `nix-shell --pure --run
'script.sh'` to run a given script without dropping into the nix shell
environment.

- \subpage page-sh-lili
    - Run `lili` on each literate source file that has been update, regenerating tangled machine code
- \subpage page-sh-test
    - Compile and run tests for portable components.
- \subpage page-sh-update_docs
    - Update the doxygen documentation website repository
- \subpage page-sh-idf
    - Install the ESP-IDF and build an IDF-based instrument firmware.
- \subpage page-sh-pico_sdk
    - Install the Raspberry Pi Pico SDK and build a Pico SDK-based instrument firmware.
