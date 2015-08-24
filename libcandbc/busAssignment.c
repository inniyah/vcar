/*  busAssignment.c -- assign bus id to DBC file
	Copyright (C) 2007-2009 Andreas Heitmann

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "busAssignment.h"
#include "showDbc.h"

#include "hashtable/hashtable.h"
#include "hashtable/hashtable_itr.h"

#include <stdio.h>
#include <string.h>

busAssignment_t *busAssignment_create(void) {
	CREATE(busAssignment_t, busAssignment);

	busAssignment->n = 0;
	busAssignment->list = NULL;
	return busAssignment;
}


void busAssignment_associate(busAssignment_t * busAssignment, int bus, const char * filename) {
	busAssignment->n++;
	busAssignment->list = (busAssignmentEntry_t *)realloc(busAssignment->list, busAssignment->n * sizeof(*(busAssignment->list)));
	busAssignment->list[busAssignment->n-1].bus = bus;
	busAssignment->list[busAssignment->n-1].filename = strdup(filename);
	busAssignment->list[busAssignment->n-1].messageHash = NULL;
}


int busAssignment_parseDBC(busAssignment_t * busAssignment) {
	int i;
	int ret = 0;

	for (i = 0; i < busAssignment->n; i++) {
		dbc_t *dbc;

		#if 0
		fprintf(stderr, "Parsing DBC file %s\n", busAssignment->list[i].filename);
		#endif

		dbc = dbc_read_file(busAssignment->list[i].filename);
		if(NULL != dbc) {
#if 0
			show_dbc_network(dbc);
			show_dbc_message_list(dbc->message_list);
			show_dbc_signals(dbc);
			show_dbc_nodes(dbc);
			show_dbc_envvars(dbc->envvar_list);
			show_dbc_valtable_list(dbc->valtable_list);
#endif

			messageHash_t *messageHash = messageHash_create(dbc->message_list);

			// busAssignment->list[i].databaseName = NULL;
			busAssignment->list[i].messageHash = messageHash;
			if(messageHash == NULL) {
				fprintf(stderr,
					"busAssignment_parseDBC(): error parsing DBC file %s\n",
					busAssignment->list[i].filename);
				ret = 1;
				break;
			}
			dbc_free(dbc);
		} else {
			fprintf(stderr, "busAssignment_parseDBC(): error opening DBC file %s\n",
				busAssignment->list[i].filename);
			ret = 1;
			break;
		}
	}
	return ret;
}


void busAssignment_free(busAssignment_t * busAssignment) {
	int i;

	if (busAssignment != NULL) {
		for (i = 0; i < busAssignment->n; i++) {
			busAssignmentEntry_t *entry = &(busAssignment->list[i]);
			free(entry->filename);
			messageHash_free(entry->messageHash);
		}
		if(busAssignment->list != NULL) {
			free(busAssignment->list);
		}
	}
	free(busAssignment);
}

static void busAssignment_messageHash_print(struct hashtable * const h) {
	if(NULL != h) {
		if (hashtable_count(h) > 0) {
			struct hashtable_itr *itr = hashtable_iterator(h);
			do {
				message_t * m = hashtable_iterator_value(itr);
				if (m) {
					signal_list_t * sl;
					printf("  CAN MSG 0x%lX (%s)\n", (unsigned long)m->id, m->name);
					for(sl = m->signal_list; sl != NULL; sl = sl->next) {
						printf("    SIGNAL %s\n", sl->signal->name);
					}

				}
			} while (hashtable_iterator_advance(itr));
		}
	}
}

void busAssignment_print(busAssignment_t * busAssignment) {
	int i;

	if (busAssignment != NULL) {
		for (i = 0; i < busAssignment->n; i++) {
			busAssignmentEntry_t * entry = &(busAssignment->list[i]);
			if (entry) {
				printf("CAN BUS %d\n", entry->bus);
				busAssignment_messageHash_print(entry->messageHash);
			}
		}
	}
}

void busAssignment_iterate(
	busAssignment_t * busAssignment,
	busAssignmentMessageCallback msg_cb,
	busAssignmentSignalCallback sgn_cb,
	busAssignmentAttributeCallback att_cb,
	void * arg
) {
	int i;
	if (busAssignment != NULL) {
		for (i = 0; i < busAssignment->n; i++) {
			busAssignmentEntry_t * entry = &(busAssignment->list[i]);
			if (entry) {
				struct hashtable * const h = entry->messageHash;
				if (NULL != h) {
					if (hashtable_count(h) > 0) {
						struct hashtable_itr *itr = hashtable_iterator(h);
						do {
							message_t * m = hashtable_iterator_value(itr);
							if (m) {
								if (NULL != msg_cb) {
									msg_cb(entry->bus, m, arg);
								}
								if (NULL != msg_cb) {
									signal_list_t * sl;
									for(sl = m->signal_list; sl != NULL; sl = sl->next) {
										sgn_cb(entry->bus, m, sl->signal, arg);
										if (NULL != att_cb) {
											attribute_list_t * al;
											for(al = sl->signal->attribute_list; al != NULL; al = al->next) {
												att_cb(entry->bus, m, sl->signal, al->attribute, arg);
											}
										}
									}
								}
							}
						} while (hashtable_iterator_advance(itr));
					}
				}
			}
		}
	}
}
