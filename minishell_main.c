#include "minishell.h"

// parse_line viene de parseo.c
int parse_line(const char *line, t_intvec *out_codes, t_cmdnode **out_list);

// --- helpers locales (porque en parseo.c intvec_free/list_free son static) ---
static void intvec_reset(t_intvec *v)
{
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

static void intvec_destroy(t_intvec *v)
{
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

static void list_destroy(t_cmdnode *h)
{
    while (h)
    {
        t_cmdnode *nx = h->next;
        free(h->cmd);
        free(h->phrase);
        free(h);
        h = nx;
    }
}

static void print_int_array(const t_intvec *v)
{
    printf("codes: ");
    for (size_t i = 0; i < v->len; i++)
    {
        printf("%d", v->data[i]);
        if (i + 1 < v->len) printf(",");
    }
    printf("\n");
}

static void print_nodes(const t_cmdnode *h)
{
    int idx = 1;
    while (h)
    {
        printf("node%d: cmd=\"%s\" phrase=\"%s\"  prev=%s next=%s\n",
               idx,
               h->cmd ? h->cmd : "",
               h->phrase ? h->phrase : "",
               h->prev ? "SI" : "NULL",
               h->next ? "SI" : "NULL");
        h = h->next;
        idx++;
    }
}

static int is_only_spaces(const char *s)
{
    if (!s) return 1;
    while (*s)
    {
        if (*s != ' ' && *s != '\t' && *s != '\n' && *s != '\r' && *s != '\v' && *s != '\f')
            return 0;
        s++;
    }
    return 1;
}

int main(void)
{
    while (1)
    {
        char *line = NULL;


        size_t cap = 0;
        printf("minishell$ ");
        fflush(stdout);
        ssize_t n = getline(&line, &cap, stdin);
        if (n <= 0)
        {
            free(line);
        }
        if (n > 0 && line[n - 1] == '\n')
            line[n - 1] = '\0';


        // Comando de salida simple (si quieres que sea "exit" exacto)
        if (strcmp(line, "exit") == 0)
        {
            free(line);
            break;
        }

        // Preparar outputs para parse_line
        t_intvec codes;
        intvec_reset(&codes);
        t_cmdnode *list = NULL;

        if (parse_line(line, &codes, &list) == 0)
        {
            // Por ahora, “minishell” solo parsea y muestra el resultado
            print_int_array(&codes);
            print_nodes(list);
        }
        else
        {
            // parse_line ya imprime su error; aquí solo mantenemos el loop
            fprintf(stderr, "(parse error)\n");
        }

        // Liberar
        list_destroy(list);
        intvec_destroy(&codes);
        free(line);
    }
    return 0;
}