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
        a->sign = 1;
        a->num[a->tail] = abs(back(a));
    }
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

void minus(big_int * a, big_int * b) {
    if (more(a, b) || equals(a, b)) {
        a->sign = 1;
        int carry = 0;
        for (size_t i = 0; i < size(b) || carry; i++) {
            a->num[i] -= carry + (i < size(b) ? b->num[i] : 0);
            carry = a->num[i] < 0;
            if (carry) a->num[i] += BASE;
        }
        while (size(a) > 1 && back(a) == 0)
            a->tail--;
    } else {
        minus(b, a);
        big_int *t = b;
        b = a;
        a = t;
        a->sign = 0;
    }
}

void plus(big_int * a, big_int * b) {
    if (a->sign == b->sign) {
        int carry = 0;
        for (size_t i = 0; i <= max(a->tail, b->tail); i++) {
            if (i == a->tail + 1) {
                push(a, 0);
            }
            a->num[i] += carry + (i <= b->tail ? b->num[i] : 0);
            carry = a->num[i] >= BASE;
            if (carry) a->num[i] -= BASE;
        }
    }
    else {if (a->sign) minus(a, b); else minus(b, a);}
}

void multi(big_int * a, big_int * b) {
    data c = malloc(sizeof(int) * (size(a) + size(b)));
    for (size_t i = 0; i < size(a); i++) {
        for (ptrdiff_t j = 0, carry = 0; j < size(b) || carry; j++) {
            long long cur = c[i + j] + a->num[i] * 1ll * (j < size(b) ? b->num[j] : 0) + carry;
            c[i + j] = cur % BASE;
            carry = cur / BASE;
        }

    }
    a->sign = a->sign && b->sign || (!a->sign) && (!b->sign);
    a->tail = (size(a) + size(b)) - 1;
    a->capacity = (size(a) + size(b));
    a->num = c;
    while (size(a) > 1 && back(a) == 0)
        a->tail--;
}



int main() {
    big_int a;
    big_int b;
    init(&a, "-200000");
    print(&a);
    printf("\n");

    init(&b, "3000000000000000000000000000000000000000234");
    print(&b);
    printf("\n");

    plus(&a, &b);
    print(&a);
    printf("\n");
    return 0;
}
