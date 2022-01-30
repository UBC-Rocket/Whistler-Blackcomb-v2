# Uploading code from the MCUXpresso to the MK66F

So you've written code and now you want to put it on the board. How do you do that?


1. Get the J-Link Mini Probe from this box.

![image](https://user-images.githubusercontent.com/69891140/151716280-574579f7-8aa1-4ac5-a8fe-745fafc95289.png)

2. Ensure that cable on the J-Link Mini is plugged in with the wire pointing away from the board, as shown:

![image](https://user-images.githubusercontent.com/69891140/151716306-62e0cfc8-4e3a-4dc6-beaa-f05352ea7155.png)

3. Plug the J-Link Mini's connector into the board at this circled spot. It will be restricted to one specific orientation.

![image](https://user-images.githubusercontent.com/69891140/151716409-b893840f-f818-4737-b88c-d6df6e253928.png)
![image](https://user-images.githubusercontent.com/69891140/151716414-39f17d59-3fd6-4d17-b658-0f134b477c48.png)

4. Use a Micro-USB cable to connect the J-Link Mini to your computer.

5. To power on the Board, there should be wires soldered on as seen in the diagram below. Connect +3.3V to the red wire soldered at GPIO, and GND to the wire soldered in CAN. IGNORE the red wire in CAN.

![image](https://user-images.githubusercontent.com/69891140/151717244-1f1ae573-186e-41f7-983d-1e57107b741c.png)

6. In MCUXpresso, in the Quickstart panel, click "Debug".

![image](https://user-images.githubusercontent.com/69891140/151716184-0bdc970c-c7c5-4da8-8b9d-f105cbf78869.png)

6. The 'J-Link EDU Minu' probe should appear in a dialog box. Press "OK".

![image](https://user-images.githubusercontent.com/69891140/151716705-845937de-adaf-4fe3-aaf9-ef90ae962882.png)

7. Press "OK" and "Accept" for any dialog boxes that appear.

8. Your IDE should now look like this. In debug mode, it pauses on the first line of the main() function. **Click the highlighted "resume" button to continue code operation**.

![image](https://user-images.githubusercontent.com/69891140/151716872-9aa19c07-7326-4d37-b521-722b7698a970.png)

9. The code should be running fine on the board now, and will pause at any breakpoints you have set in the IDE.

# Potential Errors:
1. 'Launching Whistler-Blackcomb JLink Debug' has encountered a problem.

![image](https://user-images.githubusercontent.com/69891140/151716960-e75454b7-5434-4ac4-ace6-62094893c9e3.png)

Either the board is not on, or the J-Link is not properly plugged into the board. Verify connections and voltages.

2. The code doesn't look like it's doing anything. Press this pause button and the IDE will bring you to where the last piece of code was run.

![image](https://user-images.githubusercontent.com/69891140/151717012-2923df73-c407-44e7-ad54-714a85aa3b0e.png)

If you end up here:

![image](https://user-images.githubusercontent.com/69891140/151717045-54b6c51a-07e8-4486-9077-652b2e0a49c4.png)

then that means you've allocated too much memory. Try commenting out a task or lowering the amount of memory allocated to a task by changing the value added to ``configMINIMAL_STACK_SIZE``


