#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "fy_common.h"
#include "menu.h"
#include "xvr_demo.h"

#define UNUSED_PARAM __attribute__ ((__unused__))

#define XVR_DEMO_VERSION "0.5.1"


static const char* applet_names[] = {
    "nvr_sample",
    "dvr_sample",
#if defined(ISP_ENABLE)
    "smh_sample",
#endif
    NULL
};

int (*const applet_main[])(int argc, char **argv) = {
    nvr_main,
    dvr_main,
#if defined(ISP_ENABLE)
    smh_main,
#endif
    NULL
};

static const char* applet_desc[] = {
    "nvr samples",
    "dvr samples",
#if defined(ISP_ENABLE)
    "smh samples",
#endif
    NULL
};



static const char* basename(const char *name)
{
    const char *cp = strrchr(name, '/');
    if(cp) {
        return cp + 1;
    }
    return name;
}

static char* is_prefixed_with(const char *string, const char *key)
{
    while(*key != '\0') {
        if(*key != *string) {
            return NULL;
        }
        key++;
        string++;
    }
    return (char*)string;

}

static unsigned string_array_len(char **argv)
{
    char **start = argv;

    while(*argv) {
        argv++;
    }

    return argv - start;
}

static int find_applet_by_name(const char *name)
{
    unsigned i;
    i = 0;
    while(applet_names[i]) {
        if(0 == strcmp(applet_names[i], name)) {
            return i;
        } else {
            ++i;
        }
    }
    return -1;
}


static void run_applet_no_and_exit(int applet_no, const char *name, char **argv)
{
    int argc = string_array_len(argv);
    int retval = 0;

    retval = applet_main[applet_no](argc, argv);
    /* Note: applet_main() may also not return (die on a xfunc or such) */
    exit(retval);
}


static void usage()
{
    int i = 0;
    printf("%s\n\n", FYMODULE_VERSION("xvr_demo", XVR_DEMO_VERSION));
    printf("Usage: xvr_demo [function [arguments]...]\n");
    printf("   or: function [arguments]...\n");

    printf("\n\nCurrently defined functions:\n");

    for(i = 0; NULL != applet_names[i]; i++) {
        printf("    %16s: %s\n", applet_names[i], applet_desc[i]);

    }


}

int main(int argc UNUSED_PARAM, char **argv)
{
    const char *applet_name;

	system("telnetd");

    if(is_prefixed_with(basename(argv[0]), "xvr_demo")) {
        if(argv[1]) {
            argv++;
        } else {
            usage();
            return 0;
        }
    }


    applet_name = argv[0];
    if(applet_name[0] == '-') {
        applet_name++;
    }
    applet_name = basename(applet_name);

    int applet = find_applet_by_name(applet_name);
    if(applet >= 0) {
        run_applet_no_and_exit(applet, applet_name, argv);
    } else {
        printf("%s: applet not found\n", applet_name);
        exit(127);
    }

    return 0;
}
