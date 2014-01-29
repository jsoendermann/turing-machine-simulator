#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#define TAPE_LENGTH 40

typedef struct state {
    int id;
    bool is_accept;
    bool is_reject;
} state;

state *make_state(int id, bool is_accept, bool is_reject) {
    state *s = (state*)malloc(sizeof(state));
    s->id = id;
    s->is_accept = is_accept;
    s->is_reject = is_reject;
    return s;
}

typedef struct transition {
    int from;
    char read;
    int to;
    char write;
    int direction; // -1: Left, 1: Right
} transition;

transition *make_transition(int from, char read, int to, char write, int direction) {
    transition *t = (transition*)malloc(sizeof(transition));
    t->from = from;
    t->read = read;
    t->to = to;
    t->write = write;
    t->direction = direction;
    return t;
}


// TODO turn these into lists
state *states[10];
int num_states = 0;
transition *transitions[50];
int num_transitions = 0;

// FIXME this function skips newlines. maybe there's a better way to do this
char get_non_whitespace_char() {
    char c;
    do {
        c = getchar();
    } while (isspace(c));
    return c;
}

int getint() {
    char s[2];
    do {
        s[0] = getchar();
    } while (isspace(s[0]));

    return atoi(s);
}


void make_states() {
    do {
        bool is_accept = false, is_reject = false;
        printf("State q%d has been added to the set of states. Is this an accepting state? [y|n] ", num_states);
        if (get_non_whitespace_char() == 'y') 
            is_accept = true;
        else {
            printf("Is state q%d a rejecting state? [y|n] ", num_states);
            if (get_non_whitespace_char() == 'y')
                is_reject = true;
        }
        states[num_states] = make_state(num_states, is_accept, is_reject);
        num_states++;

        printf("Would you like to create another state? [y|n] ");
    } while (get_non_whitespace_char() == 'y');
}

void make_transitions() {
    do {
        printf("For your %dth transition:\n", num_transitions);
        printf("From which state does the transition start? Please input the number of the state, e.g. '0' for state q0:\n");
        int from = getint();
        printf("Which character does the transition read?\n");
        char read = get_non_whitespace_char();
        printf("To which state does the transition go? Please input the number of the state, e.g. '0' for state q0:\n");
        int to = getint();
        printf("Which character does the transition write?\n");
        char write = get_non_whitespace_char();
        printf("Does the head move to the left or to the right? [l|r] ");
        char d = get_non_whitespace_char();
        transitions[num_transitions++] = make_transition(from, read, to, write, (d=='l'?-1:1));
        printf("Done! Do you want to create another transition? [y|n] ");
    } while (get_non_whitespace_char() == 'y');
}

void print_states_and_transitions() {
    printf("States:\n");
    for (int i = 0; i < num_states; i++)
        printf("q%d ", i);
    printf("\nAccepting states:\n");
    for (int i = 0; i < num_states; i++)
        if (states[i]->is_accept)
            printf("q%d ", i);
    printf("\nRejecting states:\n");
    for (int i = 0; i < num_states; i++)
        if (states[i]->is_reject)
            printf("q%d ", i);
    printf("\n\n");
    
    printf("Transitions:\n");
    for (int i = 0; i < num_transitions; i++) {
        transition t = *transitions[i];
        printf("From: q%d, Read: %c, To: q%d, Write: %c, Direction: %c\n", t.from, t.read, t.to, t.write, (t.direction==-1?'l':'r'));
    }
}

void print_machine_configuration(char *tape, int head) {
    for (int i = 0; i < TAPE_LENGTH; i++) {
        printf("%c ", tape[i]);
    }
    printf("\n");
    for (int i = 0; i < head; i++)
        printf("  ");
    printf("^\n");

}

state *get_state_from_id(int id) {
    for (int i = 0; i < num_states; i++)
        if (states[i]->id == id)
            return states[i];
    return NULL;
}

