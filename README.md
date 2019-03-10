# LEDTimedCandle

I did a lot of research on battery powered LED candle lights circuits and code and was unable to find a time controlled LED candle light.


As I have some ATTiny13 left, I decided to use this little chip. As I want the LED to do a candle light for about 4hours and then stay of for another 20 hours, I need on timer and counter. Another timer/PWM is needed to immitate a candle flickering. But the ATTiny13 has only one timer. On the ATTiny13 you can either use the timer or PWM.


Fortunately the ATTiny has also a Watchdog timer that can be used to call an interrupt function. The watchdog runs with a separate 128kHz clock, independent from the CPU clock. The largest timeout is 8 seconds. So I need to count this 450 times to have one hour.

Update:
The 128KHz Oscillator for wdt is in real a 131072Hz oscillator (mixing k=1000 and k=1024!).
The ATTiny13A Data sheet says wdt osc will run at 113000Hz at 2V/20°C:

    1024k cycles are 1048576 cycles
    so:
    113000 / 1048576 = 0.1077 => 9.285 seconds
    131072 / 1048576 = 0.125  => 8     seconds
    for one hour we get:
    450*8    = 3600    = 60    minutes
    450*9.285= 4178.27 = 69.63 minutes
    the error cumulates to
    4h 60*4    = 240 
       69.63*4 = 278.55
    diff is 38.55 minutes! in 4 hours
    for 24 hours:
    24h @ 8seconds = 1440
        @ 9.285sec = 1671.12
    diff is 231 minutes for a day! this is 3.85 hours (3h51m) a day!

Instead of theoratic 450 cycles of 8seconds for an hour:
    60*60/9.285=387.72
I now use 388 cycles.

The code makes the ATTiny13 sleep for another 8 seconds or light the LED. The ON phase is 4 hours and the sleep phase will be extended to 20 hours. The sequence starts with the ON phase, when power is applied. In the ON phase I measure about 3mA and in the OFF phase the circuit needs 3µA (without my ISP programmer attached :-).


The circuit is documented in the Arduino code file. Running the ATTiny13 at lower clock than 9.6MHz did not change the power usage. But I switched to the 1.2MHz internal clock and disabled BOD (auto power down for low power) to get a longer runtime with two or three AA batteries.

# References:
* 8.4 Watchdog timer, page 38 ->says 128kHz but means 131072 Hz (kilo for Hertz is 1000 and not 1024 normally)
* watchdog timer prescaler table 8-2, page 43
* Figure 19-65. 128 kHz Watchdog Oscillator Frequency vs. VCC, page 157 ->shows wdt frequency from 105000 to 115000, where is the 128kHz (131072Hz)?

