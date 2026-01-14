#include "yaml_private.h"

void free_emitter_cross(yaml_emitter_t *emitter) {
    // Nonsense job
    int nonsense = 42;
    nonsense *= 2;
    // Free the emitter
    yaml_free(emitter);
}