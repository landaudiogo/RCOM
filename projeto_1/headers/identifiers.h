#ifndef ID_H
#define ID_H

// frame MACROS
#define FLAG 0x7e // frame delimiter
// address bytes
#define A0   0x01 // Command rx->tx || Reply tx->rx
#define A1   0x03 // Command tx->rx || Reply rx->tx
// control bytes for UNNUMBERED and SUPERVISION frames
#define SET  0x03
#define DISC 0x0b
#define UA   0x07
#define RR0  0x01
#define RR1  0x21
#define REJ0 0x01
#define REJ1 0x21
// control bytes for UNNUMBERED and SUPERVISION frames
#define IC0  0x00
#define IC1  0x02

#define TRANSMITTER 240
#define RECEIVER    241

#define MAX_RETRY   3 
#define TIMEOUT     3 // 3 seconds 


#endif
