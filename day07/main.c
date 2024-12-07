#define AOC_IMPLEMENTATION
#include <aoc.h>

typedef uint64_t op_list;


uint64_t powu64(uint64_t a, uint64_t b) {
    uint64_t res = 1;
    for(uint64_t i = 0; i < b; ++i)
        res *= a;
    return res;
}

struct U64s {
    uint64_t* data;
    size_t size;
    size_t capacity;
};

uint64_t parse(const struct sview s) {
    uint64_t res = 0;
    for(size_t i = 0; i < s.len; ++i) {
        const char c = s.data[i];
        if(isdigit(c)) {
            res = res * 10 + s.data[i] - '0';
        }else{
            break;
        }
    }
    return res;
}

uint64_t concat(uint64_t a, uint64_t b) {
    uint64_t b_digs = 0;
    uint64_t b_mod = b;
    while(b_mod) {
        ++b_digs;
        b_mod /= 10;
    }
    const uint64_t res = a * powu64(10, b_digs) + b;
    return res;
}

static inline uint64_t eval(op_list ops, const uint64_t* operands, size_t n_operands) {
    uint64_t res = operands[0];
    for(size_t i = 1; i < n_operands; ++i) {
        const uint64_t op = op_list_get(ops, i - 1);
        if(op == 1) {
            res *= operands[i];
        }else{
            res += operands[i];
        }
    }
    return res;
}
static inline uint64_t eval2(op_list ops, const uint64_t* operands, size_t n_operands) {
    uint64_t res = operands[0];
    for(size_t i = 1; i < n_operands; ++i) {
        uint64_t tern_dig = ops % 3;
        ops /= 3;

        if(tern_dig == 1) {
            res *= operands[i];
        }else if(tern_dig == 0) {
            res += operands[i];
        }else {
            res = concat(res, operands[i]);
        }
    }
    return res;
}


bool SAT(uint64_t result, const uint64_t* operands, size_t n_operands) {
    const size_t OPS_COUNT = n_operands - 1;
    const op_list N = (1llu << OPS_COUNT);

    for(op_list i = 0; i < N; ++i) {
        uint64_t res = eval(i, operands, n_operands);
        if(res == result)
            return true;
    }

    return false;
}
bool SAT2(uint64_t result, const uint64_t* operands, size_t n_operands) {
    const size_t OPS_COUNT = n_operands - 1;
    const op_list N = powu64(3, OPS_COUNT);

    for(op_list i = 0; i < N; ++i) {
        uint64_t res = eval2(i, operands, n_operands);
        if(res == result)
            return true;
    }

    return false;
}


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    uint64_t task1 = 0;
    uint64_t task2 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;


        struct sview left, right;
        const int64_t res = sv_split(l, ':', &left, &right);

        assert(res != -1);
        right = sv_trim_left(right);
        struct U64s ints = { 0 };
        const uint64_t result = parse(left);
        
        while(right.len > 0) {
            struct sview num;
            sv_split(right, ' ', &num, &right);
            right = sv_trim_left(right);
            int val = parse(num);
            da_append(ints, val);
        }

        if(SAT(result, ints.data, ints.size)) {
            task1 += result;
        }
        if(SAT2(result, ints.data, ints.size)) {
            task2 += result;
        }

    }
    printf("task1: %lu\n", task1);
    printf("task2: %lu\n", task2);

    free(line);
    return 0;
}

