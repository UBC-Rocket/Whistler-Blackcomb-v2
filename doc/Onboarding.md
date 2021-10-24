# Onboarding

Welcome to Whistler-Blackcomb Avionics! We are thrilled with your new aura joining our spirit collective (this is a direct sarcastic quote from Eric). 

Here's a few things that you might want to know when joining the team:

## Meetings

Like all Whistler-Blackcomb teams, we take part in the team-wide meeting on Sundays at 10:00 am in Hennings 120, after which we do a quick workspace clean up. Then we break out into separate sub team meeting where each person gives a quick update, and spend the rest of the day working on our separate tasks. 

In addition to the main weekly meeting, we will have a midweek meeting every week. The exact time/day of this meeting isn't set in stone and can be discussed on a per-term basis to suit everyone's availability. This meeting will most likely consist of a quick update on what everyone's progress is, and then an optional work period afterwards. Attendance is expected but isn't as important as for the Sunday meetings, so if something comes up don't worry too much about missing the midweek meeting. 

## Slack

For team communication we have a UBC Rocket wide slack, with a wb-avionics channel for coordination between teams. Generally for specific things like availability or quick questions we use direct messages to either individual people or the team of active members, whereas the general wb-avionics channel isn't used as much. 

## Google Drive

UBC Rocket uses google drive as a centralized location for resources and files relating to the team. If you're reading this you should have been added to it already, but if not let Xander know and it can be arranged. We have a folder for our subteam at UBC Rocket -> WHISTLER BLACKCOMB -> Subteams -> Avionics, although finding things in the drive is quite difficult and most of our documentation is kept here on GitHub instead. 

## Whistler Blackcomb Project Background

To get familiar with the setup and organization of the Whistler Blackcomb Project, by far the best resource is the [external design review](https://docs.google.com/document/d/1aMXL6ogCYWBh2T_MYrKgUeNxM_nnbr7gomK3zXRZ8YI/edit?usp=sharing) that was completed at the beginning of 2021. It's obviously way too long and too detailed for a full reading, but skimming over the main sections quickly will give a good sense about a high level overview of the project. For a description of what the avionics system looks like specifically, read our section in the EDR in more detail. Note that specific things might have changed since its writing, but most high level things are the same. 

## Team Structure

Generally work is divided between electrical, firmware and software tasks. Electrical work is things like PCB design and manufacturing, firmware is working on code that will be on the flight computer sent to space and software tasks are those pertaining to the ground station GUI. 

To keep track of tasks and who is responsible for completing them, we use github projects. This can be seen [here](https://github.com/orgs/UBC-Rocket/projects/1) (you have to be logged in to see it). Tasks are initially in the todo section before any actual work has started on them, and once you start working you can move them to the in progress column. Once it's completed and you merge you're pull request/close the issue the item will automatically go to the finished column. The nice thing about using the built in functionality of github in this way is that because each of the tasks is tied to an issue it makes keeping track of progress much simpler. 

For code/pcb contributions, we use github as version control to keep everyone up to date with the current version of the project. We follow the fairly standard github workflow of doing work on a new branch, and then starting a pull request to merge to master/main once the work has been completed. For pull requests you should have someone review your work before finally merging. 

## GitHub Access

No matter what type of work you will be doing, you will need access to the teams GitHub repositories. Given this guide is itself hosted on GitHub you likely already know how to access the team's repos, and to get member status contact Xander. 

## Next Steps

Depending on what you'll primarily be working on, here are some next steps to get started contributing to the project. Even if you're only expecting to do one kind of work, if you're interested I'd definitely encourage you to set up the development environment for the other aspects of the team if only to get a better understanding of what's going on in the project. 

### Firmware

The firmware for the flight computer is on GitHub [here](https://github.com/UBC-Rocket/Whistler-Blackcomb-v2). The first steps to get set up with it, regardless of operating system is to follow [this guide](../doc/MCUXpresso-Setup.md), which will get you set up with MCUXpresso for board compilation. 

Next, follow [this guide](../doc/x86-Compilation.md) to set up your ability to compile our firmware for your local machine for testing. A more detailed explanation of this functionality is in the guide. If you are running Windows and either don't want to install wsl or want a native build system, then follow [this guide](../doc/Windows-Compilation-with-Cygwin.md) to enable native building instead of wsl. 

After doing these your development environment should be all set for firmware work! Explanations of specific implementations are linked in the readme file for the firmware. 

### Hardware

We use Altium for all our PCB design work. To get a free student version of Altium visit [this link](https://www.altium.com/solutions/academic-programs/student-licenses) and fill out the form with your @student.ubc.ca email address. They sometimes take a while to get back about the licenses, so be prepared to potentially have to wait a few days before getting access. 

While waiting or if you need to access Altium Files from a non-Windows computer or one that does not have Altium installed, you can use [Altium's Online Viewer](https://www.altium.com/viewer/). You still need to clone the hardware repository (or at least the specific PCBDoc file) in order to upload it to the viewer. This does not require a license and can get you looking at boards quickly although you cannot make edits.

Even if you don't have your license yet, you can still get the projects set up. First download Altium from its [download page](https://www.altium.com/products/downloads), although you won't be able to edit PCB documents without the license. Next download the following PCB projects by cloning them through git, and open them in Altium to ensure everything is working properly: 

* [Main flight computer](https://github.com/UBC-Rocket/WB-AV-4500-FC)
* [Data acquisition board](https://github.com/UBC-Rocket/WB-AV-4500-FC)
* [Battery management system](https://github.com/UBC-Rocket/WB-AV-4501-BMS)

Once you get your student license you should be set up for electronics development. 

### Software

This section to be completed later...

## Your turn!

If there was anything you found confusing or misleading in any of the onboarding documentation, please don't hesitate to help improve it! If you found something confusing other people might too, so please either let someone on the team know or implement the change/clarification in the markdown yourself. 
