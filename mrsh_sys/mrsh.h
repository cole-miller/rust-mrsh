#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

struct mrsh_array {
	void **data;
	size_t len, cap;
};

bool mrsh_array_reserve(struct mrsh_array *array, size_t size);
ssize_t mrsh_array_add(struct mrsh_array *array, void *value);
void mrsh_array_finish(struct mrsh_array *array);

enum mrsh_arithm_expr_type {
	MRSH_ARITHM_LITERAL,
	MRSH_ARITHM_VARIABLE,
	MRSH_ARITHM_UNOP,
	MRSH_ARITHM_BINOP,
	MRSH_ARITHM_COND,
	MRSH_ARITHM_ASSIGN,
};

/**
 * An aritmetic expression. One of:
 * - A literal
 * - A variable
 * - An unary operation
 * - A binary operation
 * - A condition
 * - An assignment
 */
struct mrsh_arithm_expr {
	enum mrsh_arithm_expr_type type;
};

struct mrsh_arithm_literal {
	struct mrsh_arithm_expr expr;
	long value;
};

struct mrsh_arithm_variable {
	struct mrsh_arithm_expr expr;
	char *name;
};

enum mrsh_arithm_unop_type {
	MRSH_ARITHM_UNOP_PLUS,
	MRSH_ARITHM_UNOP_MINUS,
	MRSH_ARITHM_UNOP_TILDE,
	MRSH_ARITHM_UNOP_BANG,
};

struct mrsh_arithm_unop {
	struct mrsh_arithm_expr expr;
	enum mrsh_arithm_unop_type type;
	struct mrsh_arithm_expr *body;
};

enum mrsh_arithm_binop_type {
	MRSH_ARITHM_BINOP_ASTERISK,
	MRSH_ARITHM_BINOP_SLASH,
	MRSH_ARITHM_BINOP_PERCENT,
	MRSH_ARITHM_BINOP_PLUS,
	MRSH_ARITHM_BINOP_MINUS,
	MRSH_ARITHM_BINOP_DLESS,
	MRSH_ARITHM_BINOP_DGREAT,
	MRSH_ARITHM_BINOP_LESS,
	MRSH_ARITHM_BINOP_LESSEQ,
	MRSH_ARITHM_BINOP_GREAT,
	MRSH_ARITHM_BINOP_GREATEQ,
	MRSH_ARITHM_BINOP_DEQ,
	MRSH_ARITHM_BINOP_BANGEQ,
	MRSH_ARITHM_BINOP_AND,
	MRSH_ARITHM_BINOP_CIRC,
	MRSH_ARITHM_BINOP_OR,
	MRSH_ARITHM_BINOP_DAND,
	MRSH_ARITHM_BINOP_DOR,
};

struct mrsh_arithm_binop {
	struct mrsh_arithm_expr expr;
	enum mrsh_arithm_binop_type type;
	struct mrsh_arithm_expr *left, *right;
};

struct mrsh_arithm_cond {
	struct mrsh_arithm_expr expr;
	struct mrsh_arithm_expr *condition, *body, *else_part;
};

enum mrsh_arithm_assign_op {
	MRSH_ARITHM_ASSIGN_NONE,
	MRSH_ARITHM_ASSIGN_ASTERISK,
	MRSH_ARITHM_ASSIGN_SLASH,
	MRSH_ARITHM_ASSIGN_PERCENT,
	MRSH_ARITHM_ASSIGN_PLUS,
	MRSH_ARITHM_ASSIGN_MINUS,
	MRSH_ARITHM_ASSIGN_DLESS,
	MRSH_ARITHM_ASSIGN_DGREAT,
	MRSH_ARITHM_ASSIGN_AND,
	MRSH_ARITHM_ASSIGN_CIRC,
	MRSH_ARITHM_ASSIGN_OR,
};

struct mrsh_arithm_assign {
	struct mrsh_arithm_expr expr;
	enum mrsh_arithm_assign_op op;
	char *name;
	struct mrsh_arithm_expr *value;
};

void mrsh_arithm_expr_destroy(struct mrsh_arithm_expr *expr);
struct mrsh_arithm_literal *mrsh_arithm_literal_create(long value);
struct mrsh_arithm_variable *mrsh_arithm_variable_create(char *name);
struct mrsh_arithm_unop *mrsh_arithm_unop_create(
	enum mrsh_arithm_unop_type type, struct mrsh_arithm_expr *body);
