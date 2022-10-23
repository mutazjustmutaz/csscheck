
typedef struct color *stack;

void stack_push(stack *s, char *clr);

char *stack_pop(stack *s);

int stack_empty(const stack *s);

void stack_process(stack *s);

regex_t *create_regexes(void);

void process_colors(char *csspath, regex_t *regexarr);

void destroy_regexes(regex_t *regexarr);

void start_element_tags(void *user_data, const xmlChar *name, const xmlChar **attrs);

void style_tag_paths(char **filesarr, const int farrlen);

void start_element_att(void *user_data, const xmlChar *name, const xmlChar **attrs);

void style_att_paths(char **filesarr, const int farrlen);
