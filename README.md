# RCOM Projeto 1

### Evaluation

1. Frame synchronization - state machine
2. error detection - BCC
3. ARQ - comportamento no caso de ocorrer alguma falha?
   - Automatic Reapeat Request
4. <u>**Testing with pre built application**</u>
5. **CODE ORGANIZATION**
6. Demonstration



### Valorização

1. User parameter selection: 
   - Baud rate;
   - Information frames maximum size (without stuffing).
   - Maximum number of retransmissions (default=3)
   - Timeout time selection
2. Random error handling (receiver side):
   - Should be tested on <u>header and the data fields</u>
3. REJ implementation
4. File transmission statistics
   - error recovery - close connection and reconnect it to **restart** (?? why do we have to restart the whole process ??) 
   - event log (errors)
   - Number of retransmitted & received Information frames, # of timeouts, # of sent & received REJ 



### Typical Functionality

**Goal** - Provide reliable communication between 2 systems connected by a communcation medium.

**Frame synchronization**:

- characters to delimit the start and the end of frame; (FLAG) 
- Data size may be **implicit** or **indicated** in the frame header; (C?)

Connecition estabilishment and termination (DISC, SET)

**Error control**:

- ACK after correct reception of a frame (in order and no errors)
- Timers -  transmitter decides on retransmission.  There is the case where the transmitter may close the connection and start a new process.
- NACK - If a frame is out of sequence. Receiver requests retransmission (If N(s) and N(r) how can we determine if a frame is out of order ??)
- Retransmissions can originate a duplicate which have to be detected by a receiver and deleted.
- Sequence Numbers (How ?)

**Flow Control**



### Unix Drivers

Access Method - mapped into Unix file system /dev/ttyS0 || /dev/hda1

Driver types:

- Character - 
  	READ & WRITE carried out on multuples of a character.
  	direct access (data is not stored in buffers)
- Block - 
      READ & WRITE carried out on multuples of a block of octets.
      data stored in random access buffer
- Network - 
      READ & WRITE variable size data packets
      Socket interface



### Serial Port Driver API

> functions to be read with more care in their respective man pages

**<u>File API</u>**

open() - returns a file descriptor;

read() - returns the number of bytes read;

write() - returns # of characters read;

close() - removes the lock on the file;



**<u>Termios API</u>**

**termios struct**
<u>Canonical Input</u> processing - implies that read only stops when it reaches the end of a line.
This mode has special characters like ERASE & KILL, which when received have the effect of removing the last character in the received buffer, or deleting the whole buffer.

<u>Non-Canonical Input</u> - special characters like KILL & ERASE, do not have any effect on this type of reading. The values of the mebers MIN and TIME (belonging to the c_cc array of the struct) are what will define how to process the bytes received in this mode.

c_cc[MIN]: Represents the minimum number of bytes that should be received when the read() function returns successfully.

c_cc[TIME]: Is a timer of 0.1 second granularity and it is an inter-byte timer. The timer starts when the first byte is received, and it is reset upon reception of another byte as long as timer has not exceeded the value of c_cc[TIME].

TIME & MIN configurations:

<u>TIME == 0 && MIN == 0</u>:
This is a non-blocking read where the driver returns all the data up to nbytes (defined in the read call) that are in the input queue. In the case there is no data to be collected from the input queue then read() returns 0;

<u>TIME > 0 && MIN == 0</u>: 
In the case there is data in the input queue, then the data is immediately returned to the user up to nbytes.
In the other case of the input queue not having any data, the function returns the data received until the moment the OVERALL timer is bigger than VTIME tenths.

<u>TIME > 0 && MIN > 0</u>:
In this case, the timer functions as an intercharacter timer and it starts as soon as the first byte is received and is reset everytime a byte is received. The driver returns in the case VMIN characters have been read or when the intertimer reaches the TIME value.

<u>TIME == 0 && MIN > 0</u>:
Read blocks until MIN bytes are available and returns up to th enumber of bytes requested.

> read non-canonical cases to better understand the functioning of these 2 variables:
> https://www.mkssoftware.com/docs/man5/struct_termios.5.asp



### Specification

Transparency must be guaranteed via bit stuffing.

3 types of frames:

- Supervision, Unnumbered and Information frames;
- all frames have a common header format;
- Only Information frames have a field to send data, which carries application data without interpreting it.
- Frames are protected by an error correction code:
  - In S and U, there is a simple errror correction code, since there is no data. (BCC check on the header)
  - In I frames, there is double independent protection: A BCC in the header, and a BCC for the data (even parity on each bit of the data and BCC).
- Stop and Wait error control, Unit window and modulo 2 numbering (N(r) and N(s) = 0 or 1)



### Specification frame reception

- I, S or U frames with a wrong header are ignored.
- I frames received without errors in the header and data field are accepted:
  - If it is a new frame, the data field is passed to the application and the frame is confirmed with RR.
  - In the case of it being a duplicate, the data is discarded, but the frame is confirmed with an RR anyway.
- I frames without errors in the header but with errors detected on the data field:
  - If it is a new frame REJ;
  - If it is a duplicate, reply with an RR.
- I, SET and DISC are protected by a timer:
  - If a timeout occurs, transmission should be repeated until a maximum amount of times (which should e configurable).
- All frames are delimited by flags (0x7e)
- A frame may be initialized with one or more flags, and this must be taken into considerationby the reception mechanism.

Commands: SET, DISC and I frames.

Replies: UA, RR and REJ.

The header has the following format: 

- A: 
  0x03 (if it is a command from tx -> rx or a reply from rx -> tx)
  0x01 (if it is a command form rx -> tx or a reply from tx -> rx)
- C:
  Defines the type of frame and carries the modulo 2 sequence numbers N(s) and N(r).
- BCC:
  Provides error control based on an octet that guarantees that there is an even pair of 1's for each bit position, considering all octets protected by the BCC (header or data) and the BCC before stuffing.



### Transparency

This work uses asynchronous communication which is characterized by:

- transmission of characters delimited by a start and stop bit.

For transparent communication - communication independent of the code used for transmission - it is necessary to use escape mechanisms. To be able to identify the occurence of delimiting characters in the data and replace them so that they can be correctly interpreted at the receiver.

Transmitted/Recieved characters should be interpreted as plain octets, where any of the 256 combinations are possible.

To avoid that a character inside a frame is wrongfuly recognised as a delimiting flag, bit stuffing is used:

- escape byte (0x7d);

If 0x7e occurs inside a frame, the octet is replaced by the sequence (0x7d 0x5e). This is the escape pattern followed by the **xor** operation of the replaced octed with 0x20.

If 0x7d occurs inside the frame, the octet is replaced by the sequence (0x7d 0x5d). This is the escape pattern followed by the **xor** operation of the replaced octed with 0x20.

The generation of the of BCC considers only the original octets (before stuffing), even if any octet needs to be replaced by the escape sequence.

The BCC operation is performed only after the destuffing in case there had been any substitution of the special octets previously mentioned.









### LLOPEN specifications

1. create a new termios structure which will define how the driver will be interpreted.
2. store the old termios structure to be reset after the operation terminates.
3. Inputs:
   - Porta - fd for the specified port;
   - flag - TRANSMITTER | RECEIVER;
4. return value: 
   - connection id;
   - negative value in the case of failure or error.