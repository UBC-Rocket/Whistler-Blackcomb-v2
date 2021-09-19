# Whistler Blackcomb Flight Computer Firmware

[![UBC-Rocket](https://circleci.com/gh/UBC-Rocket/Whistler-Blackcomb-v2.svg?style=shield)](https://app.circleci.com/pipelines/github/UBC-Rocket/Whistler-Blackcomb-v2)
![Commit Activity](https://img.shields.io/github/commit-activity/m/ubc-rocket/whistler-blackcomb-v2)
![Code Size](https://img.shields.io/github/languages/code-size/ubc-rocket/whistler-blackcomb-v2)
![Pull Requests](https://img.shields.io/github/issues-pr/UBC-Rocket/Whistler-Blackcomb-v2)


This is a repository with the code for the UBC Rocket Whistler Blackcomb flight computer. A compiled version of it should hopefully at some point be in [space](https://en.wikipedia.org/wiki/K%C3%A1rm%C3%A1n_line). 

## Project Documentation

All documentation is currently kept in the [doc/](doc/) directory. Note that although this is the firmware repository, all documentation for the wb-avionics subteam is located here. 

### Onboarding Documents

1. General new members [guide](doc/Onboarding.md). Read this first. 

### Firmware Project Setup

1. MCUXpresso project setup [guide](doc/MCUXpresso-Setup.md). If you are new to the project, this is a good place to start.
2. x86 Compilation explanation and setup [guide](doc/x86-Compilation.md). This is a good second step.
3. Setting up Windows compilation with Cygwin [guide](doc/Windows-Compilation-with-Cygwin.md). This is only strictly  necessary if you want to be able to do software-in-the-loop testing with [groundstation](https://github.com/UBC-Rocket/UBCRocketGroundStation).

### Electrical Project Setup

### Firmware Concept Guides

1. How continuous integration works and how to access ours in this [guide](doc/Continuous-Integration.md). 
2. FreeRTOS installation and usage [guide](doc/RTOS-Basics.md).
3. Radio system [guide](doc/Radio.md). 
4. CAN communication [guide](doc/CAN.md)

### Legacy Functionality

* Uploading to Teensy [guide](doc/Teensy-Upload.md).
