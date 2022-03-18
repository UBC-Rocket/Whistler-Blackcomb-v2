## SEGGER RTT

# Purpose:
Printf takes time and that time we've found has been correlated with some jitter in high-speed signals while we were running printfs in separate RTOS Tasks. This was particularly confusing as we had even tried using alternative RTOS functions such as `taskDelayUntil()` rather than just `taskDelay()`. To get around this we use the Real Time Transfer techology created by SEGGER which J-Link Debuggers we use.

# How to use:
Within the Utilities Folder there are the relevant files needed to be included in the project. We are using the **V754b version** to run this protocol. If you download a newer version of the J-Link software, it may come with a newer version which asks you if you would like to upgrade to the newer firmware. From minimal testing, we concluded that it's easier to use the same version as there is no unexpected behaviour which would occur when using different firmware versions. One occurence included being unable to connect to the board. If this happens, reflash the V754b firmware version onto the J-Link Debugger and do not upgrade to the newer version when asked. Alternatively, just keep this version on your computer.
<img width="254" alt="image" src="https://user-images.githubusercontent.com/24193569/158081376-431b5f91-21ee-4337-9924-9519860ed882.png">

# To Use RTT:

1. Replace your desired `printf` with the SEGGER command, as found in the examples in the above Utilities location. Debug your program

2. Open the RTT-Viewer App that was installed with your J-Link installation. For MacOS the location is as below.

```
/Applications/SEGGER/JLink_V754b
```

On Linux it was found to be the following: 

```
/opt/SEGGER/JLink
```
In this folder, there is the RTT-Viewer Application:
<img width="583" alt="image" src="https://user-images.githubusercontent.com/24193569/158081685-f96fc07c-da76-4df1-b647-d31775615361.png">

With a started debug session, connect to an Existing Session and press OK.
<img width="401" alt="image" src="https://user-images.githubusercontent.com/24193569/158081714-de028d23-78bb-404d-8329-918c82f23703.png">
After Connecting you will see your messages.
<img width="651" alt="image" src="https://user-images.githubusercontent.com/24193569/158081726-6f9d5378-8b61-4cdb-94be-fe41ae85d5ec.png">

Contact Davin for any further questions or if something is not covered in this document
