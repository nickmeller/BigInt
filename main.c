#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef int* data;

const int INIT_SIZE = 64;
const int BASE = 1000*1000*1000;

typedef struct {
    data num;
    int sign;
    size_t capacity;
    ptrdiff_t tail;
} big_int;

size_t size(big_int * a) {
    return a->tail + 1;
}

void ensure_capacity(big_int * b) {
    b->num = realloc(b->num, b->capacity * 2);
    b->capacity *= 2;
}

void push(big_int * b, int value) {
    if (b->tail + 1 == b->capacity) ensure_capacity(b);
    b->num[++b->tail] = value;
}

int back(big_int * b) {
    return b->num[b->tail];
}

void clean_front_zeros(big_int * b) {
    while (b->tail > 0 && back(b) == 0) {
        b->tail--;
    }
}

void get_minus(big_int * a) {
    if (back(a) < 0) {
        a->sign = 0;
        a->num[a->tail] = abs(back(a));
    }
}

void invert_sign(big_int * a) {
    a->sign = !a->sign;
}

void swap(big_int * a, big_int * b) {
    big_int * t = a;
    a = b;
    b = t;
}

void init(big_int *b,const char * xstr) {
    b->tail = -1;
    b->sign = 1;
    b->capacity = INIT_SIZE;
    char * str = malloc((strlen(xstr) + 1) * sizeof(char));
    strcpy(str, xstr);
    b->num = malloc(sizeof(int) * INIT_SIZE);
    for (ptrdiff_t i = strlen(str); i > 0; i-= 9) {
        str[i] = 0;
        push(b, atoi(i>=9 ? str + i - 9 : str));
    }
    clean_front_zeros(b);
    get_minus(b);
}

big_int * init_num(int a) {
    big_int * returnable = malloc(sizeof(big_int));
    returnable->sign = (a >= 0 ? 1 : 0);
    returnable->capacity = 1;
    returnable->tail = 0;
    returnable->num = malloc(sizeof(int));
    returnable->num[0] = a;
    return returnable;
}

big_int * init_cpy(big_int * a) {
    if (!a) exit(EXIT_FAILURE);
    big_int * ret = malloc(sizeof(big_int));
    ret->sign = a->sign;
    ret->num = malloc(sizeof(int) * a->capacity);
    ret->capacity = a->capacity;
    ret->tail = a->tail;
    mempcpy(ret->num, a->num, sizeof(int) * a->capacity);
    return ret;
}

void print(big_int * b) {
    if (!b->sign) printf( "-");
    printf("%d", b->tail == -1 ? 0 : back(b));
    for (ptrdiff_t i = b->tail - 1; i >= 0; i--)
        printf("%09d", b->num[i]);
}

int equals(big_int * a, big_int * b) {
    if (a->tail == b->tail) {
        for (ptrdiff_t i = a->tail; i >= 0; i--) {
            if (a->num[i] != b->num[i])
                return 0;
        }
        return 1;
    }
    return 0;
}

int more(big_int * a, big_int * b) {
    if (a->tail > b->tail) {
        return 1;
    } else if (a->tail == b->tail) {
        for (ptrdiff_t i = a->tail; i >= 0; i--) {
            if (a->num[i] < b->num[i])
                return 0;
            else if (a->num[i] == b->num[i])
                continue;
            else return 1;
        }
        return 0;
    }
    return 0;
}

int less(big_int * a, big_int * b) {
    return (!(more(a, b) || equals(a, b)));
}

ptrdiff_t max(ptrdiff_t a, ptrdiff_t b) {
    if (a - b >= 0) return a;
    else return b;
}

big_int * minus(big_int * x, big_int * b) {
    if (more(x, b) || equals(x, b)) {
        big_int * a = init_cpy(x);
        a->sign = 1;
        int carry = 0;
        for (size_t i = 0; i < size(b) || carry; i++) {
            a->num[i] -= carry + (i < size(b) ? b->num[i] : 0);
            carry = a->num[i] < 0;
            if (carry) a->num[i] += BASE;
        }
        while (size(a) > 1 && back(a) == 0)
            a->tail--;
        return a;
    } else {
        big_int * a = minus(b, x);
        a->sign = 0;
        return a;
    }
}

