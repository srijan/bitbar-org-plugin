#define BLACK   "#403842"
#define RED     "#CA411E"
#define GREEN   "#9AC666"
#define YELLOW  "#E6B352"
#define BLUE    "#5350C3"
#define MAGENTA "#9B7E9B"
#define CYAN    "#6FC1B6"
#define GREY    "#959894"


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
const char *scheduled_task_color = BLUE;
const char *deadline_task_color = RED;
const char *unscheduled_task_color = GREY;

const char *priority_colors[] =  { RED, GREEN, CYAN };
