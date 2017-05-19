#pragma once

int tx_insert(uint64_t xid, uint64_t scn);

int tx_remove(uint64_t xid);

int tx_add(LC_INFO* info, int commitflag);

void tx_travel(void);

void free_tx(void);
