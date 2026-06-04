#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "incident.h"

Incident *incident_create(int id, const char *title, Severity severity) {
    Incident *inc = malloc(sizeof(Incident));
    if (inc == NULL) {
        return NULL;
    }
    inc->id = id;
    inc->severity = severity;
    inc->status = STATUS_OPEN;
    inc->created_at = time(NULL);
    snprintf(inc->title, sizeof(inc->title), "%s", title);
    inc->description[0] = '\0';
    return inc;
}

void incident_print(const Incident *inc) {
    const char *sev[] = {"LOW", "MEDIUM", "HIGH", "CRITICAL"};
    const char *stats[] = {"OPEN", "INVESTIGATING", "RESOLVED"};

    printf("Incident %d: %-40s [%s] [%s]\n",
         inc->id,
          inc->title,
           sev[inc->severity],
            stats[inc->status]
        );
}

void incident_free(Incident *inc) {
    free(inc);
}
