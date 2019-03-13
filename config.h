/* ANSI Colors */
#define RESET   "\x1b[0m"

#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"

/* Maximum number of tasks to display in dropdown menu */
#define MAX_PRINT 25

/* Maximum display length for menu items */
#define MAX_WIDTH 75

/* Glob pattern for Org files */
const char *org_file_pattern = "~/notes/*.org";

/* Org-priorities */
const char *priorities[] = { "[#A]", "[#B]", "[#C]" };

/* Menu font */
const char *font = "Consolas";
const int font_size = 12;

/* Color Options */
const char *header_color = YELLOW;
const char *scheduled_task_color = CYAN;
const char *deadline_task_color = RED;
const char *unscheduled_task_color = BLUE;

const char *priority_colors[] =  { RED, GREEN, CYAN };
