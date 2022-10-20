#ifdef DEFINE_HASHTABLE
struct ht_t *ht;
#else
extern struct ht_t *ht;
#endif

#define TABLE_SIZE 100000