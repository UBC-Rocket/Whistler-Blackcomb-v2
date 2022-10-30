# Writing Clean Code:

Standards for our team development starting Summer 2022.





### Including Header Files


See below to add paths to compile when building our MCUXpresso File.
Under Project Properties, select C/C++ Build, then Settings
In the Window Pane open MCU C Compiler, then select Includes. 
Follow the naming convention of existing includes to allow you to use an `#include <library.h>` statement in your code.
https://community.nxp.com/t5/LPCXpresso-IDE-FAQs/Adding-Include-paths-to-a-project/m-p/467852


### Utilize "camelCase"
October 30, 2022

- Variables (including structs) are lower camelCase
- Functions are lower camelCase
- Types are "<type>_t" 

