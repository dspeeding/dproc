#include "basetype.h"
#include "Lhash.h"

void lh_stats(IN _LHASH *lh, IN FILE *out)
{
    fprintf(out, "num_items             = %lu\n", lh->num_items);
    fprintf(out, "num_nodes             = %u\n", lh->num_nodes);
    fprintf(out, "num_alloc_nodes       = %u\n", lh->num_alloc_nodes);
    fprintf(out, "num_expands           = %lu\n", lh->num_expands);
    fprintf(out, "num_expand_reallocs   = %lu\n", lh->num_expand_reallocs);
    fprintf(out, "num_contracts         = %lu\n", lh->num_contracts);
    fprintf(out, "num_contract_reallocs = %lu\n", lh->num_contract_reallocs);
    fprintf(out, "num_hash_calls        = %lu\n", lh->num_hash_calls);
    fprintf(out, "num_comp_calls        = %lu\n", lh->num_comp_calls);
    fprintf(out, "num_insert            = %lu\n", lh->num_insert);
    fprintf(out, "num_replace           = %lu\n", lh->num_replace);
    fprintf(out, "num_delete            = %lu\n", lh->num_delete);
    fprintf(out, "num_no_delete         = %lu\n", lh->num_no_delete);
    fprintf(out, "num_retrieve          = %lu\n", lh->num_retrieve);
    fprintf(out, "num_retrieve_miss     = %lu\n", lh->num_retrieve_miss);
    fprintf(out, "num_hash_comps        = %lu\n", lh->num_hash_comps);
    fprintf(out, "p                     = %u\n", lh->p);
    fprintf(out, "pmax                  = %u\n", lh->pmax);
    fprintf(out, "up_load               = %lu\n", lh->up_load);
    fprintf(out, "down_load             = %lu\n", lh->down_load);
}

void lh_node_stats(IN _LHASH *lh, IN FILE *out)
{
    LHASH_NODE *n;
    unsigned int i, num;

    for (i = 0; i < lh->num_nodes; i++) {
        for (n = lh->b[i], num = 0; n != NULL; n = n->next)
            num++;
        fprintf(out, "node %6u -> %3u\n", i, num);
    }
}

void lh_node_usage_stats(IN _LHASH *lh, IN FILE *out)
{
    LHASH_NODE *n;
    unsigned long num;
    unsigned int i;
    unsigned long total = 0, n_used = 0;

    for (i = 0; i < lh->num_nodes; i++) {
        for (n = lh->b[i], num = 0; n != NULL; n = n->next)
            num++;
        if (num != 0) {
            n_used++;
            total += num;
        }
    }
    fprintf(out, "%lu nodes used out of %u\n", n_used, lh->num_nodes);
    fprintf(out, "%lu items\n", total);
    if (n_used == 0)
        return;
    fprintf(out, "load %d.%02d  actual load %d.%02d\n",
            (int)(total / lh->num_nodes),
            (int)((total % lh->num_nodes) * 100 / lh->num_nodes),
            (int)(total / n_used), (int)((total % n_used) * 100 / n_used));
}
