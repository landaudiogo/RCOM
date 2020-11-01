//flag
#define F 0b01111110

//address
#define A_I_T       0b00000011
#define A_SET_T     A_I_T
#define A_DISC_T    A_I_T
#define A_UA_R      A_I_T
#define A_RR_R      A_I_T
#define A_REJ_R     A_I_T

#define A_I_R       0b00000001
#define A_SET_R     A_I_R
#define A_DISC_R    A_I_R
#define A_UA_T      A_I_R
#define A_RR_T      A_I_R
#define A_REJ_T     A_I_R

//control
#define C_I_0       0b00000000
#define C_I_1       0b00000010
#define C_RR_0      0b00000001
#define C_RR_1      0b00100001
#define C_SU_0      0b00000101
#define C_SU_1      0b00100101