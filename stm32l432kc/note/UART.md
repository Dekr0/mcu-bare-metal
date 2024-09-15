## UART

### Introduction

- UART (Universal Asynchronous Receive and Transmission Protocol) is a 1 to 1 
serial transmission protocol.

#### Serial vs parallel

- Serial transmission means data are sent one bit after the another.
- Parallel transmission means bits that consist of the whole data are sent in 
parallel.
- Serial is usually slower and require more clock cycles due to each bit is sent 
one after another instead of a single clock pulse all the bits.

#### Serial communication with clock vs no clock

- A serial communication protocol that relies on a clock is synchronous. Otherwise, 
it's asynchronous.
- When a protocol relies on a lock, it means it will send data at a specific speed 
and specific time.

### UART Configuration

- For a protocol like UART that does not relies on a clock, it will require 
configuration in the following settings before transmission.
    - Transmission speed
    - Data length
    - Start and stop bit
- Transmission speed (in bauds / second) is for receiver to detect when a single 
bit starts and ends, and for transmitter to know how long it should hold the 
signal at a specific signal strength.
    - A common speed is 9600 bauds / second. That means the length of a pulse bit 
    is divided by 9600, which is 104 us.
- Start and stop bit is for receiver to detect when the whole data starts and ends. 
- Usually, the data bit will not be right at the cycle starts because the data bit 
can be unstable. Instead, for each data bit, it will be saved in the middle of the 
a signal pulse.
