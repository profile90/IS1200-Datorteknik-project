
float absf(float x) {
    if(x < 0) {
        return (x * -1);
    }
    else {
        return x;
    }
}
int absi(int x) {
    if(x < 0) {
        return (x * -1);
    }
    else {
        return x;
    }
}
float pow(float x, float y) {
    short i;
    float result = x;
    for(i = 1; i < y; i++) {
        result *= result;
    }
    return result;
}
short powi(int x, int y) {
    short i;
    short result = x;
    for(i = 1; i < y; i++)
        result *= result;
    return result;
}
float sqrt(float x) {
    float x0 = x;
    float x1;
    for(; ;) {
        x1 = x0 - (x0 * x0 - x)/(2 * x0);
        float delta = (x1 - x0)/ x0;
        if(delta < .000001 && delta > -.000001) {
            return x1;
        }
        x0 = x1;
    }
}


void swap(short * x, short * y) {
    short temp = *x;
    *x = *y;
    *y = temp;
}