struct mrsh_arithm_binop *mrsh_arithm_binop_create(
	enum mrsh_arithm_binop_type type, struct mrsh_arithm_expr *left,
	struct mrsh_arithm_expr *right);
struct mrsh_arithm_cond *mrsh_arithm_cond_create(
	struct mrsh_arithm_expr *condition, struct mrsh_arithm_expr *body,
	struct mrsh_arithm_expr *else_part);
struct mrsh_arithm_assign *mrsh_arithm_assign_create(
	enum mrsh_arithm_assign_op op, char *name,
	struct mrsh_arithm_expr *value);
struct mrsh_arithm_literal *mrsh_arithm_expr_get_literal(
	const struct mrsh_arithm_expr *expr);
struct mrsh_arithm_variable *mrsh_arithm_expr_get_variable(
	const struct mrsh_arithm_expr *expr);
struct mrsh_arithm_unop *mrsh_arithm_expr_get_unop(
	const struct mrsh_arithm_expr *expr);
struct mrsh_arithm_binop *mrsh_arithm_expr_get_binop(
	const struct mrsh_arithm_expr *expr);
struct mrsh_arithm_cond *mrsh_arithm_expr_get_cond(
	const struct mrsh_arithm_expr *expr);
struct mrsh_arithm_assign *mrsh_arithm_expr_get_assign(
	const struct mrsh_arithm_expr *expr);

#include <stdbool.h>
#include <stddef.h>

struct mrsh_buffer {
	char *data;
	size_t len, cap;
};

/**
 * Makes sure at least `size` bytes can be written to the buffer, without
 * increasing its length. Returns a pointer to the end of the buffer, or NULL if
 * resizing fails. Callers are responsible for manually increasing `buf->len`.
 *
 * This function is useful when e.g. reading from a file. Example with error
 * handling left out:
 *
 *   char *dst = mrsh_buffer_reserve(buf, READ_SIZE);
 *   ssize_t n = read(fd, dst, READ_SIZE);
 *   buf->len += n;
 */
char *mrsh_buffer_reserve(struct mrsh_buffer *buf, size_t size);
/**
 * Increases the length of the buffer by `size` bytes. Returns a pointer to the
 * beginning of the newly appended space, or NULL if resizing fails.
 */
char *mrsh_buffer_add(struct mrsh_buffer *buf, size_t size);
bool mrsh_buffer_append(struct mrsh_buffer *buf, const char *data, size_t size);
bool mrsh_buffer_append_char(struct mrsh_buffer *buf, char c);
/**
 * Get the buffer's current data and reset it.
 */
char *mrsh_buffer_steal(struct mrsh_buffer *buf);
void mrsh_buffer_finish(struct mrsh_buffer *buf);

#define MRSH_HASHTABLE_BUCKETS 256

struct mrsh_hashtable_entry {
	struct mrsh_hashtable_entry *next;
	unsigned int hash;
	char *key;
	void *value;
};

struct mrsh_hashtable {
	struct mrsh_hashtable_entry *buckets[MRSH_HASHTABLE_BUCKETS];
};

typedef void (*mrsh_hashtable_iterator_func)(const char *key, void *value,
	void *user_data);

void mrsh_hashtable_finish(struct mrsh_hashtable *table);
void *mrsh_hashtable_get(struct mrsh_hashtable *table, const char *key);
void *mrsh_hashtable_set(struct mrsh_hashtable *table, const char *key,
	void *value);
void *mrsh_hashtable_del(struct mrsh_hashtable *table, const char *key);
/**
 * Calls `iterator` for each (key, value) pair in the hash table. It is safe to
 * call `mrsh_hashtable_del` on the current element, however it is not safe to
 * do so an any other element.
 */
void mrsh_hashtable_for_each(struct mrsh_hashtable *table,
	mrsh_hashtable_iterator_func iterator, void *user_data);

#include <stdbool.h>

/**
 * Position describes an arbitrary source position including line and column
 * location.
 */
struct mrsh_position {
	size_t offset; // starting at 0
	int line; // starting at 1
	int column; // starting at 1
};

/**
 * Range describes a continuous source region. It has a beginning position and
 * a non-included ending position.
 */
struct mrsh_range {
	struct mrsh_position begin, end;
};

