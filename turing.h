#define TAPE_LENGTH 40

typedef struct state {
    int id;
    bool is_accept;
    bool is_reject;
} state;

typedef struct transition {
    int from;
    char read;
    int to;
    char write;
    int direction; // -1: Left, 1: Right
} transition;

state *make_state(int id, bool is_accept, bool is_reject);
transition *make_transition(int from, char read, int to, char write, int direction);
char get_non_whitespace_char();
int getint();
void make_states();
void make_transitions();
void print_states_and_transitions();
void print_machine_configuration(char *tape, int head);
state *get_state_from_id(int id);
void simulate_machine(char *input);
void make_example_states_and_transitions();
int main(int argc, char **argv);
