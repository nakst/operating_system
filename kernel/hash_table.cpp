template <class T>
struct HashedItem {
	T *next;
	T **reference;
	UniqueIdentifier *hash;
	T *thisItem;
};

template <class T>
struct HashTable {
#define HASH_TABLE_BITS (12)
	T *table[1 << HASH_TABLE_BITS];
};
