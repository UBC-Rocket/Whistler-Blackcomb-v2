# Whistler Blackcomb Flight Computer Firmware

[![UBC-Rocket](https://circleci.com/gh/UBC-Rocket/Whistler-Blackcomb-v2.svg?style=shield)](https://app.circleci.com/pipelines/github/UBC-Rocket/Whistler-Blackcomb-v2)
![Commit Activity](https://img.shields.io/github/commit-activity/m/ubc-rocket/whistler-blackcomb-v2)
![Code Size](https://img.shields.io/github/languages/code-size/ubc-rocket/whistler-blackcomb-v2)
![Pull Requests](https://img.shields.io/github/issues-pr/UBC-Rocket/Whistler-Blackcomb-v2)


This is a repository with the code for the UBC Rocket Whistler Blackcomb flight computer. A compiled version of it should hopefully at some point be in [space](https://en.wikipedia.org/wiki/K%C3%A1rm%C3%A1n_line). 

## Project Structure

This is a high level overview of the file structure we use for this project. Any folders not mentioned here are libraries/drivers that are not custom developed. 

* board
  * Generated files by MCUXpresso for pin configuration
* common
  * Code that is shared between x86 and board implementations, i.e. logic code
* data
  * Misc. files used for testing or logging
* doc
  * Project documentation in markdown
* envs
  * Hardware abstraction layer that calls hardware level functionality directly
* source
  * Main program entrance point
* test
  * Unit test folder
* tools
  * Misc. tools for ease of development

## Project Documentation

All documentation is currently kept in the [doc/](doc/) directory. Note that although this is the firmware repository, all documentation for the wb-avionics subteam is located here for ease of finding things. 

### Onboarding Documents

1. General new members [guide](doc/Onboarding.md). Read this first. 

### Firmware Project Setup

1. MCUXpresso project setup [guide](doc/MCUXpresso-Setup.md). 
2. x86 Compilation explanation and setup [guide](doc/x86-Compilation.md). 
3. Setting up Windows compilation with Cygwin [guide](doc/Windows-Compilation-with-Cygwin.md). This is only strictly  necessary if you want to be able to do software-in-the-loop testing with [groundstation](https://github.com/UBC-Rocket/UBCRocketGroundStation).

### Firmware Concept Guides

1. Uploading code from MCUXpresso to our board [guide](doc/Firmware-Upload.md)
2. Miscellaneous bugs and unexpected interactions that you want to warn people about should go in this [list](doc/Misc-Bugs.md). 
3. How continuous integration works and how to access ours in this [guide](doc/Continuous-Integration.md). 
4. FreeRTOS installation and usage [guide](doc/RTOS-Basics.md).
5. Radio system [guide](doc/Radio.md). 
6. CAN communication [guide](doc/CAN.md)

### Legacy Functionality

1. Uploading to Teensy [guide](doc/Teensy-Upload.md).