enum mrsh_node_type {
	MRSH_NODE_PROGRAM,
	MRSH_NODE_COMMAND_LIST,
	MRSH_NODE_AND_OR_LIST,
	MRSH_NODE_COMMAND,
	MRSH_NODE_WORD,
};

struct mrsh_node {
	enum mrsh_node_type type;
};

enum mrsh_word_type {
	MRSH_WORD_STRING,
	MRSH_WORD_PARAMETER,
	MRSH_WORD_COMMAND,
	MRSH_WORD_ARITHMETIC,
	MRSH_WORD_LIST,
};

/**
 * A word can be:
 * - An unquoted or a single-quoted string
 * - A candidate for parameter expansion
 * - A candidate for command substitution
 * - A candidate for arithmetic expansion
 * - An unquoted or a double-quoted list of words
 */
struct mrsh_word {
	struct mrsh_node node;
	enum mrsh_word_type type;
};

/**
 * A string word is a type of word. It can be unquoted or single-quoted.
 */
struct mrsh_word_string {
	struct mrsh_word word;
	char *str;
	bool single_quoted;

	// true if candidate for field splitting (ie. result of parameter
	// expansion, command substitution or arithmetic expansion)
	bool split_fields;

	struct mrsh_range range;
};

enum mrsh_word_parameter_op {
	MRSH_PARAM_NONE, // `$name` or `${parameter}`, no-op
	MRSH_PARAM_MINUS, // `${parameter:-[word]}`, Use Default Values
	MRSH_PARAM_EQUAL, // `${parameter:=[word]}`, Assign Default Values
	MRSH_PARAM_QMARK, // `${parameter:?[word]}`, Indicate Error if Null or Unset
	MRSH_PARAM_PLUS, // `${parameter:+[word]}`, Use Alternative Value
	MRSH_PARAM_LEADING_HASH, // `${#parameter}`, String Length
	MRSH_PARAM_PERCENT, // `${parameter%[word]}`, Remove Smallest Suffix Pattern
	MRSH_PARAM_DPERCENT, // `${parameter%%[word]}`, Remove Largest Suffix Pattern
	MRSH_PARAM_HASH, // `${parameter#[word]}`, Remove Smallest Prefix Pattern
	MRSH_PARAM_DHASH, // `${parameter##[word]}`, Remove Largest Prefix Pattern
};

/**
 * A word parameter is a type of word candidate for parameter expansion. The
 * format is either `$name` or `${expression}`.
 */
struct mrsh_word_parameter {
	struct mrsh_word word;
	char *name;
	enum mrsh_word_parameter_op op;
	bool colon; // only for -, =, ?, +
	struct mrsh_word *arg; // can be NULL

	struct mrsh_position dollar_pos;
	struct mrsh_range name_range;
	struct mrsh_range op_range; // can be invalid
	struct mrsh_position lbrace_pos, rbrace_pos; // can be invalid
};

/**
 * A word command is a type of word candidate for command substitution. The
 * format is either `` `command` `` or `$(command)`.
 */
struct mrsh_word_command {
	struct mrsh_word word;
	struct mrsh_program *program; // can be NULL
	bool back_quoted;

	struct mrsh_range range;
};

/**
 * An arithmetic word is a type of word containing an arithmetic expression. The
 * format is `$((expression))`.
 */
struct mrsh_word_arithmetic {
	struct mrsh_word word;
	struct mrsh_word *body;
};

/**
 * A word list is a type of word. It can be unquoted or double-quoted. Its
 * children are _not_ separated by blanks. Here's an example:
 *
 *   abc"d ef"g'h i'
 */
struct mrsh_word_list {
	struct mrsh_word word;
	struct mrsh_array children; // struct mrsh_word *
	bool double_quoted;

	struct mrsh_position lquote_pos, rquote_pos; // can be invalid
};

enum mrsh_io_redirect_op {
	MRSH_IO_LESS, // <
	MRSH_IO_GREAT, // >
	MRSH_IO_CLOBBER, // >|
	MRSH_IO_DGREAT, // >>
	MRSH_IO_LESSAND, // <&
	MRSH_IO_GREATAND, // >&
	MRSH_IO_LESSGREAT, // <>
	MRSH_IO_DLESS, // <<
	MRSH_IO_DLESSDASH, // <<-
};

/**
 * An IO redirection operator. The format is: `[io_number]op name`.
 */
