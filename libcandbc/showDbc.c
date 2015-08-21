/*  dbcls -- list contents of a DBC file 
    Copyright (C) 2007-2011 Andreas Heitmann

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "showDbc.h"
#include "model/dbcModel.h"
#include "model/dbcReader.h"

#include <stdio.h>
#include <string.h>
#include <libgen.h>

static const char * decode_mux_type(mux_t mux_type) {
	const char * text;
	switch(mux_type) {
		case m_signal:
			text = "Signal";
			break;
		case m_multiplexor:
			text = "Multiplexor";
			break;
		case m_multiplexed:
			text = "Multiplexed";
			break;
		default:
			text = "unknown";
			break;
	}
	return text;
}

static const char * decode_signal_val_type(signal_val_type_t signal_val_type) {
	const char * text;
	switch(signal_val_type) {
		case svt_integer:
			text = "integer";
			break;
		case svt_float:
			text = "float";
			break;
		case svt_double:
			text = "double";
			break;
		default:
			text = "unknown";
			break;
	}
	return text;
}

static void show_attribute(attribute_t *a) {
	if(a->name) {
		printf("  %s = ",a->name);
	} else {
		printf("  (undef) = ");
	}

	switch(a->value->value_type) {
		case vt_integer:
			printf("%ld (INT)",a->value->value.int_val);
			break;
		case vt_float:
			printf("%f (DOUBLE)",a->value->value.double_val);
			break;
		case vt_string:
			printf("\"%s\" (STRING)",a->value->value.string_val);
			break;
		case vt_enum:
			printf("\"%s\" (ENUM)",a->value->value.enum_val);
			break;
		case vt_hex:
			printf("0x%lx (HEX)",a->value->value.hex_val);
			break;
		default:
			printf(" (UNKNOWN)");
	}
}

static void show_string_list(string_list_t *string_list) {
	while(string_list) {
		fputs(string_list->string, stdout);
		if(string_list->next != NULL) {
			putchar(',');
		}
		string_list = string_list->next;
	}
}

static void show_attribute_list(attribute_list_t *al) {
	if(al != NULL) {
		for(al = al; al != NULL; al = al->next) {
			if(al->attribute != NULL) {
				show_attribute(al->attribute);
			}
		}
	}
}

static void show_val_map(val_map_t *vm) {
	for(; vm!=NULL; vm=vm->next) {
		printf("%lu=\"%s\"", vm->val_map_entry->index, vm->val_map_entry->value);
		if(vm->next != NULL) {
			putchar(',');
		}
	}
}

void show_dbc_valtable_list(valtable_list_t *valtable_list) {
	for(; valtable_list!=NULL; valtable_list=valtable_list->next) {
		printf("%s;\"%s\";",
			valtable_list->valtable->name,
			valtable_list->valtable->comment ? valtable_list->valtable->comment : ""
		);
		show_val_map(valtable_list->valtable->val_map);
	}
}

void show_dbc_network(dbc_t *dbc) {
	/*
	* network name:
	* - value of attribute "DBName" if present
	* - filename otherwise (basename w/o extension)
	*/
	string_t network_name;

	network_name = basename(dbc->filename);
	strtok(network_name,".");
	printf("network: %s\n",network_name);

	if(dbc->network) {
		if(dbc->network->comment) {
			printf("comment: \"%s\"\n",dbc->network->comment);
		}
		show_attribute_list(dbc->network->attribute_list);
	}
}

static void show_message_header(void) {
	fputs(
			"message_id;"
			"message_name;"
			"message_len;"
			"message_sender;"
			"message_comment;"
			"message_attribute_list;"
			"message_transmitter_list;",
		stdout );
}

static void show_signal_header(void) {
	fputs(
			"signal_name;"
			"signal_mux_type;"
			"signal_mux_value;"
			"signal_bit_start;"
			"signal_bit_len;"
			"signal_endianess;"
			"signal_signedness;"
			"signal_scale;"
			"signal_offset;"
			"signal_min;"
			"signal_max;"
			"signal_val_type;"
			"signal_unit;"
			"signal_receiver_list;"
			"signal_comment;"
			"signal_attribute_list;"
			"signal_val_map\n",
		stdout );
}

