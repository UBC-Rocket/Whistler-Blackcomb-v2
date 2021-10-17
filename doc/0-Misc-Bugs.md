# Miscellaneous bugs

## Purpose

If anyone ever encounters something while developing on this project that makes you think "wow, I really wish I had known that beforehand" or "man, I hope nobody has to debug an error as stupid as that again", __write it here!__ There's not point in struggling with the same bug twice. This document is meant to be a compilation of many of such issues. Even if it's unlikely that anyone else will encounter it (e.g. specific to environment), it can be useful just for your own sake. 

## FreeRTOS

* Do not dynamically allocate memory/arrays inside tasks
  * This will mess with execution flow and cause tasks to fail
  * If you really need to there are special functions to do so
  * For example, **do not** do either of the following inside the task: 
```C
int arr[20];
int* arr = (int*) malloc(20 * sizeof(int));
```

* Stack size
  * If your program seems to be working up until you add a seemingly innocent line, you're stack size might be too low
  * To fix this change the stack size parameter when creating the task

* Tasks cannot end!
  * FreeRTOS has separation anxiety and is not comfortable with tasks ending unannounced. It may block and act strangely.
  * If a task is going to end, I think you can avert this issue with `vTaskSuspend(NULL);` at the end of the task. 

## General

* Don't forget to flush the buffer for things to be printed
    * For example having a "blink" sketch repeatedly running the first following line will yield no result! 
    * This sounds obvious (and indeed it is) but it's very easy to forget

```C
printf("Hello World"); // Bad
printf("Hello World\n"); // Good
```