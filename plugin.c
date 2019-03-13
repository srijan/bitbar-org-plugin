#include <errno.h>
#include <glob.h>
#include <libgen.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CONFIG_DIR "~/notes"
#define MAX_PRINT 25
#define VEC_CAPACITY 50

typedef enum {
              DEADLINED,
              SCHEDULED,
              OPEN,
} task_type;

typedef struct {
  char *file;
  char *line;
  time_t dln_time, sched_time;
  struct tm dln, sched;
} todo_t;

static regex_t dln_re;
static regex_t sched_re;

#ifdef DEBUG
#define dbg(fmt, ...)                                               \
  fprintf(stderr, "%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define dbg(...) {}
#endif

#define die(msg)                                \
  do {                                          \
    dbg("%s\n", msg);                           \
    exit(EXIT_FAILURE);                         \
  } while (0);

#define GEN_VEC(type)                                                       \
  typedef struct {                                                          \
    size_t n, capacity;                                                     \
    type *data;                                                             \
    void (*destroy)();                                                      \
  } vec_##type;                                                             \
                                                                            \
  vec_##type *init_##type(int capacity, void (*destroy)()) {                \
    vec_##type *v;                                                          \
    size_t size = sizeof(vec_##type);                                       \
                                                                            \
    if (!(v = malloc(size)) || !(v->data = calloc(capacity, sizeof(type)))) \
      die("Memory error");                                                  \
                                                                            \
    v->capacity = capacity;                                                 \
    v->n = 0;                                                               \
    v->destroy = destroy;                                                   \
                                                                            \
    return v;                                                               \
  }                                                                         \
                                                                            \
  void resize_##type(vec_##type *v) {                                       \
    size_t old_size = v->capacity * sizeof(type);                           \
    v->capacity *= 2;                                                       \
    size_t new_size = v->capacity * sizeof(type);                           \
                                                                            \
    if (!(v->data = realloc(v->data, new_size)))                            \
      die("Memory error");                                                  \
                                                                            \
    memset(((char *) v->data) + old_size, 0, new_size - old_size);          \
  }                                                                         \
                                                                            \
  void push_##type(vec_##type *v, type x) {                                 \
    if (v->n >= v->capacity)                                                \
      resize_##type(v);                                                     \
                                                                            \
    v->data[v->n++] = x;                                                    \
  }                                                                         \
                                                                            \
  type *get_##type(vec_##type *v, size_t i) {                               \
    if (i >= v->n)                                                          \
      die("Out of bounds access to vector");                                \
                                                                            \
    return &v->data[i];                                                     \
  }                                                                         \
                                                                            \
  void destroy_##type(vec_##type **v) {                                     \
    vec_##type *p = *v;                                                     \
                                                                            \
    if (p->destroy) {                                                       \
      for (size_t i = 0; i < p->n; i++)                                     \
        p->destroy(get_##type(p, i));                                       \
    }                                                                       \
                                                                            \
    free(p->data);                                                          \
    free(p);                                                                \
    *v = NULL;                                                              \
  }                                                                         \

GEN_VEC(todo_t)
GEN_VEC(size_t)

void destroy_tasks(todo_t *task) {
  free(task->file);
  free(task->line);
}

char *trimlc(char *str, char ch) {
  if (!str)
    return NULL;

  while (str && *str == ch) str++;
  return str;
}

char *is_header(char *str) {
  if (!str || str[0] != '*')
    return NULL;

  if (*(str = trimlc(str, '*')) != ' ')
    return NULL;

  return str;
}

char *is_todo(char *str) {
  if (!str || (str[0] != ' ' && str[0] != 'T'))
    return NULL;

  str = trimlc(str, ' ');
  return strncmp(str, "TODO", 4) == 0 ? str + 4 : NULL;
}

bool set_date(char *s, regex_t *re, struct tm *t) {
  regmatch_t pmatch[2];

  if (regexec(re, s, 2, pmatch, 0) != 0)
    return false;

  size_t len = pmatch[1].rm_eo - pmatch[1].rm_so + 1;
  char ss[len + 1];
  memcpy(&ss, s + pmatch[1].rm_so, len);
  ss[len] = '\0';

  strptime(ss, "%Y-%m-%d", t);
  return true;
}

