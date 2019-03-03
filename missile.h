typedef struct point {
    short x;
    short y;
} point;


typedef struct missile {
    short sx, sy;
    short dx, dy;
    short cx, cy;
    short p;
    short error;
    point progress[128];
    short shot;
    short exploded;
    short cleared;
} missile;

typedef struct cursor {
    point co[5];
} Cursor;


void missileUpdate(missile * m);
short collision(missile * m, missile * e);
void removecircle(missile * m, short x0, short y0, short radius);
void clrmissile(missile * m);
void shoot(missile * m, short sx, short sy, short dx, short dy);
void explode(missile * m, short x0, short y0, short radius);
void missileHandle(missile * m);