static void show_signal(signal_list_t *sl) {
	printf(
			"%s;"       /* signal name */
			"%s;"       /* mux type */
			"%ld;"      /* mux value */
			"%d;"       /* bit start */
			"%d;"       /* bit len */
			"%d;"       /* endianess */
			"%d;"       /* signedness */
			"%f;"       /* scale */
			"%f;"       /* offset */
			"%f;"       /* min */
			"%f;"       /* max */
			"%s;"       /* signal val type */
			"\"%s\";"   /* unit */             
		,
		/* Signal */
		sl->signal->name,
		decode_mux_type(sl->signal->mux_type),
		sl->signal->mux_value,
		sl->signal->bit_start,
		sl->signal->bit_len,
		sl->signal->endianess,
		sl->signal->signedness,
		sl->signal->scale,
		sl->signal->offset,
		sl->signal->min,
		sl->signal->max,
		decode_signal_val_type(sl->signal->signal_val_type),
		sl->signal->unit?sl->signal->unit:""
	);

	show_string_list(sl->signal->receiver_list);
	putchar(';');
	printf("\"%s\";",sl->signal->comment?sl->signal->comment:"");
	show_attribute_list(sl->signal->attribute_list);
	putchar(';');
	show_val_map(sl->signal->val_map);
	putchar('\n');
}

static void show_string(string_t string) {
	if(string) {
		printf("\"%s\"", string);
	}
}

#if 0
static void show_message_old(message_t *message) {
	printf(
			"$%lX;%s;%d;%s",
		message->id,
		message->name,
		message->len,
		message->sender
	);
	putchar(';');
	show_string(message->comment);
	putchar(';');
	show_string_list(message->transmitter_list);
}
#endif

static void show_message(message_list_t *ml) {
	printf(
			"$%lX;"     /* message id */
			"%s;"       /* message name */
			"%d;"       /* message len */
			"%s"        /* sender */
		,
		ml->message->id,
		ml->message->name,
		ml->message->len,
		ml->message->sender
	);

	putchar(';');
	show_string(ml->message->comment);
	putchar(';');
	show_attribute_list(ml->message->attribute_list);
	putchar(';');
	show_string_list(ml->message->transmitter_list);
}

void show_dbc_message_list(message_list_t *message_list) {
	while(message_list != NULL) {
		show_message(message_list);
		putchar('\n');
		message_list = message_list->next;
	}
}

void show_dbc_signals(dbc_t *dbc) {
	message_list_t *ml;
	signal_list_t *sl;

	show_message_header();
	show_signal_header();

	for(ml = dbc->message_list; ml != NULL; ml = ml->next) {
		for(sl = ml->message->signal_list; sl != NULL; sl = sl->next) {
			show_message(ml);
			putchar(';');
			show_signal(sl);
		}
	}
}

void show_dbc_nodes(dbc_t *dbc) {
	node_list_t *nl;

	for (nl = dbc->node_list; nl != NULL; nl = nl->next) {
		printf("%s", nl->node->name);
		if (nl->node->comment) {
			printf(";\"%s\"", nl->node->comment);
		}
		putchar('\n');
		show_attribute_list(nl->node->attribute_list);
	}
}

void show_dbc_envvars(envvar_list_t *envvar_list) {
	envvar_list_t *el;

	for (el = envvar_list; el != NULL; el = el->next) {
		const char *string_from_et[] = {
			"INTEGER",
			"FLOAT",
			"STRING",
			"DATA",
		};
		const char *string_from_at[] = {
			"UNRESTRICTED",
			"READONLY",
			"WRITEONLY",
			"READWRITE",
	};

		printf(
				"\"%s\";%s;%s;%ld;%ld;\"%s\";%ld;%ld;",
			el->envvar->name,
			string_from_et[(int)el->envvar->envtype],
			string_from_at[(int)el->envvar->access],
			el->envvar->min,
			el->envvar->max,
			el->envvar->unit,
			el->envvar->initial,
			el->envvar->index
		);

		/* print node list */
		show_string_list(el->envvar->node_list);
		printf(";");

		/* print value map */
		show_val_map(el->envvar->val_map);
		printf("\n");
	}
}

