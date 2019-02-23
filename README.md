# LEDTimedCandle

I did a lot of research on battery powered LED candle lights circuits and code and was unable to find a time controlled LED candle light.


As I have some ATTiny13 left, I decided to use this little chip. As I want the LED to do a candle light for about 4hours and then stay of for another 20 hours, I need on timer and counter. Another timer/PWM is needed to immitate a candle flickering. But the ATTiny13 has only one timer. On the ATTiny13 you can either use the timer or PWM.


Fortunately the ATTiny has also a Watchdog timer that can be used to call an interrupt function. The watchdog runs with a separate 128kHz clock, independent from the CPU clock. The largest timeout is 8 seconds. So I need to count this 450 times to have one hour.


The code makes the ATTiny13 sleep for another 8 seconds or light the LED. The ON phase is 4 hours and the sleep phase will be extended to 20 hours. The sequence starts with the ON phase, when power is applied. In the ON phase I measure about 3mA and in the OFF phase the circuit needs 300µA.


The circuit is documented in the Arduino code file. Running the ATTiny13 at lower clock than 9.6MHz did not change the power usage. But I switched to the 1.2MHz internal clock and disabled BOD (auto power down for low power) to get a longer runtime with two or three AA batteries.


