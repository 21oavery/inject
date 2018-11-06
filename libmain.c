//fs:18h -> teb
//teb+30h -> peb
//peb+0Ch -> peb_ldr_data
//peb_ltr_data+0Ch -> mInLoadOrder[0]
//mInLoadOrder[n] -> mInLoadOrder[n+1]

#if __SIZEOF_POINTER__ == 4
#define PEB_LTR_OFFSET 0x0c
#else
#define PEB_LTR_OFFSET 0x18
#endif

#if __SIZEOF_POINTER__ == 4
#define LTR_LIST_OFFSET 0x0c
#else
#define LTR_LIST_OFFSET 0x10
#endif

extern void *get_peb()

void *get_mTable() {
    void *p = get_peb();
    p = ((char *) p) + PEB_LTR_OFFSET;
    p = *((void **) p);
    p = ((char *) p) + LTR_LIST_OFFSET;
    p = *((void **) p);
    return p;
}

unsigned long hash(char *data, unsigned int len) { //http://www.cse.yorku.ca/~oz/hash.html
    unsigned long h = 5381;
    for (unsigned int i = 0; i < len; i++) {
        h = ((h << 5) + h) ^ data[i];
    }
    return h;
}

void *getModule(unsigned long hash) {
    void *mt = get_mTable();
    mt