struct mrsh_io_redirect {
	int io_number; // -1 if unspecified
	enum mrsh_io_redirect_op op;
	struct mrsh_word *name; // filename or here-document delimiter
	struct mrsh_array here_document; // struct mrsh_word *, only for << and <<-

	struct mrsh_position io_number_pos; // can be invalid
	struct mrsh_range op_range;
};

/**
 * A variable assignment. The format is: `name=value`.
 */
struct mrsh_assignment {
	char *name;
	struct mrsh_word *value;

	struct mrsh_range name_range;
	struct mrsh_position equal_pos;
};

enum mrsh_command_type {
	MRSH_SIMPLE_COMMAND,
	MRSH_BRACE_GROUP,
	MRSH_SUBSHELL,
	MRSH_IF_CLAUSE,
	MRSH_FOR_CLAUSE,
	MRSH_LOOP_CLAUSE, // `while` or `until`
	MRSH_CASE_CLAUSE,
	MRSH_FUNCTION_DEFINITION,
};

/**
 * A command. It is either a simple command, a brace group or an if clause.
 */
struct mrsh_command {
	struct mrsh_node node;
	enum mrsh_command_type type;
};

/**
 * A simple command is a type of command. It contains a command name, followed
 * by command arguments. It can also contain IO redirections and variable
 * assignments.
 */
struct mrsh_simple_command {
	struct mrsh_command command;
	struct mrsh_word *name; // can be NULL if it contains only assignments
	struct mrsh_array arguments; // struct mrsh_word *
	struct mrsh_array io_redirects; // struct mrsh_io_redirect *
	struct mrsh_array assignments; // struct mrsh_assignment *
};

/**
 * A brace group is a type of command. It contains command lists and executes
 * them in the current process environment. The format is:
 * `{ compound-list ; }`.
 */
struct mrsh_brace_group {
	struct mrsh_command command;
	struct mrsh_array body; // struct mrsh_command_list *

	struct mrsh_position lbrace_pos, rbrace_pos;
};

/**
 * A subshell is a type of command. It contains command lists and executes
 * them in a subshell environment. The format is: `( compound-list )`.
 */
struct mrsh_subshell {
	struct mrsh_command command;
	struct mrsh_array body; // struct mrsh_command_list *

	struct mrsh_position lparen_pos, rparen_pos;
};

/**
 * An if clause is a type of command. The format is:
 *
 *   if compound-list
 *   then
 *       compound-list
 *   [elif compound-list
 *   then
 *       compound-list] ...
 *   [else
 *       compound-list]
 *   fi
 */
struct mrsh_if_clause {
	struct mrsh_command command;
	struct mrsh_array condition; // struct mrsh_command_list *
	struct mrsh_array body; // struct mrsh_command_list *
	struct mrsh_command *else_part; // can be NULL

	struct mrsh_range if_range; // for `if` or `elif`
	struct mrsh_range then_range, fi_range;
	struct mrsh_range else_range; // can be invalid
};

/**
 * A for clause is a type of command. The format is:
 *
 *   for name [ in [word ... ]]
 *   do
 *       compound-list
 *   done
 */
struct mrsh_for_clause {
	struct mrsh_command command;
	char *name;
	bool in;
	struct mrsh_array word_list; // struct mrsh_word *
	struct mrsh_array body; // struct mrsh_command_list *

	struct mrsh_range for_range, name_range, do_range, done_range;
	struct mrsh_range in_range; // can be invalid
};

enum mrsh_loop_type {
	MRSH_LOOP_WHILE,
	MRSH_LOOP_UNTIL,
};

/**
 * A loop clause is a type of command. The format is:
 *
 *   while/until compound-list-1
 *   do
 *       compound-list-2
 *   done
 */
struct mrsh_loop_clause {
	struct mrsh_command command;
	enum mrsh_loop_type type;
	struct mrsh_array condition; // struct mrsh_command_list *
	struct mrsh_array body; // struct mrsh_command_list *

	struct mrsh_range while_until_range; // for `while` or `until`
	struct mrsh_range do_range, done_range;
};

/**
 * A case item contains one or more patterns with a body. The format is:
 *
 *   [(] pattern[ | pattern] ... ) compound-list ;;
 *
 * The double-semicolumn is optional if it's the last item.
 */
struct mrsh_case_item {
	struct mrsh_array patterns; // struct mrsh_word *
	struct mrsh_array body; // struct mrsh_command_list *

