<h1>STM32H7-APC1-sensor</h1>
<p>Program to get APC1 sensor to work on a STM32H750 microcontroller</p>
<p>
  This program tries to get data from the APC1 sensor. Its made to receive one set of measurements which are then printed to the terminal - if
  it where to work. Sometimes it receives 4, 1 or 0 bytes. Sometimes it does not even transmit the command. If you figure out what the problem
  is, then contact me.
  <b>Note again, in its current state it does not work.</b>
  Uploaded by request.
</p>

<h3>FSM:</h3>
<p>
  The final program should work as a finitive state machine. This is already implemented in main.c (the FSM folder), it just needs appropriate HAL functions (look at the comments).   
</p>

<h3>Hardware used:</h3>
<p>
  <ul>
    <li>ScioSense APC1 sensor</li>
    <li>STM32H750B-DK</li>
  </ul>
</p>
