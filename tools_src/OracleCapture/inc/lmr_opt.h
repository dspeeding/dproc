#pragma once

int lmr_setlog(XOCI_CTX* ctx, char* logfile, int flag);

int lmr_start(XOCI_CTX* ctx, uint64_t start_scn, uint64_t end_scn);

int lmr_end(XOCI_CTX* ctx);

int lmr_remove(XOCI_CTX* ctx);

