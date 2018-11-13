//fs:18h -> teb
//teb+30h -> peb
//peb+0Ch -> peb_ldr_data
//peb_ltr_data+0Ch -> mInLoadOrder[0]
//mInLoadOrder[n] -> mInLoadOrder[n+1]

#if __SIZEOF_POINTER__ == 4

#define PEB_LTR_OFFSET 12
#define LTR_LIST_OFFSET 12
#define LTR_ENTRY_NAME_OFFSET 44
#define OPT_DATADIR_OFFSET 96

#else

#define PEB_LTR_OFFSET 24
#define LTR_LIST_OFFSET 16
#define LTR_ENTRY_NAME_OFFSET 88
#define OPT_DATADIR_OFFSET 112

#endif

#define USTRING_BUFFER_OFFSET 4
#define DOS_PE_OFFSET 60
#define PE_OPT_OFFSET 20
#define PE_DATADIR_OFFSET (PE_OPT_OFFSET + OPT_DATADIR_OFFSET)

#define deref(x) (*((void **) (x)))
#define offset(x, y) (((char *) (x)) + (y))
#define doffset(x, y) deref(offset(x, y))
#define droffset(b, x, y) offset(b, *((unsigned long *) offset(x, y)))
#define pullul(x) (*((unsigned long *) (x)))
#define pullulo(x, y) pullul(((char *) (x)) + (y))

extern void *get_peb()

void *get_mTable() {
    void *p = get_peb();
    p = doffset(p, PEB_LTR_OFFSET);
    p = offset(p, LTR_LIST_OFFSET);
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
    return hashBuffer(offset(data, USTRING_BUFFER_OFFSET), *((unsigned short *) data));
}

void *getModule(unsigned long hash) { // Won't load a new module
    void *mt = get_mTable();
    void *head = mt;
    while (1) {
        mt = *((void **) mt);
        if (mt == head) break
        if (hash == hashUString(offset(mt, LTR_ENTRY_NAME_OFFSET))) {
            return mt;
        }
    }
    return NULL;
}

void *getDataDir(void *mod) {
    return offset(doffset(mod, DOS_PE_OFFSET), PE_DATADIR_OFFSET);
}

char strCmpCustom(char *a, char *b) { // 0 - equal, 1 - first is higher, 2 - second is higher
    while (1) {
        if (*a < *b) return 1;
        if (*a > *b) return 2;
        if (*a == *b) {
            if (*a == 0) return 0;
            a++;
            b++;
        }
    }
}

#define ETABLE_OBASE_OFFSET 16
#define ETABLE_NUMFUNCTS_OFFSET 20
#define getNumFuncts(x) pullulo(x, ETABLE_NUMFUNCTS_OFFSET)
#define ETABLE_NUMNAMES_OFFSET 24
#define getNumNames(x) pullulo(x, ETABLE_NUMNAMES_OFFSET)
#define ETABLE_ADDRFUNCT_OFFSET 28
#define ETABLE_ADDRNAME_OFFSET 32
#define ETABLE_ADDRONAME_OFFSET 36

unsigned long getFunctionOrdinal(void *mod, char *name) {
    void *e = getDataDir(mod);
    //unsigned long eSize = ((unsigned long *) e)[1];
    e = offset(mod, pullul(e));
    char *s;
    unsigned long start = 0;
    unsigned long end;
    if ((end = getNumNames(e)) == 0) return NULL;
    end--;
    unsigned long temp;
    char r;
    while (start != end) {
        temp = (end - start) / 2 + start;
        switch (strCmpCustom(name, ((char **) droffset(mod, e, ETABLE_ADDRNAME_OFFSET))[temp])) {
            case 0:
                return ((unsigned long *) droffset(mod, e, ETABLE_ADDRONAME_OFFSET))[temp];
            case 1:
                end = temp - 1;
                break;
            case 2:
                start = temp + 1;
                break;
        }
    }
    if (strCmpCustom(name, ((char **) droffset(mod, e, ETABLE_ADDRNAME_OFFSET))[start]) == 0) return ((unsigned long *) droffset(mod, e, ETABLE_ADDRONAME_OFFSET))[start];
    else return NULL;
}

void *getFunction(void *mod, unsigned long ordinal) { // Won't follow forwarded exports
    ordinal -= *((unsigned long *) offset(e, ETABLE_OBASE_OFFSET));
    void *e = getDataDir(mod);
    void *eLast = ((char *) e) + ((unsigned long *) e)[1] - 1;
    e = droffset(mod, e, 0);
    if (ordinal >= *((unsigned long *) offset(e, ETABLE_NUMFUNCTS_OFFSET))) return NULL;
    void *f = droffset(mod, e, ETABLE_ADDRFUNCT_OFFSET);
    if ((f < e) || (f > eLast)) return f;
    return NULL;
}
