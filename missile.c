#include "missile.h"
void missileUpdate(missile * m) {
    OLED_setPixel(m->cx, m->cy);
    /*
        Adapted from http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
        Using the Bresenham line algorithm.
    */
    int e2;

    int dx = absi(m->dx - m->sx);
    int dy = absi(m->dy - m->sy);

    int sx = m->sx < m->dx ? 1 : -1;
    int sy = m->sy < m->dy ? 1 : -1;

    if(m->cx == m->dx && m->cy == m->dy) {
        m->exploded = 1;
        m->shot = 0;
        return;
    }

    e2 = m->error;

    if (e2 > -dx) { m->error -= dy; m->cx += sx; }
    if (e2 <  dy) { m->error += dx; m->cy += sy; }

    m->progress[m->p].x = m->cx;
    m->progress[m->p].y = m->cy;
    m->p++;    
}



short mcollisiondetect(missile * m, missile * enemy) { 
    short mcollision = 0;

    if(m->progress[m->p].x == enemy->progress[enemy->p].x && m->progress[m->p].y == enemy->progress[enemy->p].y) {
        mcollision = 1;
    }

    return mcollision;
}

short explosioncollision(missile * m, short a, short b, short radius) {
    short ecollision = 0;

    if((m->progress[m->p].x - a)*(m->progress[m->p].x - a) + (m->progress[m->p].y - b) * (m->progress[m->p].y - b) <= radius * radius) {
        ecollision = 1;
    }

    return ecollision;
}

short collision(missile * m, missile * e) {
    if(!(m->exploded || e->exploded)) {
        if(powi(m->cx - e->cx, 2) + powi(m->cy - e->cy, 2) <= 25)
            return 1;
        if(powi(e->cx - m->cx, 2) + powi(e->cy - m->cy, 2) <= 25) 
            return 1;

    }

    if(m->cy == e->cx && m->cy == e->cy) {
        return 1;
    }

    return 0;
}

void removecircle(missile * m, short x0, short y0, short radius) {
    short k, l;
    for(k = -radius; k <= radius; k++) {
        for( l = -radius; l <= radius; l++) {
            if(k*k + l*l <= radius*radius) {
                OLED_clrPixel(x0 + l, y0 + k);
            }
        }
    }
}


void clrmissile(missile * m) {
        while(m->p >= 0) {
            OLED_clrPixel(m->progress[m->p].x, m->progress[m->p].y);
            m->p--;
        }
       removecircle(m, m->cx, m->cy, 4); 
}

void shoot(missile * m, short sx, short sy, short dx, short dy) {

    clrmissile(m);

    m->sx = sx;
    m->sy = sy;
    m->cx = sx;
    m->cy = sy;

    m->dx = dx;
    m->dy = dy;
    m->shot = 1;
    m->p = 0;
    m->exploded = 0;

    m->error = 0xff;
    m->cleared = -6;

    int dx_ = absi(m->dx - m->sx);
    int dy_ = absi(m->dy - m->sy);

    int sx_ = m->sx < m->dx ? 1 : -1;
    int sy_ = m->sy < m->dy ? 1 : -1;
    m->error = (dx_ > dy_ ? dx_ : -dy_) / 2;

}

void explode(missile * m, short x0, short y0, short radius)
{
    short k, l;
    for(k = -radius; k <= radius; k++) {
        for( l = -radius; l <= radius; l++) {
            if(k*k + l*l <= radius*radius) {
                OLED_setPixel(x0 + l, y0 + k);
            }
        }
    }
    m->exploded = 0;


}

void missileHandle(missile * m) {
    if(m->shot) {
        missileUpdate(m);
    }
    if(m->exploded && m->shot == 0) {     
        explode(m, m->cx, m->cy, 4);
    }
    if(m->exploded == 0 && m->shot == 0 && m->cleared == 0) {
        clrmissile(m);
        m->cleared = 1;
    } else if(m->exploded == 0 && m->shot == 0) {
        m->cleared++;
    }

}