	struct mrsh_position lparen_pos; // can be invalid
	// TODO: pipe positions between each pattern
	struct mrsh_position rparen_pos;
	struct mrsh_range dsemi_range; // can be invalid
};

/**
 * A case clause is a type of command. The format is:
 *
 *   case word in
 *       [(] pattern1 ) compound-list ;;
 *       [[(] pattern[ | pattern] ... ) compound-list ;;] ...
 *       [[(] pattern[ | pattern] ... ) compound-list]
 *   esac
 */
struct mrsh_case_clause {
	struct mrsh_command command;
	struct mrsh_word *word;
	struct mrsh_array items; // struct mrsh_case_item *

	struct mrsh_range case_range, in_range, esac_range;
};

/**
 * A function definition is a type of command. The format is:
 *
 *   fname ( ) compound-command [io-redirect ...]
 */
struct mrsh_function_definition {
	struct mrsh_command command;
	char *name;
	struct mrsh_command *body;
	struct mrsh_array io_redirects; // struct mrsh_io_redirect *

	struct mrsh_range name_range;
	struct mrsh_position lparen_pos, rparen_pos;
};

enum mrsh_and_or_list_type {
	MRSH_AND_OR_LIST_PIPELINE,
	MRSH_AND_OR_LIST_BINOP,
};

/**
 * An AND-OR list is a tree of pipelines and binary operations.
 */
struct mrsh_and_or_list {
	struct mrsh_node node;
	enum mrsh_and_or_list_type type;
};

/**
 * A pipeline is a type of AND-OR list which consists of multiple commands
 * separated by `|`. The format is: `[!] command1 [ | command2 ...]`.
 */
struct mrsh_pipeline {
	struct mrsh_and_or_list and_or_list;
	struct mrsh_array commands; // struct mrsh_command *
	bool bang; // whether the pipeline begins with `!`

	struct mrsh_position bang_pos; // can be invalid
	// TODO: pipe positions between each command
};

enum mrsh_binop_type {
	MRSH_BINOP_AND, // `&&`
	MRSH_BINOP_OR, // `||`
};

/**
 * A binary operation is a type of AND-OR list which consists of multiple
 * pipelines separated by `&&` or `||`.
 */
struct mrsh_binop {
	struct mrsh_and_or_list and_or_list;
	enum mrsh_binop_type type;
	struct mrsh_and_or_list *left, *right;

	struct mrsh_range op_range;
};

/**
 * A command list contains AND-OR lists separated by `;` (for sequential
 * execution) or `&` (for asynchronous execution).
 */
struct mrsh_command_list {
	struct mrsh_node node;
	struct mrsh_and_or_list *and_or_list;
	bool ampersand; // whether the command list ends with `&`

	struct mrsh_position separator_pos; // can be invalid
};

/**
 * A shell program. It contains command lists.
 */
struct mrsh_program {
	struct mrsh_node node;
	struct mrsh_array body; // struct mrsh_command_list *
};

typedef void (*mrsh_node_iterator_func)(struct mrsh_node *node,
	void *user_data);

bool mrsh_position_valid(const struct mrsh_position *pos);
bool mrsh_range_valid(const struct mrsh_range *range);

struct mrsh_word_string *mrsh_word_string_create(char *str,
	bool single_quoted);
struct mrsh_word_parameter *mrsh_word_parameter_create(char *name,
	enum mrsh_word_parameter_op op, bool colon, struct mrsh_word *arg);
struct mrsh_word_command *mrsh_word_command_create(struct mrsh_program *prog,
	bool back_quoted);
struct mrsh_word_arithmetic *mrsh_word_arithmetic_create(
	struct mrsh_word *body);
struct mrsh_word_list *mrsh_word_list_create(struct mrsh_array *children,
	bool double_quoted);
struct mrsh_simple_command *mrsh_simple_command_create(struct mrsh_word *name,
	struct mrsh_array *arguments, struct mrsh_array *io_redirects,
	struct mrsh_array *assignments);
struct mrsh_brace_group *mrsh_brace_group_create(struct mrsh_array *body);
struct mrsh_subshell *mrsh_subshell_create(struct mrsh_array *body);
struct mrsh_if_clause *mrsh_if_clause_create(struct mrsh_array *condition,
	struct mrsh_array *body, struct mrsh_command *else_part);