big_int * plus(big_int * x, big_int * y) {
    big_int *a = init_cpy(x);
    if (a->sign == y->sign) {
        int carry = 0;
        for (size_t i = 0; i <= max(a->tail, y->tail); i++) {
            if (i == a->tail + 1) {
                push(a, 0);
            }
            a->num[i] += carry + (i <= y->tail ? y->num[i] : 0);
            carry = a->num[i] >= BASE;
            if (carry) a->num[i] -= BASE;
        }
        return a;
    }
    else {if (a->sign) return minus(a, y); else return minus(y, a);}
}

big_int * multi(big_int * x, big_int * b) {
    big_int * a = init_cpy(x);
    data c = malloc(sizeof(int) * (size(a) + size(b)));
    memset(c, 0, sizeof(int) * (size(a) + size(b)));
    for (size_t i = 0; i < size(a); i++) {
        for (int j = 0, carry = 0; j < (int)size(b) || carry; j++) {
            long long cur = c[i + j] + a->num[i] * 1ll * (j < size(b) ? b->num[j] : 0) + carry;
            c[i + j] = (int)(cur % BASE);
            carry = (int)(cur / BASE);
        }
    }
    a->sign = a->sign && b->sign || (!a->sign) && (!b->sign);
    a->tail = (size(a) + size(b)) - 1;
    a->capacity = (size(a) + size(b));
    free(a->num);
    a->num = c;
    while (size(a) > 1 && back(a) == 0)
        a->tail--;
    return a;
}

int lessOrEquals(big_int * a, big_int * b) {
    return less(a, b) || equals(a,b);
}

big_int * div2(big_int * a) {
    int carry = 0;
    for (ptrdiff_t i = size(a) - 1; i>=0; i--) {
        long long cur = a->num[i] + carry * 1ll * BASE;
        a->num[i] = (int) cur / 2;
        carry = (int) cur % 2;
    }
    while (size(a) > 1 && back(a) == 0)
        a->tail--;
    return a;
}

big_int * sqroot(big_int * a) {
    if (!a->sign) exit(EXIT_FAILURE);
    big_int * l = init_num(1);
    big_int * r = init_cpy(a);
    while(less(l, minus(r, init_num(1)))) {
        big_int * m = div2(plus(l, r));
        if (less(multi(m,m), a)) {
            l = m;
        } else {
            r = m;
        }
    }
    return r;
}

big_int * divl(big_int * a, big_int * b) {
    big_int * l = init_num(1);
    big_int * r = init_cpy(a);
    while(less(l, minus(r, init_num(1)))) {
        big_int * m = div2(plus(l, r));
        if (less(multi(m,b), a)) {
            l = m;
        } else {
            r = m;
        }
    }
    return r;
}

char * input_str(char * str, FILE * in) {
    int capacity = 64;
    str = malloc(sizeof(char) * capacity);

    char ch;
    size_t sz = 0;
    while ((ch = getc(in) != EOF) && ch != ' ' && ch != '\n') {
        if (sz == capacity) {
            capacity *= 2;
            str = realloc(str, sizeof(char) * capacity);
        }
        str[sz++] = ch;
    }
    if (sz == capacity) {
        str = realloc(str, sizeof(char) * (capacity + 1));
    }
    str[sz] = '\n';
    return str;
}

void input_file(char * input, char * op, char * left, char * right) {
    FILE * in = fopen(input, "r");
    if (!in) {
        fprintf(stderr, "Error while opening file");
        exit(EXIT_FAILURE);
    }
    fscanf(in, "%s", op);
}


int main(int argc, char ** argv) {
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments" );
        exit(EXIT_FAILURE);
    }
    char * op = malloc(sizeof(char) * 3);
    char * op1;
    char * op2;
    return 0;
}
