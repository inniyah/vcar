#ifndef INCLUDE_SHOWDBC_H_8AE695AA_4C11_11E5_95ED_10FEED04CD1C
#define INCLUDE_SHOWDBC_H_8AE695AA_4C11_11E5_95ED_10FEED04CD1C

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "model/dbcModel.h"

#ifdef __cplusplus
extern "C" {
#endif

void show_dbc_valtable_list(valtable_list_t *valtable_list);
void show_dbc_network(dbc_t *dbc);
void show_dbc_message_list(message_list_t *message_list);
void show_dbc_signals(dbc_t *dbc);
void show_dbc_nodes(dbc_t *dbc);
void show_dbc_envvars(envvar_list_t *envvar_list);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SHOWDBC_H_8AE695AA_4C11_11E5_95ED_10FEED04CD1C */