struct mrsh_for_clause *mrsh_for_clause_create(char *name, bool in,
	struct mrsh_array *word_list, struct mrsh_array *body);
struct mrsh_loop_clause *mrsh_loop_clause_create(enum mrsh_loop_type type,
	struct mrsh_array *condition, struct mrsh_array *body);
struct mrsh_case_clause *mrsh_case_clause_create(struct mrsh_word *word,
	struct mrsh_array *items);
struct mrsh_function_definition *mrsh_function_definition_create(char *name,
	struct mrsh_command *body, struct mrsh_array *io_redirects);
struct mrsh_pipeline *mrsh_pipeline_create(struct mrsh_array *commands,
	bool bang);
struct mrsh_binop *mrsh_binop_create(enum mrsh_binop_type type,
	struct mrsh_and_or_list *left, struct mrsh_and_or_list *right);
struct mrsh_command_list *mrsh_command_list_create(void);
struct mrsh_program *mrsh_program_create(void);

void mrsh_node_destroy(struct mrsh_node *node);
void mrsh_word_destroy(struct mrsh_word *word);
void mrsh_io_redirect_destroy(struct mrsh_io_redirect *redir);
void mrsh_assignment_destroy(struct mrsh_assignment *assign);
void mrsh_command_destroy(struct mrsh_command *cmd);
void mrsh_and_or_list_destroy(struct mrsh_and_or_list *and_or_list);
void mrsh_command_list_destroy(struct mrsh_command_list *l);
void mrsh_program_destroy(struct mrsh_program *prog);

struct mrsh_word *mrsh_node_get_word(const struct mrsh_node *node);
struct mrsh_command *mrsh_node_get_command(const struct mrsh_node *node);
struct mrsh_and_or_list *mrsh_node_get_and_or_list(
	const struct mrsh_node *node);
struct mrsh_command_list *mrsh_node_get_command_list(
	const struct mrsh_node *node);
struct mrsh_program *mrsh_node_get_program(const struct mrsh_node *node);

struct mrsh_word_string *mrsh_word_get_string(const struct mrsh_word *word);
struct mrsh_word_parameter *mrsh_word_get_parameter(
	const struct mrsh_word *word);
struct mrsh_word_command *mrsh_word_get_command(const struct mrsh_word *word);
struct mrsh_word_arithmetic *mrsh_word_get_arithmetic(
	const struct mrsh_word *word);
struct mrsh_word_list *mrsh_word_get_list(const struct mrsh_word *word);

struct mrsh_simple_command *mrsh_command_get_simple_command(
	const struct mrsh_command *cmd);
struct mrsh_brace_group *mrsh_command_get_brace_group(
	const struct mrsh_command *cmd);
struct mrsh_subshell *mrsh_command_get_subshell(
	const struct mrsh_command *cmd);
struct mrsh_if_clause *mrsh_command_get_if_clause(
	const struct mrsh_command *cmd);
struct mrsh_for_clause *mrsh_command_get_for_clause(
	const struct mrsh_command *cmd);
struct mrsh_loop_clause *mrsh_command_get_loop_clause(
	const struct mrsh_command *cmd);
struct mrsh_case_clause *mrsh_command_get_case_clause(
	const struct mrsh_command *cmd);
struct mrsh_function_definition *mrsh_command_get_function_definition(
	const struct mrsh_command *cmd);

struct mrsh_pipeline *mrsh_and_or_list_get_pipeline(
	const struct mrsh_and_or_list *and_or_list);
struct mrsh_binop *mrsh_and_or_list_get_binop(
	const struct mrsh_and_or_list *and_or_list);

void mrsh_node_for_each(struct mrsh_node *node,
	mrsh_node_iterator_func iterator, void *user_data);

void mrsh_word_range(struct mrsh_word *word, struct mrsh_position *begin,
	struct mrsh_position *end);
void mrsh_command_range(struct mrsh_command *cmd, struct mrsh_position *begin,
	struct mrsh_position *end);
char *mrsh_word_str(const struct mrsh_word *word);
char *mrsh_node_format(struct mrsh_node *node);
void mrsh_program_print(struct mrsh_program *prog);

struct mrsh_node *mrsh_node_copy(const struct mrsh_node *node);
struct mrsh_word *mrsh_word_copy(const struct mrsh_word *word);
struct mrsh_io_redirect *mrsh_io_redirect_copy(
	const struct mrsh_io_redirect *redir);
