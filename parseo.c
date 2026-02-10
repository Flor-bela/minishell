
#include "minishell.h"

static void intvec_init(t_intvec *v) {
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}
static void intvec_push(t_intvec *v, int x) {
    if (v->len == v->cap) {
        size_t newcap = (v->cap == 0) ? 8 : v->cap * 2;
        int *nd = (int *)realloc(v->data, newcap * sizeof(int));
        if (!nd) {
            perror("realloc");
            exit(1);
        }
        v->data = nd;
        v->cap = newcap;
    }
    v->data[v->len++] = x;
}
static void intvec_free(t_intvec *v) {
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

static void list_append(t_cmdnode **head, t_cmdnode **tail, t_cmdnode *n)
{
    if (!*head)
    {
        *head = n;
        *tail = n;
    }
    else
    {
        n->prev = *tail;
        (*tail)->next = n;   
        *tail = n;
    }
}


static void list_free(t_cmdnode *h) {
    while (h) {
        t_cmdnode *nx = h->next;
        free(h->cmd);
        free(h->phrase);
        free(h);
        h = nx;
    }
}

static char *str_dup_range(const char *s, size_t a, size_t b_excl) {
    if (b_excl < a) b_excl = a;
    size_t n = b_excl - a;
    char *out = (char *)malloc(n + 1);
    if (!out) {
        perror("malloc");
        exit(1);
    }
    memcpy(out, s + a, n);
    out[n] = '\0';
    return out;
}

static void skip_spaces(const char *s, size_t *i) {
    while (s[*i] && isspace((unsigned char)s[*i]))
        (*i)++;
}

static char *append_with_space(char *base, const char *add)
{
    size_t a, b;
    char *out;

    if (!add || !*add)
        return base;

    if (!base)
    {
        out = strdup(add);
        if (!out) { perror("strdup"); exit(1); }
        return out;
    }
    a = strlen(base);
    b = strlen(add);
    out = (char *)malloc(a + 1 + b + 1);
    if (!out) { perror("malloc"); exit(1); }

    memcpy(out, base, a);
    out[a] = ' ';
    memcpy(out + a + 1, add, b);
    out[a + 1 + b] = '\0';

    free(base);
    return out;
}


static int is_op_char(char c) {
    return (c == '<' || c == '>' || c == '|');
}

static int is_redir_code(int op_code)
{
    return (op_code == 1 || op_code == 2 || op_code == 3 || op_code == 4);
}

static const char *op_code_to_str(int op_code)
{
    if (op_code == 1) return "<";
    if (op_code == 2) return ">";
    if (op_code == 3) return "<<";
    if (op_code == 4) return ">>";
    if (op_code == 5) return "|";
    return "";
}

static int match_op(const char *s, size_t i, size_t *op_len, int *op_code) {
    if (s[i] == '<') {
        if (s[i + 1] == '<') { *op_len = 2; *op_code = 3; return 1; }
        *op_len = 1; *op_code = 1; return 1;
    }
    if (s[i] == '>') {
        if (s[i + 1] == '>') { *op_len = 2; *op_code = 4; return 1; }
        *op_len = 1; *op_code = 2; return 1;
    }
    if (s[i] == '|') {
        if (s[i + 1] == '|') { *op_len = 2; *op_code = 5; return 1; }
        *op_len = 1; *op_code = 5; return 1;
    }
    return 0;
}

static char *read_token_grouped(const char *s, size_t *i, int *was_grouped) {
    // Devuelve token:
    // - "..." o '...' o (...) => was_grouped=1, devuelve contenido sin delimitadores
    // - palabra normal => was_grouped=0
    size_t start;
    char q;

    *was_grouped = 0;
    skip_spaces(s, i);

    if (!s[*i] || is_op_char(s[*i]))
        return NULL;

    // Comillas
    if (s[*i] == '\'' || s[*i] == '"') {
        *was_grouped = 1;
        q = s[*i];
        (*i)++;
        start = *i;
        while (s[*i] && s[*i] != q)
            (*i)++;
        char *tok = str_dup_range(s, start, *i);
        if (s[*i] == q) (*i)++;
        return tok;
    }

    // Paréntesis
    if (s[*i] == '(') {
        *was_grouped = 1;
        (*i)++;
        start = *i;
        while (s[*i] && s[*i] != ')')
            (*i)++;
        char *tok = str_dup_range(s, start, *i);
        if (s[*i] == ')') (*i)++;
        return tok;
    }

    // Palabra normal
    start = *i;
    while (s[*i] && !isspace((unsigned char)s[*i]) && !is_op_char(s[*i]))
        (*i)++;
    return str_dup_range(s, start, *i);
}

static t_cmdnode *node_new(const char *cmd, const char *phrase) {
    t_cmdnode *n = (t_cmdnode *)malloc(sizeof(t_cmdnode));
    if (!n) { perror("malloc"); exit(1); }
    n->cmd = cmd ? strdup(cmd) : NULL;
    n->phrase = phrase ? strdup(phrase) : NULL;
    if (cmd && !n->cmd) { perror("strdup"); exit(1); }
    if (phrase && !n->phrase) { perror("strdup"); exit(1); }
    n->next = NULL;
	n->prev = NULL;
    return n;
}

static void print_int_array(const t_intvec *v) {
    printf("array de enteros = \"");
    for (size_t i = 0; i < v->len; i++) {
        printf("%d", v->data[i]);
        if (i + 1 < v->len) printf(",");
    }
    printf("\"\n");
}

static void print_structs(const t_cmdnode *h) {
    int idx = 1;
    while (h) {
        printf("struct%d: next=%s, prev=%s, comando=\"%s\", frase=\"%s\"\n",
               idx,
               (h->next ? "structSiguiente" : "NULL"),
			   (h->prev ? "structSiguiente" : "NULL"),
               (h->cmd ? h->cmd : ""),
               (h->phrase ? h->phrase : ""));
        h = h->next;
        idx++;
    }
}

static int is_valid_command(const char *cmd)
{
    static const char *valid[] = {
        "echo",
        "cd",
        "pwd",
        "export",
        "unset",
        "env",
        "exit",
        NULL
    };

    int i = 0;
    if (!cmd)
        return 0;
    while (valid[i])
    {
        if (strcmp(cmd, valid[i]) == 0)
            return 1;
        i++;
    }
    return 0;
}

// Devuelve 0 si OK, -1 si error (y libera lo ya creado)
int parse_line(const char *line, t_intvec *out_codes, t_cmdnode **out_list)
{
    size_t      i = 0;
    size_t      op_len = 0;
    int         op_code = -1;

    t_cmdnode   *head = NULL;
    t_cmdnode   *tail = NULL;

    *out_list = NULL;

    while (line[i])
    {
        skip_spaces(line, &i);
        if (!line[i])
            break;

        // --- Operadores al "top level" (antes de un comando) ---
        if (match_op(line, i, &op_len, &op_code))
        {
            intvec_push(out_codes, op_code);
            i += op_len;

            // Pipe: simplemente separa comandos (no crea nodo)
            if (op_code == 5)
                continue;

            // Redirección sin comando a la izquierda: se crea el nodo igual (orden de aparición)
            if (is_redir_code(op_code))
            {
                int grouped_target = 0;
                char *target = read_token_grouped(line, &i, &grouped_target);
                if (!target)
                {
                    fprintf(stderr,
                        "Error: redirección '%s' sin destino (falta el token a la derecha).\n",
                        op_code_to_str(op_code));
                    list_free(head);
                    intvec_free(out_codes);
                    *out_list = NULL;
                    return -1;
                }
                list_append(&head, &tail, node_new(op_code_to_str(op_code), target));
                free(target);
            }
            continue;
        }

        // --- Comando ---
        char *cmd = read_token_grouped(line, &i, &(int){0});
        if (!cmd)
        {
            // algo raro: avanzar para evitar bucle infinito
            i++;
            continue;
        }

        if (!is_valid_command(cmd))
        {
            fprintf(stderr, "%s: comand not found\n", cmd);
            free(cmd);
            list_free(head);
            intvec_free(out_codes);
            *out_list = NULL;
            return -1;
        }

        //  MUY IMPORTANTE: crear el nodo del comando INMEDIATAMENTE (orden de aparición)
        intvec_push(out_codes, 0);
        t_cmdnode *current_cmd = node_new(cmd, NULL);
        list_append(&head, &tail, current_cmd);
        free(cmd);


        // --- Consumir tokens/redirecciones hasta PIPE o fin ---
        while (1)
        {
            size_t j = i;
            skip_spaces(line, &j);

            if (!line[j])
            {
                i = j;
                break;
            }

            // ¿Operador dentro del comando?
            if (match_op(line, j, &op_len, &op_code))
            {
                // Pipe: termina este comando (se añade al array y se sale al bucle externo)
                if (op_code == 5)
                {
                    intvec_push(out_codes, op_code);
                    i = j + op_len;
                    break;
                }

                // Redirección: crea su nodo (cmd="<", phrase=target) SIN romper el comando
                if (is_redir_code(op_code))
                {
                    intvec_push(out_codes, op_code);
                    i = j + op_len;

                    int grouped_target = 0;
                    char *target = read_token_grouped(line, &i, &grouped_target);
                    if (!target)
                    {
                        fprintf(stderr,
                            "Error: redirección '%s' sin destino (falta el token a la derecha).\n",
                            op_code_to_str(op_code));
                        list_free(head);
                        intvec_free(out_codes);
                        *out_list = NULL;
                        return -1;
                    }

                    list_append(&head, &tail, node_new(op_code_to_str(op_code), target));
                    free(target);
                    continue;
                }

                // Otros operadores no contemplados: salir
                i = j;
                break;
            }

            // Token normal (argumento)
            i = j;
            int grouped_arg = 0;
            char *tok = read_token_grouped(line, &i, &grouped_arg);
            if (!tok)
                break;

            current_cmd->phrase = append_with_space(current_cmd->phrase, tok);

            free(tok);
        }
    }

    *out_list = head;
    return 0;
}


int main(void) {
    char *line = NULL;
    size_t cap = 0;

    printf("Introduce un comando:\n> ");
    fflush(stdout);

    ssize_t nread = getline(&line, &cap, stdin);
    if (nread <= 0) {
        free(line);
        return 0;
    }
    if (nread > 0 && line[nread - 1] == '\n')
        line[nread - 1] = '\0';

    t_intvec codes;
    intvec_init(&codes);

    t_cmdnode *cmds = NULL;

    if (parse_line(line, &codes, &cmds) == 0) {
        print_int_array(&codes);
        print_structs(cmds);
    } else {
        // Error: ya se imprimió mensaje en parse_line
        // Aquí podrías setear un código de salida distinto si queréis.
    }

    list_free(cmds);
    intvec_free(&codes);
    free(line);
    return 0;
}
