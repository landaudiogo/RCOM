# Notes on the Application Code



### Transmitter

1) runs llopen:

- if this function returns -1 then it is assumed an error occured, otherwise everything functions properly.

2) start a cycle to read a file: 

- starts by reading up to MAX_PAYLOAD_SIZE from the file
- sends the bytes read into llwrite:
  - before sending the buffer, it writes in the first position of the buffer either 1 or 0.
    - 1 represents writing to the file
    - 0 represents terminating the writing operation
  - if this function returns < 0 then an error occured;
- if bytes read is 0, then llwrite will send one byte with value 0x0 and end transmission;

3) llclose to end connection between sender and receiver



### Receiver



1) llopen return -1 in case it fails nayhing else otherwise

2) starts a cycle to write to a created file:

- llread
  - if llread returns < 0 then an error has occured
  - if it has more than 0 then there are 2 scenarios
    - if the first value written to the buffer == 1 then we are to write it to the file
    - if the first value written to the buffer == 0 then we close the connection

3) llclose