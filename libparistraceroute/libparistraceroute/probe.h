#ifndef PROBE_H
#define PROBE_H

/**
 * \file probe.h
 * \brief Header for probes
 */

#include <stdbool.h>
#include "stackedlist.h"
#include "field.h"
#include "layer.h"
#include "buffer.h"
#include "bitfield.h"

#define WAITING_RESP 0
#define TTL_RESP 1
#define NO_RESP 2
#define END_RESP 3
#define OTHER_RESP 4

/**
 * \struct probe_t
 * \brief Structure representing a probe
 */
typedef struct {
    /** Fields that have not yet been attributed to a protocol */
    layer_t *top_layer;
    /** Buffer that will store probe fields content */
    buffer_t *buffer;
    /** Bitfield to keep track of modified fields vs. default ones */
    bitfield_t *bitfield;
} probe_t;

/**
 * \brief Create a probe
 * \return A pointer to a probe_t structure containing the probe
 */
probe_t * probe_create(void);

/**
 * \brief Free a probe
 * \param probe A pointer to a probe_t structure containing the probe
 * \return None
 */
void probe_free(probe_t *probe);

/**
 * \brief Add a field to a probe
 * \param probe A pointer to a probe_t structure containing the probe
 * \param field A pointer to a field_t structure containing the field
 * \return None
 */
void probe_add_field(probe_t *probe, field_t *field);

/**
 * \brief Assigns a set of fields to a probe
 * \param probe A pointer to a probe_t structure representing the probe
 * \param arg1 The first of a list of pointers to a field_t structure representing a field to add
 * \return 0 if successful,
 */
int probe_set_fields(probe_t *probe, field_t *field1, ...);

/**
 * \brief Iterates through the fields in a probe, passing them as the argument to a callback function
 * \param probe A pointer to a probe_t structure representing the probe
 * \param data A pointer to hold the returned data from the callback (?)
 * \param callback A function pointer to a callback function
 * \return None
 */
void probe_iter_fields(probe_t *probe, void *data, void (*callback)(field_t *field, void *data));

/**
 * \brief Get an array of pointers to the fields of a probe
 * \param probe Pointer to a probe_t structure to get the fields from
 * \return NULL (NYI - Will be Array of pointers to field_t structures stored in the probe)
 */
field_t ** probe_get_fields(probe_t *probe);
/**
 * \brief Get the payload from a probe
 * \param probe Pointer to a probe_t structure to get the payload from
 * \return NULL (Not yet implemented)
 */
unsigned char *probe_get_payload(probe_t *probe);
/**
 * \brief Get the size of the payload from a probe
 * \param probe Pointer to a probe_t structure to get the payload size from
 * \return 0 (NYI - Will be Size of the probe's payload)
 */
unsigned int probe_get_payload_size(probe_t *probe);
/**
 * \brief Get the protocol from the probe with the given index
 * \param i Integer index of the probe to use
 * \return NULL (NYI - Will be Pointer to the name of the protocol)
 */
char* probe_get_protocol_by_index(unsigned int i);

/**
 * \brief Get the protocol number (?) from a probe
 * \param probe Pointer to a probe_t structure to use
 * \return 0 (NYI)
 */
unsigned int probe_get_num_proto(probe_t *probe);
/**
 * \brief Get the header fields from a probe
 * \param probe Pointer to a probe_t structure to use
 * \return NULL (NYI - Will be Array of pointers to the field_t structures contained in the given probe)
 */
field_t ** probe_get_fields(probe_t *probe);

/******************************************************************************
 * pt_loop_t
 ******************************************************************************/
struct pt_loop_s;

/**
 * \brief Callback function to handle the reply from a probe
 * \param loop Pointer to a pt_loop_s structure within which the probe is running (?)
 * \param probe Pointer to a probe_t structure containing the probe that was sent (?)
 * \param reply Pointer to an empty probe_t structure to hold the reply (?)
 * \return None
 */
void pt_probe_reply_callback(struct pt_loop_s *loop, probe_t *probe, probe_t *reply);
/**
 * \brief Send a probe across the network
 * \param loop Pointer to a pt_loop_s structure describing the loop in which the probe should run (?)
 * \param probe Pointer to a probe_t structure describing the probe to send
 * \return None
 */
void pt_probe_send(struct pt_loop_s *loop, probe_t *probe);


#endif