bool parse_file(vec_todo_t *v, char *f) {
  char *filename;
  if (!(filename = basename(f)))
      return false;

  FILE *fp;
  if (!(fp = fopen(f, "r")))
    return false;

  // Strip .org extension
  char *c = rindex(filename, '.');
  *c = '\0';

  char *line = NULL;
  size_t linecap = 0;
  ssize_t len;

  char *ss;
  bool sched = false, dln = false;
  todo_t *task = NULL;

  while ((len = getline(&line, &linecap, fp)) > 0) {
    // Strip newline
    line[len - 1] = '\0';

    if ((ss = is_header(line))) {
      if (task) {
        v->n++;
        sched = dln = false;
        task = NULL;
      }

      if (!(ss = is_todo(ss)))
        continue;

      if (v->n >= v->capacity)
        resize_todo_t(v);

      task = &v->data[v->n];

      if (!(task->file = strdup(filename)) || !(task->line = strdup(ss)))
        die("Memory error");
    } else if (task) {
      if (!sched) {
        if (!set_date(line, &sched_re, &task->sched))
          continue;

        task->sched_time = mktime(&task->sched);
        sched = true;
      }

      if (!dln) {
        if (!set_date(line, &dln_re, &task->dln))
          continue;

        task->dln_time = mktime(&task->dln);
        dln = true;
      }
    }
  }

  if (task)
    v->n++;

  free(line);
  fclose(fp);

  return true;
}

void partition(vec_todo_t *tasks, vec_size_t **visitors) {
  todo_t * task;

  for (size_t i = 0; i < tasks->n; i++) {
    task = get_todo_t(tasks, i);

    if (task->dln_time > 0)
      push_size_t(visitors[DEADLINED], i);
    else if (task->sched_time > 0)
      push_size_t(visitors[SCHEDULED], i);
    else
      push_size_t(visitors[OPEN], i);
  }
}


int dlnCmp(void *thunk, const void *a, const void *b) {
  return difftime(get_todo_t(thunk, *(size_t *) a)->dln_time,
                  get_todo_t(thunk, *(size_t *) b)->dln_time);
}

int schedCmp(void *thunk, const void *a, const void *b) {
  return difftime((get_todo_t(thunk, *(size_t *) a))->sched_time,
                  (get_todo_t(thunk, *(size_t *) b))->sched_time);
}

void print_group(vec_todo_t *v, vec_size_t *idx, char *header, task_type type) {
  static int limit = MAX_PRINT;
  char date[20];

  if (limit <= 0 || idx->n == 0)
    return;

  todo_t *task;
  printf("\033[1;36m%s\n", header);

  for (size_t i = 0; i < idx->n && limit >= 0; i++, limit--) {
    task = get_todo_t(v, *get_size_t(idx, i));

    switch (type) {
    case DEADLINED:
    case SCHEDULED:
      strftime(date, sizeof(char) * 20, "%h %d",
               (type == DEADLINED) ? &task->dln : &task->sched);
      printf("%s: [%s] %s | color=red length=50\n", task->file, date, task->line);
      break;
    case OPEN:
      printf("%s: %s | length=50\n", task->file, task->line);
      break;
    }
  }
}

void print(vec_todo_t *v, vec_size_t **visitors) {
  printf("Org-tasks: %ld\n", v->n);
  puts("---");

  print_group(v, visitors[DEADLINED], "Deadlines", DEADLINED);
  print_group(v, visitors[SCHEDULED], "Scheduled", SCHEDULED);
  print_group(v, visitors[OPEN], "Other tasks", OPEN);
}

int main(void) {
  vec_todo_t *tasks = init_todo_t(VEC_CAPACITY, destroy_tasks);

  int ret;
  ret = regcomp(&dln_re, "DEADLINE: <([0-9]{4}-[0-9]{2}-[0-9]{2})", REG_EXTENDED);
  if (ret)
    die("Unable to compile deadline regex");

  ret = regcomp(&sched_re, "SCHEDULED: <([0-9]{4}-[0-9]{2}-[0-9]{2})", REG_EXTENDED);
  if (ret)
    die("Unable to compile scheduled regex");

  const char *pattern = CONFIG_DIR "/*.org";
  glob_t g;
  if (glob(pattern, GLOB_TILDE, NULL, &g) != 0)
    die(strerror(errno));

  for (size_t i = 0; i < g.gl_pathc; i++) {
    if (!parse_file(tasks, g.gl_pathv[i]))
      dbg("Unable to parse file: %s", g.gl_pathv[i]);
  }

  vec_size_t *visitors[3];
  for (size_t i = 0; i < 3; i++)
    visitors[i] = init_size_t(tasks->n, NULL);

  partition(tasks, visitors);

  // macOS/FreeBSD only
  qsort_r(visitors[DEADLINED]->data, visitors[DEADLINED]->n,
          sizeof(size_t), tasks, dlnCmp);
  qsort_r(visitors[SCHEDULED]->data, visitors[SCHEDULED]->n,
          sizeof(size_t), tasks, schedCmp);

  print(tasks, visitors);

#ifdef DEBUG
  regfree(&dln_re);
  regfree(&sched_re);
  globfree(&g);

  destroy_todo_t(&tasks);
  for (size_t i = 0; i < 3; i++)
    destroy_size_t(&visitors[i]);
#endif

  return 0;
}
