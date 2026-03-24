#kw_override BUG_ON(cond) do { if(cond) abort(); } while (0)

#kw_override __MTLK_KMALLOC(size, flags) malloc(size)
#kw_override __MTLK_KFREE(size) free(size)
