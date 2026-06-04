#ifndef INCIDENT_H
#define INCIDENT_H

#include <time.h>

typedef enum {
    SEVERITY_LOW,
    SEVERITY_MEDIUM,
    SEVERITY_HIGH,
    SEVERITY_CRITICAL
} Severity;


typedef enum {
    STATUS_OPEN,
    STATUS_INVESTIGATING,
    STATUS_RESOLVED
} IncidentStatus;

typedef struct {
    int id;
    char title[128];
    char description[512];
    Severity severity;
    IncidentStatus status;
    time_t created_at;
} Incident;

Incident *incident_create(int id, const char *title, Severity severity);
void incident_print(const Incident *inc);
void incident_free(Incident *inc);





#endif