void simulate_machine(char *input) {
    char tape[TAPE_LENGTH];
    for (int i = 0; i < TAPE_LENGTH; i++)
        tape[i] = '-';
    int head = TAPE_LENGTH/3;
    for (int i = 0; input[i] != '\0'; i++)
        tape[head+i] = input[i];

    print_machine_configuration(tape, head);

    state *current_state = states[0];

    while (!(current_state->is_accept || current_state->is_reject)) {
        char c = tape[head];

        transition *matching_transition = NULL;
        for (int i = 0; i < num_transitions; i++) {
            if (transitions[i]->from == current_state->id && transitions[i]->read == c) {
                matching_transition = transitions[i];
                break;
            }
        }
        if (matching_transition == NULL) {
            printf("Failure, no matching transition found\n");
            exit(-1);
        }

        printf("State change: q%d -> q%d\n", current_state->id, matching_transition->to);

        current_state = get_state_from_id(matching_transition->to);
        tape[head] = matching_transition->write;
        head += matching_transition->direction;

        print_machine_configuration(tape, head);
    }
    if (current_state->is_accept)
        printf("Success, q%d is an accepting state!\n", current_state->id);
    if (current_state->is_reject)
        printf("Failure, q%d is a rejecting state!\n", current_state->id);
}

void make_example_states_and_transitions() {
    states[num_states++] = make_state(0, false, false);
    states[num_states++] = make_state(1, false, false);
    states[num_states++] = make_state(2, false, false);
    states[num_states++] = make_state(3, false, false);
    states[num_states++] = make_state(4, false, false);
    states[num_states++] = make_state(5, false, true);
    states[num_states++] = make_state(6, true, false);

    transitions[num_transitions++] = make_transition(0, '1', 1, '-', 1);
    transitions[num_transitions++] = make_transition(0, '-', 5, '-', 1);
    transitions[num_transitions++] = make_transition(0, '.', 5, '.', 1);
    transitions[num_transitions++] = make_transition(1, '.', 1, '.', 1);
    transitions[num_transitions++] = make_transition(1, '-', 6, '-', 1);
    transitions[num_transitions++] = make_transition(1, '1', 2, '.', 1);
    transitions[num_transitions++] = make_transition(2, '.', 2, '.', 1);
    transitions[num_transitions++] = make_transition(2, '1', 3, '1', 1);
    transitions[num_transitions++] = make_transition(2, '-', 4, '-', -1);
    transitions[num_transitions++] = make_transition(4, '1', 4, '1', -1);
    transitions[num_transitions++] = make_transition(4, '.', 4, '.', -1);
    transitions[num_transitions++] = make_transition(4, '-', 1, '-', 1);
    transitions[num_transitions++] = make_transition(3, '1', 2, '.', 1);
    transitions[num_transitions++] = make_transition(3, '.', 3, '.', 1);
    transitions[num_transitions++] = make_transition(3, '-', 5, '-', 1);
}

int main(int argc, char **argv) {
    char *input = (char*)malloc(sizeof(char)*50);

    printf("Welcome to Jan Soendermann's Turing Machine simulator!\n\n");

    printf("Would you like to try the 1^2^n example from the book? [y|n] ");
    if (get_non_whitespace_char() == 'y')
        make_example_states_and_transitions();
    else {
        states[0] = make_state(num_states++, false, false);
        printf("A starting state q0 has been created for you\n");

        printf("Would you like to create additional states? [y|n] ");
        if (get_non_whitespace_char() == 'y')
            make_states();

        printf("The following states have been created:\n");
        for (int i = 0; i < num_states; i++)
            printf("q%d ", i);
        printf("\n\n");

        printf("Next, let's create some transitions between your states.\n");
        make_transitions();
        printf("\n");
    }

    printf("The following states and transitions have been created:\n");
    print_states_and_transitions();
    printf("\n");

    printf("Please enter the input string\n");
    scanf("%s", input);

    simulate_machine(input);
}
