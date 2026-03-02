
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
        if (!nd) { perror("realloc"); exit(1); }
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

/* =========================
   Lista doble de nodos
   ========================= */

static t_cmdnode *node_new(const char *cmd, const char *phrase) {
    t_cmdnode *n = (t_cmdnode *)malloc(sizeof(*n));
    if (!n) { perror("malloc"); exit(1); }
    n->cmd = cmd ? strdup(cmd) : NULL;
    n->phrase = phrase ? strdup(phrase) : NULL;
    if (cmd && !n->cmd) { perror("strdup"); exit(1); }
    if (phrase && !n->phrase) { perror("strdup"); exit(1); }
    n->next = NULL;
    n->prev = NULL;
    return n;
}

static void list_append(t_cmdnode **head, t_cmdnode **tail, t_cmdnode *n) {
    if (!*head) {
        *head = n;
        *tail = n;
    } else {
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

/* =========================
   Helpers parseo
   ========================= */

static void skip_spaces(const char *s, size_t *i) {
    while (s[*i] && isspace((unsigned char)s[*i]))
        (*i)++;
}

static char *dup_range(const char *s, size_t a, size_t b_excl) {
    if (b_excl < a) b_excl = a;
    size_t n = b_excl - a;
    char *out = (char *)malloc(n + 1);
    if (!out) { perror("malloc"); exit(1); }
    memcpy(out, s + a, n);
    out[n] = '\0';
    return out;
}

// Une tokens en una sola phrase separada por espacios
static char *append_with_space(char *base, const char *add) {
    if (!add || !*add) return base;

    if (!base) {
        char *out = strdup(add);
        if (!out) { perror("strdup"); exit(1); }
        return out;
    }

    size_t a = strlen(base);
    size_t b = strlen(add);
    char *out = (char *)malloc(a + 1 + b + 1);
    if (!out) { perror("malloc"); exit(1); }

    memcpy(out, base, a);
    out[a] = ' ';
    memcpy(out + a + 1, add, b);
    out[a + 1 + b] = '\0';

    free(base);
    return out;
}

// Operadores:
// 0: comando
// 1: <
// 2: >
// 3: <<
// 4: >>
// 5: pipe (| o ||)
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

static int is_redir_code(int op_code) {
    return (op_code == 1 || op_code == 2 || op_code == 3 || op_code == 4);
}

static const char *op_code_to_str(int op_code) {
    if (op_code == 1) return "<";
    if (op_code == 2) return ">";
    if (op_code == 3) return "<<";
    if (op_code == 4) return ">>";
    if (op_code == 5) return "|";
    return "";
}

// Lee un token:
// - palabra normal hasta espacio u operador
// - '...' o "..." (sin comillas exteriores)
// - ( ... ) simple (sin anidado), útil si lo usáis para "agrupar" (opcional)
static char *read_token_grouped(const char *s, size_t *i, int *grouped_out) {
    size_t start;
    char q;

    if (grouped_out) *grouped_out = 0;
    skip_spaces(s, i);

    if (!s[*i]) return NULL;

    // Si empieza por operador, no es token
    {
        size_t op_len; int op_code;
        if (match_op(s, *i, &op_len, &op_code))
            return NULL;
    }

    if (s[*i] == '\'' || s[*i] == '"') {
        q = s[*i];
        (*i)++;
        start = *i;
        while (s[*i] && s[*i] != q) (*i)++;
        char *tok = dup_range(s, start, *i);
        if (s[*i] == q) (*i)++;
        if (grouped_out) *grouped_out = 1;
        return tok;
    }

    if (s[*i] == '(') {
        (*i)++;
        start = *i;
        while (s[*i] && s[*i] != ')') (*i)++;
        char *tok = dup_range(s, start, *i);
        if (s[*i] == ')') (*i)++;
        if (grouped_out) *grouped_out = 1;
        return tok;
    }

    start = *i;
    while (s[*i] && !isspace((unsigned char)s[*i])) {
        size_t op_len; int op_code;
        if (match_op(s, *i, &op_len, &op_code))
            break;
        (*i)++;
    }
    return dup_range(s, start, *i);
}

/* =========================
   PARSE_LINE
   =========================
   - NO filtra comandos permitidos: acepta cualquier token como comando.
   - Crea nodo del comando EN CUANTO lo detecta (para mantener orden de aparición)
   - Crea nodo por cada redirección, con phrase = token a la derecha
   - Todos los tokens que no sean operadores ni targets de redir
     se concatenan a phrase del comando (para TODOS los comandos)
*/
int parse_line(const char *line, t_intvec *out_codes, t_cmdnode **out_list) {
    size_t i = 0;
    size_t op_len = 0;
    int op_code = -1;

    t_cmdnode *head = NULL, *tail = NULL;

    *out_list = NULL;

    while (line[i]) {
        skip_spaces(line, &i);
        if (!line[i]) break;

        // Operadores fuera de un comando (ej: redir al inicio)
        if (match_op(line, i, &op_len, &op_code)) {
            intvec_push(out_codes, op_code);
            i += op_len;

            if (op_code == 5) {
                // pipe a nivel top: lo registramos y seguimos
                continue;
            }

            if (is_redir_code(op_code)) {
                int grouped_target = 0;
                char *target = read_token_grouped(line, &i, &grouped_target);
                if (!target) {
                    fprintf(stderr,
                        "Error: redirección '%s' sin destino (falta el token a la derecha).\n",
                        op_code_to_str(op_code));
                    list_free(head);
                    intvec_free(out_codes);
                    return -1;
                }
                list_append(&head, &tail, node_new(op_code_to_str(op_code), target));
                free(target);
            }
            continue;
        }

        // Leer comando (aceptamos cualquiera)
        int grouped_cmd = 0;
        char *cmd = read_token_grouped(line, &i, &grouped_cmd);
        if (!cmd) {
            i++;
            continue;
        }

        // Crear nodo del comando inmediatamente (orden de aparición)
        intvec_push(out_codes, 0);
        t_cmdnode *current_cmd = node_new(cmd, NULL);
        list_append(&head, &tail, current_cmd);
        free(cmd);

        // Consumir hasta pipe o fin: tokens normales => args del comando; redirs => nodo aparte
        while (1) {
            size_t j = i;
            skip_spaces(line, &j);

            if (!line[j]) {
                i = j;
                break;
            }

            if (match_op(line, j, &op_len, &op_code)) {
                if (op_code == 5) {
                    // Pipe: registrar y salir al bucle externo (orden de aparición)
                    intvec_push(out_codes, op_code);
                    i = j + op_len;
                    break;
                }

                if (is_redir_code(op_code)) {
                    // Redirección: registrar, crear nodo, consumir target, y SEGUIR (no corta args)
                    intvec_push(out_codes, op_code);
                    i = j + op_len;

                    int grouped_target = 0;
                    char *target = read_token_grouped(line, &i, &grouped_target);
                    if (!target) {
                        fprintf(stderr,
                            "Error: redirección '%s' sin destino (falta el token a la derecha).\n",
                            op_code_to_str(op_code));
                        list_free(head);
                        intvec_free(out_codes);
                        return -1;
                    }
                    list_append(&head, &tail, node_new(op_code_to_str(op_code), target));
                    free(target);
                    continue;
                }

                // Otros operadores (si aparecieran) -> cortar
                i = j;
                break;
            }

            // Token normal => argumento del comando
            i = j;
            int grouped_arg = 0;
            char *tok = read_token_grouped(line, &i, &grouped_arg);
            if (!tok) break;

            current_cmd->phrase = append_with_space(current_cmd->phrase, tok);
            free(tok);
        }
    }

    *out_list = head;
    return 0;
}

/* =========================
   Debug / main de prueba
   ========================= */

static void print_int_array(const t_intvec *v) {
    printf("array de enteros = \"");
    for (size_t k = 0; k < v->len; k++) {
        printf("%d", v->data[k]);
        if (k + 1 < v->len) printf(",");
    }
    printf("\"\n");
}

static void print_nodes(const t_cmdnode *h) {
    int idx = 1;
    while (h) {
        printf("struct%d: prev=%s, next=%s, cmd=\"%s\", phrase=\"%s\"\n",
               idx,
               (h->prev ? "SI" : "NULL"),
               (h->next ? "SI" : "NULL"),
               (h->cmd ? h->cmd : ""),
               (h->phrase ? h->phrase : ""));
        h = h->next;
        idx++;
    }
}

#ifdef PARSEO_TEST
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

        t_cmdnode *list = NULL;
        if (parse_line(line, &codes, &list) == 0) {
            print_int_array(&codes);
            print_nodes(list);
        }

        list_free(list);
        intvec_free(&codes);
        free(line);
        return 0;
    }
#endif