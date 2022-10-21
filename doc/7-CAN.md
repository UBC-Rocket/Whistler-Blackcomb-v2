# CAN Communication

To communicate between the DAQ board and the main flight computer, a can bus is used. Sensor reads and other events can then be transmitted over this bus to communicate. By default the can protocol only sets up a system for arbitrary packets of data bytes to be sent, with an id associated with it. For this project, the id of the packet is the id of the board (DAQ or FC) used to send it. The remaining bits then follow a scheme to determine what kind of data is contained. Note that this is currently not complete, as additional events (e.g. actuator activation) still have to be added, but the general setup should be similar. 

* Data byte 0: data packet id
  * Determines what type of data is being conveyed in this packet
  * Can be one of the following: 
    * 0: Pressure transducer (PT) read
    * 1: Thermocouple (TC) read
    * 2: Command read
    * [To add others later]
  * Depending on the this bytes, the remaining bytes represent different things
* Pressure transducer read
  * Data byte 1: PT id
    * [Will list PTs later once finalized]
  * Data bytes 2-5: reading in kPa
    * data is transmitted in little endian format
* Thermocouple read
  * Data byte 1: TC id
    * [Will list TCs later once finalized]
  * Data bytes 2-5: reading in degrees C
    * data is transmitted in little endian format
* Command read
  * Data byte 1: Command id
  * Data bytes 2-5: value of command

For SIM communication, the id byte is currently set to be 'c' for can packets to be received. 
