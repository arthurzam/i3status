#include <pthread.h>
#include <yajl/yajl_tree.h>
#include <stdlib.h>

#include "i3status.h"

static const char* events_paths[5][2] = {
    {"name", NULL},
    {"instance", NULL},
    {"button", NULL},
    {"x", NULL},
    {"y", NULL}
};

void* events_thread(void* args)
{
    char input[2048] = {0};
    char errbuf[1024] = {0};
    char* walker;

    yajl_val node;
    yajl_val val;
    char name[512], instance[512];
    int button, x, y;

    while(fgets(input, sizeof(input), stdin))
    {
        walker = input;
        if(*walker == '[')
            walker++;
        if(*walker == '\0')
            continue;
        if(*walker == ',')
            walker++;

        node = yajl_tree_parse(walker, errbuf, sizeof(errbuf));
        if (node == NULL) {
            fprintf(stderr, "parse_error: ");
            if (strlen(errbuf)) fprintf(stderr, " %s", errbuf);
            else fprintf(stderr, "unknown error");
            fprintf(stderr, "\n");
            continue;
        }

        val = yajl_tree_get(node, events_paths[0], yajl_t_string);
        if(val && YAJL_IS_STRING(val))
            strncpy(name, val->u.string, 512);
        else
            name[0] = '\0';

        val = yajl_tree_get(node, events_paths[1], yajl_t_string);
        if(val && YAJL_IS_STRING(val))
            strncpy(instance, val->u.string, 512);
        else
            instance[0] = '\0';

        val = yajl_tree_get(node, events_paths[2], yajl_t_number);
        button = (val ? YAJL_GET_INTEGER(val) : -1);

        val = yajl_tree_get(node, events_paths[3], yajl_t_number);
        x = (val ? YAJL_GET_INTEGER(val) : -1);

        val = yajl_tree_get(node, events_paths[4], yajl_t_number);
        y = (val ? YAJL_GET_INTEGER(val) : -1);

        yajl_tree_free(node);

        if(strcmp(name, "volume") == 0) {
            char* mixer = strchr(instance, '.');
            if(mixer == NULL)
                continue;
            *(mixer++) = '\0';
            char* mixer_id = strchr(mixer, '.');
            if(mixer_id == NULL)
                continue;
            *(mixer_id++) = '\0';
            mouse_volume(button, instance, mixer, atoi(mixer_id));
        }
    }
    return NULL;
}
