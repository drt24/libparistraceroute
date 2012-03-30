#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "probe.h"
#include "protocol.h"
#include "pt_loop.h"

probe_t * probe_create(void)
{
    probe_t *probe;

    probe = malloc(sizeof(probe_t));

    /* Create the buffer to store the field content */
    probe->buffer = buffer_create();
    if (!(probe->buffer))
        goto error;

    /* Initially the probe has no top layer... */
    probe->top_layer = NULL;

    /* ... and an empty bitfield */
    probe->bitfield = bitfield_create(0);

    return probe;

error:
    free(probe);
    probe = NULL;

    return NULL;
}

void probe_free(probe_t *probe)
{
    buffer_free(probe->buffer);
    free(probe);
    probe = NULL;
}

// What if we call directly layer_set_protocol ??
int probe_set_protocols(probe_t *probe, char *protocol1, ...)
{
    int res;
    va_list args;
    size_t buflen = 0;
    layer_t *cur_layer;

    /* define the layer structure */
    va_start(args, protocol1);
    res = layer_set_protocols(probe->top_layer, protocol1, args);
    va_end(args);

    if (res < 0)
        goto error;

    /* allocate the buffer according to the layer structure */
    for (cur_layer = probe->top_layer; cur_layer; cur_layer = cur_layer->sublayer) {
        /* We would need to consider variable length headers */
        buflen += cur_layer->protocol ? cur_layer->protocol->header_len : cur_layer->size;
    }
    buffer_resize(probe->buffer, buflen);

    /* Initialize the buffer with default protocol values */
    for (cur_layer = probe->top_layer; cur_layer; cur_layer = cur_layer->sublayer) {
        /* We would need to consider variable length headers */
        cur_layer->protocol->write_default_header(cur_layer->offset);
    }

    return 0;

error:
    return -1;
}

int probe_set_fields(probe_t *probe, field_t *field1, ...) {
    int res;
    va_list args;

    va_start(args, field1);
    res = layer_set_fields(probe->top_layer, field1, args);
    va_end(args);
    
    return res;
}

/* Iterator */

typedef struct {
    void *data;
    void (*callback)(field_t *field, void *data);
} iter_fields_data_t;

void probe_iter_fields_callback(void *element, void *data) {
    iter_fields_data_t *d = (iter_fields_data_t*)data;
    d->callback((field_t*)element, d->data);
}

void probe_iter_fields(probe_t *probe, void *data, void (*callback)(field_t *, void *))
{
    /*
    iter_fields_data_t tmp = {
        .data = data,
        .callback = callback
    };
    */
    
    // not implemented : need to iter over protocol fields of each layer
}

unsigned int probe_get_num_proto(probe_t *probe)
{
    return 0; // TODO
}

field_t ** probe_get_fields(probe_t *probe)
{
    return NULL; // TODO
}

unsigned char *probe_get_payload(probe_t *probe)
{
    // point into the packet structure
    return NULL; // TODO
}

unsigned int probe_get_payload_size(probe_t *probe)
{
    return 0; // TODO
}

char* probe_get_protocol_by_index(unsigned int i)
{
    return NULL; // TODO
}

/******************************************************************************
 * pt_loop_t
 ******************************************************************************/

void pt_probe_reply_callback(pt_loop_t *loop, probe_t *probe, probe_t *reply)
{
    // Search for probe and find the caller
    //algorithm_instance_add_event(instance->caller->caller_algorithm,
    //        event_create(PROBE_REPLY_RECEIVED, NULL));
    // Delete the probe, what about multiple answers ?
    return;
}

void pt_probe_send(pt_loop_t *loop, probe_t *probe)
{
    /* We need to remember who has been sending the probe */
    /* + Reply callback */
    network_send_probe(loop->network, probe, pt_probe_reply_callback);
}