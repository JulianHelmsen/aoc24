#define AOC_IMPLEMENTATION
#include <aoc.h>

struct machine {
    uint64_t pc;
    int64_t a;
    int64_t b;
    int64_t c;
};


struct p_sol {
    int bit;
    uint64_t value;
    size_t len;
};

struct partial_solution_list {
    size_t capacity;
    size_t size;
    struct p_sol* data;
};

void disassmble(const int* prg, size_t prg_size) {
    for(size_t i = 0; i + 1 < prg_size; i += 2) {
        const size_t pc = i;
        const int opcode = prg[pc];
        const int operand_literal = prg[pc + 1];

        char op[3] = { '#', '0' + operand_literal , '\0'};
        switch(operand_literal) {
        case 0:
        case 1:
        case 2:
        case 3:
              break;
        case 4:
              op[0] = '$';
              op[1] = 'A';
              break;
        case 5:
              op[0] = '$';
              op[1] = 'B';
              break;
        case 6:
              op[0] = '$';
              op[1] = 'C';
              break;
        case 7:
              fprintf(stderr, "Invalid operand_literal: %d\n", operand_literal);
              exit(1);
              break;
        default:
              fprintf(stderr, "Unknown operand_literal: %d\n", operand_literal);
              exit(1);
              break;
        }

        switch(opcode) {
        case 0: // division
            printf("\t$A /= (#1 << %s)\n", op);
            break;
        case 1: // bxl
            printf("\t$B ^= #%d\n", operand_literal);
            break;
        case 2: // bst
            printf("\t$B ^= %s %% #8\n", op);
            break;
        case 3: // jnz
            printf("\tif($A != 0) goto %d\n",  operand_literal);
            break;
        case 4: // bxc
            printf("\t$B ^= $C\n");
            break;
        case 5: {// out
            printf("\tprint %s %% #8\n", op);
            break;
        }
        case 6: // bdv
            printf("\t$B = $A / (#1 << %s)\n", op);
            break;
        case 7: // cdv
            printf("\t$C = $A / (#1 << %s)\n", op);
            break;
        }
    }
}

bool execute(struct Ints* output, struct machine m, const int* prg, size_t prg_size, bool compare, const int* cmp_values, size_t cmp_size) {
    while(m.pc + 1 < prg_size) {
        assert(m.pc + 1 < prg_size);
        const int opcode = prg[m.pc];
        const int operand_literal = prg[m.pc + 1];
        int64_t operand = 0;


        switch(operand_literal) {
        case 0:
        case 1:
        case 2:
        case 3:
              operand = (int) operand_literal;
              break;
        case 4:
              operand = m.a;
              break;
        case 5:
              operand = m.b;
              break;
        case 6:
              operand = m.c;
              break;
        case 7:
              fprintf(stderr, "Invalid operand_literal: %d\n", operand_literal);
              exit(1);
              break;
        default:
              fprintf(stderr, "Unknown operand_literal: %d\n", operand_literal);
              exit(1);
              break;
        }

        switch(opcode) {
        case 0: // division
            m.a /= (1l << operand);
            break;
        case 1: // bxl
            m.b ^= operand_literal;
            break;
        case 2: // bst
            m.b = operand % 8;
            break;
        case 3: // jnz
            if(m.a != 0) {
                m.pc = operand_literal - 2;
            }
            break;
        case 4: // bxc
            m.b = m.b ^ m.c;
            break;
        case 5: {// out
            int64_t out = operand % 8;
            da_append((*output), (int) out);
            if(compare) {
                if(output->size > cmp_size)
                    (void) out;
                    //return false;
                else if(output->data[output->size - 1] != cmp_values[output->size - 1])
                    return false;
            }
            break;
        }
        case 6: // bdv
            m.b = m.a / (1l << operand);
            break;
        case 7: // bdv
            m.c = m.a / (1l << operand);
            break;
        }
        m.pc += 2;
    }
    return compare ? output->size == cmp_size : true;
}

