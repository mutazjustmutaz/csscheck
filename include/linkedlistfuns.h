struct elt {
  char *value;
  struct elt *next;
};

typedef struct elt *stack;

void stack_push(stack *s, char *str);

char *stack_pop(stack *s);

void stack_process(stack *s);