struct mrsh_assignment *mrsh_assignment_copy(
	const struct mrsh_assignment *assign);
struct mrsh_command *mrsh_command_copy(const struct mrsh_command *cmd);
struct mrsh_and_or_list *mrsh_and_or_list_copy(
	const struct mrsh_and_or_list *and_or_list);
struct mrsh_command_list *mrsh_command_list_copy(
	const struct mrsh_command_list *l);
struct mrsh_program *mrsh_program_copy(const struct mrsh_program *prog);

#include <stdio.h>

struct mrsh_parser;
struct mrsh_buffer;

/**
 * An alias callback. The alias named is given as a parameter and the alias
 * value should be returned. NULL should be returned if the alias doesn't exist.
 */
typedef const char *(*mrsh_parser_alias_func)(const char *name,
	void *user_data);

/**
 * Create a parser from a file descriptor.
 */
struct mrsh_parser *mrsh_parser_with_fd(int fd);
/**
 * Create a parser from a static buffer.
 */
struct mrsh_parser *mrsh_parser_with_data(const char *buf, size_t len);
/**
 * Create a parser with a shared buffer. Data will be read from `buf` each time
 * the parser needs input data.
 */
struct mrsh_parser *mrsh_parser_with_buffer(struct mrsh_buffer *buf);
void mrsh_parser_destroy(struct mrsh_parser *parser);
/**
 * Parse a complete multi-line program.
 */
struct mrsh_program *mrsh_parse_program(struct mrsh_parser *parser);
/**
 * Parse a program line. Continuation lines are consumed.
 */
struct mrsh_program *mrsh_parse_line(struct mrsh_parser *parser);

/**
 * Parse an arithmetic expression.
 */
struct mrsh_arithm_expr *mrsh_parse_arithm_expr(struct mrsh_parser *parser);
/**
 * Check if the input has been completely consumed.
 */
bool mrsh_parser_eof(struct mrsh_parser *parser);
/**
 * Set the alias callback.
 */
void mrsh_parser_set_alias_func(struct mrsh_parser *parser,
	mrsh_parser_alias_func alias, void *user_data);
/**
 * Check if the parser ended with a syntax error. The error message is returned.
 * The error position can optionally be obtained.
 */
const char *mrsh_parser_error(struct mrsh_parser *parser,
	struct mrsh_position *pos);
/**
 * Check if the input ends on a continuation line.
 */
bool mrsh_parser_continuation_line(struct mrsh_parser *parser);
/**
 * Reset the parser state.
 */
void mrsh_parser_reset(struct mrsh_parser *parser);

#include <stdint.h>
#include <stdio.h>

enum mrsh_option {
	// -a: When this option is on, the export attribute shall be set for each
	// variable to which an assignment is performed.
	MRSH_OPT_ALLEXPORT = 1 << 0,
	// -b: Shall cause the shell to notify the user asynchronously of background
	// job completions.
	MRSH_OPT_NOTIFY = 1 << 1,
	// -C: Prevent existing files from being overwritten by the shell's '>'
	// redirection operator; the ">|" redirection operator shall override this
	// noclobber option for an individual file.
	MRSH_OPT_NOCLOBBER = 1 << 2,
	// -e: When this option is on, when any command fails (for any of the
	// reasons listed in Consequences of Shell Errors or by returning an exit
	// status greater than zero), the shell immediately shall exit
	MRSH_OPT_ERREXIT = 1 << 3,
	// -f: The shell shall disable pathname expansion.
	MRSH_OPT_NOGLOB = 1 << 4,
	// -h: Locate and remember utilities invoked by functions as those functions
	// are defined (the utilities are normally located when the function is
	// executed).
	MRSH_OPT_PRELOOKUP = 1 << 5,
	// -m: All jobs shall be run in their own process groups. Immediately before
	// the shell issues a prompt after completion of the background job, a
	// message reporting the exit status of the background job shall be written
	// to standard error. If a foreground job stops, the shell shall write a
	// message to standard error to that effect, formatted as described by the
	// jobs utility.
	MRSH_OPT_MONITOR = 1 << 6,
	// -n: The shell shall read commands but does not execute them; this can be
	// used to check for shell script syntax errors. An interactive shell may
	// ignore this option.
	MRSH_OPT_NOEXEC = 1 << 7,
	// -o ignoreeof: Prevent an interactive shell from exiting on end-of-file.
	MRSH_OPT_IGNOREEOF = 1 << 8,
	// -o nolog: Prevent the entry of function definitions into the command
	// history
	MRSH_OPT_NOLOG = 1 << 9,
	// -o vi: Allow shell command line editing using the built-in vi editor.
	MRSH_OPT_VI = 1 << 10,
	// -u: When the shell tries to expand an unset parameter other than the '@'
	// and '*' special parameters, it shall write a message to standard error
	// and the expansion shall fail.
	MRSH_OPT_NOUNSET = 1 << 11,
	// -v: The shell shall write its input to standard error as it is read.
	MRSH_OPT_VERBOSE = 1 << 12,
	// -x: The shell shall write to standard error a trace for each command
	// after it expands the command and before it executes it.
	MRSH_OPT_XTRACE = 1 << 13,
};

