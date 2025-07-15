# Connect Four Game as a Linux Kernel Module

### Contact

**Author:** Paul Abili <br>
**Email:** pabili1@umbc.edu <br>

### Overview

<p> The goal of this project is to simulate a connect 4 game. <br>
The User MUST call RESET $ before the game can be played. <br>
Player 1, always starts the game but they can start with either R or Y depending on their RESET input. <br>
The player and the computer alternate until either the player wins, the computer wins or there is no more space left on the board. All code is within the main branch NOT the master branch. </p>

### Installation and Setup
#### Libraries used:

#include <linux/kernel.h> <br>
#include <linux/module.h> <br>
#include <linux/device.h> <br>
#include <linux/slab.h> <br>
#include <linux/string.h> <br>
#include <linux/random.h> <br>
#include <linux/init.h> <br>
#include <linux/cdev.h> <br>
#include <linux/fs.h> <br>
#include <asm/uaccess.h> <br>
#include <asm/errno.h> <br>

### Build and Compile

Build & run this program with: make && sudo insmod fourinarow.ko <br>

#### Valid Inputs:

RESET $ <br>
$ is either R or Y <br>
BOARD <br>
DROPC $ <br>
$ is a letter from A-H <br>
CTURN <br>

After a command a response will be written to the character device driver which can be checked through cat /dev/fouurianrow <br>

#### Reponses:

OK: input accepted
OOT: generic error response, returned when its the other players turn
NOGAME: the game is not active
WIN: the player won 
LOSE: the computer won
TIE: the game is a tie	
The current status of the board ex:

A B C D E F G H <br>
----------------- <br>
8|0 0 0 0 0 0 0 0 <br>
7|0 0 0 0 0 0 0 0 <br>
6|0 0 0 0 0 0 0 0 <br>
5|0 0 0 0 0 0 0 0 <br>
4|0 0 0 0 0 0 0 0 <br>
3|0 0 0 0 0 0 0 0 <br>
2|0 0 0 0 0 0 0 0 <br>
1|0 0 0 0 0 0 0 0 <br>

To close the Kernel Module: sudo rmmod fourinarow

### Testing

I liked to test by running log lines such as: <br>
<p>clear && make clean && make && sudo insmod fourinarow.ko && echo RESET R >> /dev/fourinarow && cat /dev/fourinarow && echo DROPC A >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo CTURN >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo DROPC A >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo CTURN >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo DROPC A >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo CTURN >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo DROPC A >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && echo CTURN >> /dev/fourinarow && cat /dev/fourinarow && echo BOARD >> /dev/fourinarow && cat /dev/fourinarow && sudo rmmod fourinarow </p>

<p> After such lines I would check the behavior after each drop, and I would notice if the player 1 won, noting the ability for this not to happen, because of the randomness. After the run I would check the terminal to see the behavior of the game after a game ended and would try to restart the game to see what happened and so on. </p>

### Troubleshooting

<p>I think most of my issues stemmed from what seemed like outdated documentation.
When I couldn’t replicate the functionality from a few different sources, I was so confused on what I was doing wrong, but when I tested the same code, the read function sometimes simply didn’t work and would endlessly loop. Additionally, when sources mentioned certain functions, I noticed the return type of a few changed from when it was published.</p>

<p> I also had issues with permissions but after looking to stack overflow, I was able to quickly fix it
My other main problem which I wasn’t sure how to fix was after writing it seemed like there were still characters left in the buffer. Fixing it took a long time. </p>

<p> Finally I wasn’t sure how to create an array of random non repeating integers; I couldn’t find a function to do so but it might exist. As a result, I came up with my best semblance of one in my implementation. </p>
 
### References
#### External Sources

A lot of these sources were used to understand how kernel modules work and to fix very specific issues
https://linux.die.net/lkmpg/x569.html?__cf_chl_tk=LTcIMOg5KrTP9zNJGOB8mtjsibcwepTzAatvj
nLOt1g-1744219327-1.0.1.1-rB.NMghvyEmv78up3XWhj5vMRe8jrGIOXsl8E2pOodM
https://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/
https://static.lwn.net/images/pdf/LDD3/ch03.pdf
https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html
https://stackoverflow.com/questions/62461312/simple-read-from-buffer-simple-write-to-buffer-vs-copy-to-user-copy-from-user
https://stackoverflow.com/questions/61218291/how-to-set-the-mode-for-a-character-device-in-a-linux-kernel-module
https://stackoverflow.com/questions/12124628/endlessly-looping-when-reading-from-character-device