void print_output(struct Ints output) {
    printf("\"");
    if(output.size > 0)
        printf("%d", output.data[0]);
    for(size_t i = 1; i < output.size; ++i)
        printf(",%d", output.data[i]);
    printf("\"");
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;

    struct Ints prog = { 0 };
    struct machine m = { 0 };
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        if(sv_starts_with(l, sv_create("Register"))) {
            const int64_t idx = sv_find_char(l, ':');
            if(idx == -1) {
                fprintf(stderr, "malformed register line: %.*s\n", (int) l.len, l.data);
                exit(1);
            }
            const char r_name = l.data[idx - 1];
            const struct sview snum = sv_trim_left(sv_subview(l, idx + 1, SVIEW_NPOS));
            const int64_t val = sv_parse_i64(snum);

            switch(r_name) {
                case 'A':
                    m.a = val;
                    break;
                case 'B':
                    m.b = val;
                    break;
                case 'C':
                    m.c = val;
                    break;
                default:
                    fprintf(stderr, "Invalid register: %.*s\n", (int) l.len, l.data);
                    exit(1);
            }
        }else if(sv_starts_with(l, sv_create("Program:"))) {
            struct sview nums = sv_trim_left(sv_subview(l, 1 + sv_find_char(l, ':'), SVIEW_NPOS));
            prog = parse_ints(nums, ',');
        }
    }
    free(line);
    disassmble(prog.data, prog.size);

    struct Ints output = { 0 };
    execute(&output, m, prog.data, prog.size, false, NULL, 0);
    
    printf("Task1: ");
    print_output(output);
    printf("\n");

#if 0
    output.size = 0;
    m.a = (1ll << 45) - 1;
    execute(&output, m, prog.data, prog.size, false, NULL, 0);
    assert(output.size < prog.size);
    output.size = 0;
    m.a = (1ll << 45);
    execute(&output, m, prog.data, prog.size, false, NULL, 0);
    assert(output.size == prog.size);

    output.size = 0;
    m.a = (1ll << 48);
    execute(&output, m, prog.data, prog.size, false, NULL, 0);
    assert(output.size == 1 + prog.size);

    output.size = 0;
    m.a = (1ll << 48) - 1;
    execute(&output, m, prog.data, prog.size, false, NULL, 0);
    assert(output.size == prog.size);
    const size_t LOW = 1ll << 45;
    const size_t HIGH = 1ll << 48;
    (void) HIGH;
    (void) LOW;
#endif

    struct p_sol initial = {
        .bit = 0,
        .value = 0,
    };
    struct partial_solution_list solutions = { 0 };
    da_append(solutions, initial);

    // uint64_t smallest_a = -1;
    uint64_t task2_sol = -1;
    while(solutions.size > 0) {
        const struct p_sol sol = solutions.data[--solutions.size];
        printf("%d: %lu\n", sol.bit, sol.value);
        m.pc = 0;
        m.a = sol.value;
        m.b = 0;
        m.c = 0;

        da_clear(output);
        execute(&output, m, prog.data, prog.size, false, NULL, 0);

        if(output.size > prog.size)
            continue;

        int err_count = 0;
        for(size_t i = 0; i < output.size; ++i) {
            if(prog.data[i] != output.data[i]) {
                err_count += 1;
            }
        }
        if(err_count == 0 && output.size == prog.size) {
            if(task2_sol > sol.value)
                task2_sol = sol.value;
        }
        if(err_count > 3)
            continue;
        
        
        // branch
        struct p_sol n1 = {
            .bit = sol.bit + 1,
            .value = sol.value | (1lu << sol.bit),
        };
        struct p_sol n0 = {
            .bit = sol.bit + 1,
            .value = sol.value,
        };

        if(n1.bit < 64) {
            da_append(solutions,n1);
        }

        if(n0.bit < 64) {
            da_append(solutions,n0);
        }
    }
    printf("Task2: %lu\n", task2_sol);

    free(prog.data);
    return 0;
}