enum mrsh_variable_attrib {
	MRSH_VAR_ATTRIB_NONE = 0,
	MRSH_VAR_ATTRIB_EXPORT = 1 << 0,
	MRSH_VAR_ATTRIB_READONLY = 1 << 1,
};

struct mrsh_call_frame {
	char **argv;
	int argc;
	struct mrsh_call_frame *prev;
};

struct mrsh_state {
	int exit;
	uint32_t options; // enum mrsh_option
	struct mrsh_call_frame *frame; // last call frame
	bool interactive;
	int last_status;
};

struct mrsh_parser;

struct mrsh_state *mrsh_state_create(void);
void mrsh_state_destroy(struct mrsh_state *state);
void mrsh_state_set_parser_alias_func(
		struct mrsh_state *state, struct mrsh_parser *parser);
void mrsh_env_set(struct mrsh_state *state,
	const char *key, const char *value, uint32_t attribs);
void mrsh_env_unset(struct mrsh_state *state, const char *key);
const char *mrsh_env_get(struct mrsh_state *state,
	const char *key, uint32_t *attribs);
int mrsh_run_program(struct mrsh_state *state, struct mrsh_program *prog);
int mrsh_run_word(struct mrsh_state *state, struct mrsh_word **word);
bool mrsh_run_arithm_expr(struct mrsh_state *state,
	struct mrsh_arithm_expr *expr, long *result);
/**
 * Enable or disable job control. This will setup signal handlers, process
 * groups and the terminal accordingly.
 */
bool mrsh_set_job_control(struct mrsh_state *state, bool enabled);
/**
 * Destroy terminated jobs and print job notifications. This function should be
 * called after mrsh_run_program.
 */
void mrsh_destroy_terminated_jobs(struct mrsh_state *state);

bool mrsh_has_builtin(const char *name);
bool mrsh_has_special_builtin(const char *name);
int mrsh_run_builtin(struct mrsh_state *state, int argc, char *argv[]);

struct mrsh_init_args {
	const char *command_file;
	const char *command_str;
};

int mrsh_process_args(struct mrsh_state *state, struct mrsh_init_args *args,
	int argc, char *argv[]);

#include <stdbool.h>

/**
 * Expands $PS1 or returns the POSIX-specified default of "$" or "#". The caller
 * must free the return value.
 */
char *mrsh_get_ps1(struct mrsh_state *state, int next_history_id);

/**
 * Expands $PS2 or returns the POSIX-specified default of ">". The caller must
 * free the return value.
 */
char *mrsh_get_ps2(struct mrsh_state *state);

/**
 * Expands $PS4 or returns the POSIX-specified default of "+ ". The caller must
 * free the return value.
 */
char *mrsh_get_ps4(struct mrsh_state *state);

/**
 * Copies variables from the environment and sets up internal variables like
 * IFS, PPID, PWD, etc.
 */
bool mrsh_populate_env(struct mrsh_state *state, char **environ);

/**
 * Sources /etc/profile and $HOME/.profile. Note that this behavior is not
 * specified by POSIX. It is recommended to call this file in login shells
 * (for which argv[0][0] == '-' by convention).
 */
void mrsh_source_profile(struct mrsh_state *state);

/** Sources $ENV. It is recommended to source this in interactive shells. */
void mrsh_source_env(struct mrsh_state *state);

/**
 * Run the trap registered on EXIT. It is recommended to call this function
 * right before exiting the shell.
 */
bool mrsh_run_exit_trap(struct mrsh_state *state);
