
short mi = 0;
short ei = 0;

short gx = 64;
short gy = 32;
short gox, goy;

short prev = 0;
int enmissilecounter = 0;
short timeoutcount = 0;

missile ms[MISSILEMAX];
missile en[MISSILEMAX];

Cursor c;

#define TMR2PERIOD ((80000000 / 256) / 1000)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big."
#endif
