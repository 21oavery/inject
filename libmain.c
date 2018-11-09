//fs:18h -> teb
//teb+30h -> peb
//peb+0Ch -> peb_ldr_data
//peb_ltr_data+0Ch -> mInLoadOrder[0]
//mInLoadOrder[n] -> mInLoadOrder[n+1]

#if __SIZEOF_POINTER__ == 4

#define PEB_LTR_OFFSET 0x0c
#define LTR_LIST_OFFSET 0x0c
#define LTR_ENTRY_NAME_OFFSET 0x2c

#else

#define PEB_LTR_OFFSET 0x18
#define LTR_LIST_OFFSET 0x10
#define LTR_ENTRY_NAME_OFFSET 0x58

#endif

#define USTRING_BUFFER_OFFSET 4

extern void *get_peb()

void *get_mTable() {
    void *p = get_peb();
    p = ((char *) p) + PEB_LTR_OFFSET;
    p = *((void **) p);
    p = ((char *) p) + LTR_LIST_OFFSET;
    return p;
}

unsigned long hashBuffer(char *data, unsigned int len) { //http://www.cse.yorku.ca/~oz/hash.html
    unsigned long h = 5381;
    for (unsigned int i = 0; i < len; i++) {
        h = ((h << 5) + h) ^ data[i];
    }
    return h;
}

unsigned long hashUString(void *data) {
    return hashBuffer(((char *) data) + USTRING_BUFFER_OFFSET, *((unsigned short *) data));
}

void *getModule(unsigned long hash) {
    void *mt = get_mTable();
    void *head = mt;
    while (1) {
        mt = *((void **) mt);
        if (mt == head) break
        if (hash == hashUString(((char *) mt) + LTR_ENTRY_NAME_OFFSET)) {
            return mt;
        }
    }
    return NULL;
}

