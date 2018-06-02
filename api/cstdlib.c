// TODO Separate this into another library from the API.

int rename(const char *oldPath, const char *newPath) {
	size_t oldPathBytes = OSCStringLength(oldPath);
	const char *newFileName = newPath + OSCStringLength(newPath);

	while (true) {
		newFileName--;
		if (*newFileName == '/') {
			newFileName++;
			break;
		}
	}

	size_t newFileNameBytes = OSCStringLength(newFileName);
	size_t newPathBytes = OSCStringLength(newPath) - newFileNameBytes;

	OSNodeInformation oldNode, newDirectory;

	if (OSOpenNode((char *) oldPath, oldPathBytes, OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &oldNode) != OS_SUCCESS) {
		return -1;
	}

	if (OSOpenNode((char *) newPath, newPathBytes, OS_OPEN_NODE_FAIL_IF_NOT_FOUND | OS_OPEN_NODE_DIRECTORY, &newDirectory) != OS_SUCCESS) {
		OSCloseHandle(oldNode.handle);
		return -1;
	}

	if (OSMoveNode(oldNode.handle, newDirectory.handle, (char *) newFileName, newFileNameBytes) != OS_SUCCESS) {
		OSCloseHandle(oldNode.handle);
		OSCloseHandle(newDirectory.handle);
		return -1;
	}

	OSCloseHandle(oldNode.handle);
	OSCloseHandle(newDirectory.handle);
	return 0;
}

#if 0
int errno; // TODO Thread-local storage.

FILE *stdin;
FILE *stdout;
FILE *stderr;

void *memset(void *s, int c, size_t n) {
	uint8_t *s8 = (uint8_t *) s;
	for (uintptr_t i = 0; i < n; i++) {
		s8[i] = (uint8_t) c;
	}
	return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
	uint8_t *dest8 = (uint8_t *) dest;
	const uint8_t *src8 = (const uint8_t *) src;
	for (uintptr_t i = 0; i < n; i++) {
		dest8[i] = src8[i];
	}
	return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
	if ((uintptr_t) dest < (uintptr_t) src) {
		return memcpy(dest, src, n);
	} else {
		uint8_t *dest8 = (uint8_t *) dest;
		const uint8_t *src8 = (const uint8_t *) src;
		for (uintptr_t i = n; i; i--) {
			dest8[i - 1] = src8[i - 1];
		}
		return dest;
	}
}

size_t strlen(const char *s) {
	size_t n = 0;
	while (s[n]) n++;
	return n;
}

size_t strnlen(const char *s, size_t maxlen) {
	size_t n = 0;
	while (s[n] && maxlen--) n++;
	return n;
}

void *malloc(size_t size) {
	void *x = OSHeapAllocate(size, false);
	return x;
}

void *calloc(size_t num, size_t size) {
	return OSHeapAllocate(num * size, true);
}

void free(void *ptr) {
	OSHeapFree(ptr);
}

int abs(int n) {
	if (n < 0)	return 0 - n;
	else		return n;
}

void *realloc(void *ptr, size_t size) {
	if (!ptr) return malloc(size);

	size_t oldSize = ((OSHeapRegion *) ((uint8_t *) ptr - 0x10))->size;
	
	if (!oldSize) {
		oldSize = ((OSHeapRegion *) ((uint8_t *) ptr - 0x10))->largeRegionSize;
	} else {
		oldSize -= 0x10;
	}

	void *newBlock = malloc(size);
	if (!newBlock) return nullptr;
	memcpy(newBlock, ptr, oldSize > size ? size : oldSize);
	free(ptr);
	return newBlock;
}

char *getenv(const char *name) {
	(void) name;
	return nullptr;
}

int toupper(int c) {
	if (c >= 'a' && c <= 'z') {
		return c - 'a' + 'z';
	} else {
		return c;
	}
}

int tolower(int c) {
	if (c >= 'A' && c <= 'Z') {
		return c - 'A' + 'a';
	} else {
		return c;
	}
}

int strcasecmp(const char *s1, const char *s2) {
	while (true) {
		if (*s1 != *s2 && tolower(*s1) != tolower(*s2)) {
			if (*s1 == 0) return -1;
			else if (*s2 == 0) return 1;
			return *s1 - *s2;
		}

		if (*s1 == 0) {
			return 0;
		}

		s1++;
		s2++;
	}
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2 && tolower(*s1) != tolower(*s2)) {
			if (*s1 == 0) return -1;
			else if (*s2 == 0) return 1;
			return *s1 - *s2;
		}

		if (*s1 == 0) {
			return 0;
		}

		s1++;
		s2++;
	}

	return 0;
}

int strcmp(const char *s1, const char *s2) {
	while (true) {
		if (*s1 != *s2) {
			if (*s1 == 0) return -1;
			else if (*s2 == 0) return 1;
			return *s1 - *s2;
		}

		if (*s1 == 0) {
			return 0;
		}

		s1++;
		s2++;
	}
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			if (*s1 == 0) return -1;
			else if (*s2 == 0) return 1;
			return *s1 - *s2;
		}

		if (*s1 == 0) {
			return 0;
		}

		s1++;
		s2++;
	}

	return 0;
}

int isspace(int c) {
	if (c == ' ')  return 1;
	if (c == '\f') return 1;
	if (c == '\n') return 1;
	if (c == '\r') return 1;
	if (c == '\t') return 1;
	if (c == '\v') return 1;

	return 0;
}

unsigned long int strtoul(const char *nptr, char **endptr, int base) {
	// TODO errno

	if (base > 36) return 0;

	while (isspace(*nptr)) {
		nptr++;
	}

	if (*nptr == '+') {
		nptr++;
	} else if (*nptr == '-') {
		nptr++;
	}

	if (base == 0) {
		if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
			base = 16;
			nptr += 2;
		} else if (nptr[0] == '0') {
			CF(Print)("WARNING: strtoul with base=0, detected octal\n");
			base = 8; // Why?!?
			nptr++;
		} else {
			base = 10;
		}
	}

	uint64_t value = 0;
	bool overflow = false;

	while (true) {
		int64_t digit = ConvertCharacterToDigit(*nptr, base);

		if (digit != -1) {
			nptr++;

			uint64_t x = value;
			value *= base;
			value += (uint64_t) digit;

			if (value / base != x) {
				overflow = true;
			}
		} else {
			break;
		}
	}

	if (overflow) {
		value = ULONG_MAX;
	}

	if (endptr) {
		*endptr = (char *) nptr;
	}

	return value;
}

char *strsep(char **stringp, const char *delim) {
	char *string = *stringp;

	if (!string) {
		return NULL;
	}

	size_t tokenLength = strcspn(string, delim);

	if (string[tokenLength] == 0) {
		*stringp = NULL;
	} else {
		string[tokenLength] = 0;
		*stringp = string + tokenLength + 1;
	}

	return string;
}

char *strcat(char *dest, const char *src) {
	char *o = dest;
	dest += strlen(dest);

	while (*src) {
		*dest = *src;
		src++;
		dest++;
	}

	*dest = 0;

	return o;
}

int atoi(const char *nptr) {
	return (int) strtol(nptr, NULL, 10);
}

int getpagesize() {
	return OS_PAGE_SIZE;
}

long int strtol(const char *nptr, char **endptr, int base) {
	// TODO errno

	if (base > 36) return 0;

	while (isspace(*nptr)) {
		nptr++;
	}

	bool positive = true;

	if (*nptr == '+') {
		positive = true;
		nptr++;
	} else if (*nptr == '-') {
		positive = false;
		nptr++;
	}

	if (base == 0) {
		if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
			base = 16;
			nptr += 2;
		} else if (nptr[0] == '0') {
			CF(Print)("WARNING: strtol with base=0, detected octal\n");
			base = 8; // Why?!?
			nptr++;
		} else {
			base = 10;
		}
	}

	int64_t value = 0;
	bool overflow = false;

	while (true) {
		int64_t digit = ConvertCharacterToDigit(*nptr, base);

		if (digit != -1) {
			nptr++;

			int64_t x = value;
			value *= base;
			value += digit;

			if (value / base != x) {
				overflow = true;
			}
		} else {
			break;
		}
	}

	if (!positive) {
		value = -value;
	}

	if (overflow) {
		value = positive ? LONG_MAX : LONG_MIN;
	}

	if (endptr) {
		*endptr = (char *) nptr;
	}

	return value;
}

char *strstr(const char *haystack, const char *needle) {
	size_t haystackLength = strlen(haystack);
	size_t needleLength = strlen(needle);

	if (haystackLength < needleLength) {
		return nullptr;
	}

	for (uintptr_t i = 0; i <= haystackLength - needleLength; i++) {
		for (uintptr_t j = 0; j < needleLength; j++) {
			if (haystack[i + j] != needle[j]) {
				goto tryNext;
			}
		}

		return (char *) haystack + i;

		tryNext:;
	}

	return nullptr;
}

void qsort(void *_base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	if (nmemb <= 1) return;

	uint8_t *base = (uint8_t *) _base;
	uint8_t swap[size];

	intptr_t i = -1, j = nmemb;

	while (true) {
		while (compar(base + ++i * size, base) < 0);
		while (compar(base + --j * size, base) > 0);

		if (i >= j) break;

		memcpy(swap, base + i * size, size);
		memcpy(base + i * size, base + j * size, size);
		memcpy(base + j * size, swap, size);
	}

	qsort(base, ++j, size, compar);
	qsort(base + j * size, nmemb - j, size, compar);
}

char *strcpy(char *dest, const char *src) {
	size_t stringLength = strlen(src);
	memcpy(dest, src, stringLength + 1);
	return dest;
}

char *stpcpy(char *dest, const char *src) {
	size_t stringLength = strlen(src);
	memcpy(dest, src, stringLength + 1);
	return dest + stringLength;
}

size_t strspn(const char *s, const char *accept) {
	size_t count = 0;

	while (true) {
		char character = *s;

		const char *search = accept;

		while (true) {
			char c = *search;

			if (!c) {
				break;
			} else if (character == c) {
				goto match;
			}

			search++;
		}

		return count;

		match:;
		count++;
		s++;
	}
}

size_t strcspn(const char *s, const char *reject) {
	size_t count = 0;

	while (true) {
		char character = *s;
		if (!character) return count;

		const char *search = reject;

		while (true) {
			char c = *search;

			if (!c) {
				goto match;
			} else if (character == c) {
				break;
			}

			search++;
		}

		return count;

		match:;
		count++;
		s++;
	}
}

char *strrchr(const char *s, int c) {
	const char *start = s;
	if (!s[0]) return NULL;
	s += strlen(s) - 1;

	while (true) {
		if (*s == c) {
			return (char *) s;
		}

		if (s == start) {
			return NULL;
		}

		s--;
	}
}

char *strchr(const char *s, int c) {
	while (true) {
		if (*s == c) {
			return (char *) s;
		}

		if (*s == 0) {
			return NULL;
		}

		s++;
	}
}

char *strncpy(char *dest, const char *src, size_t n) {
	size_t i;

	for (i = 0; i < n && src[i]; i++) {
		dest[i] = src[i];
	}

	for (; i < n; i++) {
		dest[i] = 0;
	}

	return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	return CF(CompareBytes)((void *) s1, (void *) s2, n);
}

void *memchr(const void *_s, int _c, size_t n) {
	uint8_t *s = (uint8_t *) _s;
	uint8_t c = (uint8_t) _c;

	for (uintptr_t i = 0; i < n; i++) {
		if (s[i] == c) {
			return s + i;
		}
	}

	return nullptr;
}

int isalpha(int c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(int c) {
	return (c >= '0' && c <= '9');
}

struct lconv *localeconv() {
	static lconv l;
	l.decimal_point = (char *) ".";
	l.thousands_sep = (char *) "";
	l.grouping = (char *) "";
	l.int_curr_symbol = (char *) "";
	l.currency_symbol = (char *) "";
	l.mon_decimal_point = (char *) "";
	l.mon_thousands_sep = (char *) "";
	l.mon_grouping = (char *) "";
	l.positive_sign = (char *) "";
	l.negative_sign = (char *) "";
	l.frac_digits = CHAR_MAX;
	l.p_cs_precedes = CHAR_MAX;
	l.n_cs_precedes = CHAR_MAX;
	l.p_sep_by_space = CHAR_MAX;
	l.n_sep_by_space = CHAR_MAX;
	l.p_sign_posn = CHAR_MAX;
	l.n_sign_posn = CHAR_MAX;
	l.int_frac_digits = CHAR_MAX;
	l.int_p_cs_precedes = CHAR_MAX;
	l.int_n_cs_precedes = CHAR_MAX;
	l.int_p_sep_by_space = CHAR_MAX;
	l.int_n_sep_by_space = CHAR_MAX;
	l.int_p_sign_posn = CHAR_MAX;
	l.int_n_sign_posn = CHAR_MAX;
	return &l;
}

int rand() {
	uint8_t a = OSGetRandomByte();
	uint8_t b = OSGetRandomByte();
	uint8_t c = OSGetRandomByte();
	return (a << 16) | (b << 8) | (c << 0);
}

int vsnprintf(char *str, size_t size, const char *format, va_list arguments) {
	int x = stbsp_vsnprintf(str, size, format, arguments );
	return x;
}

int snprintf(char *str, size_t size, const char *format, ...) {
	va_list arguments;
	va_start(arguments, format);
	int x = stbsp_vsnprintf(str, size, format, arguments );
	va_end(arguments);
	return x;
}

int sprintf(char *str, const char *format, ...) {
	va_list arguments;
	va_start(arguments, format);
	int x = stbsp_vsnprintf(str, 65536, format, arguments );
	va_end(arguments);
	return x;
}

int printf(const char *format, ...) {
	char *buffer = (char *) OSHeapAllocate(65536, false);
	va_list arguments;
	va_start(arguments, format);
	int x = stbsp_vsnprintf(buffer, 65536, format, arguments );
	OSPrintDirect(buffer, x);
	va_end(arguments);
	OSHeapFree(buffer);
	return x;
}

int isalnum(int c) {
	return isalpha(c) || isdigit(c);
}

int iscntrl(int c) {
	return c < 0x20 || c == 0x7F;
}

int isgraph(int c) {
	return c > ' ' && c < 0x7F;
}

int islower(int c) {
	return c >= 'a' && c <= 'z';
}

int isprint(int c) {
	return c >= ' ' && c < 127;
}

int ispunct(int c) {
	return c != ' ' && !isalnum(c);
}

int isupper(int c) {
	return c >= 'A' && c <= 'Z';
}

int isxdigit(int c) {
	return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

char *setlocale(int category, const char *locale) {
	(void) category;
	(void) locale;
	return nullptr;
}

int system(const char *command) {
	OSExecuteProgram(command, OSCStringLength(command));
	return 0;
}

void srand(unsigned int seed) {
	osRandomByteSeed = seed;
}

void (*signal(int sig, void (*func)(int)))(int) {
	(void) sig;
	(void) func;
	return nullptr;
}

void exit(int status) {
	(void) status;
	OSTerminateProcess(OS_CURRENT_PROCESS);
}

void abort() {
	OSCrashProcess(OS_ERROR_UNKNOWN_OPERATION_FAILURE);
	while (true);
}

int strcoll(const char *s1, const char *s2) {
	return OSCompareStrings((char *) s1, (char *) s2, OSCStringLength(s1), OSCStringLength(s2));
}

char *strerror(int errnum) {
	(void) errnum;
	return (char*) "unknown operation failure";
}

char *strpbrk(const char *s, const char *accept) {
	size_t l1 = OSCStringLength(s), l2 = OSCStringLength(accept);

	for (uintptr_t i = 0; i <  l1; i++) {
		char c = s[i];

		for (uintptr_t j = 0; j < l2; j++) {
			if (accept[j] == c) {
				return (char *) (i + s);
			}
		}
	}

	return nullptr;
}

int remove(const char *path) {
	OSNodeInformation node;
	OSError error = OSOpenNode((char *) path, OSCStringLength(path), OS_OPEN_NODE_RESIZE_ACCESS | OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node);
	if (error != OS_SUCCESS) return -1;
	error = OSDeleteNode(node.handle); 
	return error == OS_SUCCESS ? 0 : -1;
}

static bool OpenStream(const char *path, const char *_mode, FILE *stream) {
	uint64_t flags = 0;

#define FOPEN_MODE_R  (0)
#define FOPEN_MODE_R2 (1)
#define FOPEN_MODE_W  (2)
#define FOPEN_MODE_W2 (3)
#define FOPEN_MODE_A  (4)
#define FOPEN_MODE_A2 (5)
	int mode = 0;

	size_t modeLength = strlen(_mode);
	for (uintptr_t i = 0; i < modeLength; i++) {
		char c = _mode[i];

		switch (c) {
			case 'r': { mode += FOPEN_MODE_R; } break;
			case 'w': { mode += FOPEN_MODE_W; } break;
			case 'a': { mode += FOPEN_MODE_A; } break;
			case '+': { mode += 1; } break;
		}
	}

	bool clearFile = false;
	bool seekToEndBeforeWrites = false;

	switch (mode) {
		case FOPEN_MODE_R: {
			flags = OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_FAIL_IF_NOT_FOUND;
		} break;

		case FOPEN_MODE_R2: {
			flags = OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_FAIL_IF_NOT_FOUND | OS_OPEN_NODE_RESIZE_ACCESS;
		} break;

		case FOPEN_MODE_W: {
			flags = OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS;
			clearFile = true;
		} break;

		case FOPEN_MODE_W2: {
			flags = OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS;
			clearFile = true;
		} break;

		case FOPEN_MODE_A: {
			flags = OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS;
			seekToEndBeforeWrites = true;
		} break;

		case FOPEN_MODE_A2: {
			flags = OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS;
			seekToEndBeforeWrites = true;
		} break;
	}

	OSNodeInformation _node;
	OSError error = OSOpenNode((char *) path, OSCStringLength(path), flags, &_node);

	if (error != OS_SUCCESS) {
		return false;
	}

	if (clearFile) {
		error = OSResizeFile(_node.handle, 0); 

		if (error != OS_SUCCESS) {
			OSCloseHandle(_node.handle);
			return false;
		}
	}

	FILE *file = stream;
	memset(file, 0, sizeof(FILE));
	file->node = _node;
	file->ungotc = -1;
	file->seekToEndBeforeWrites = seekToEndBeforeWrites;

	return true;
}

FILE *fopen(const char *path, const char *_mode) {
	FILE *stream = (FILE *) OSHeapAllocate(sizeof(FILE), true);

	if (!OpenStream(path, _mode, stream)) {
		OSHeapFree(stream);
		return nullptr;
	}

	return stream;
}

static void CloseStream(FILE *stream) {
	if (stream->virtualFile) return;
	fflush(stream);
	if (stream->temporary) OSDeleteNode(stream->node.handle);
	OSCloseHandle(stream->node.handle);
}

int fclose(FILE *stream) {
	CloseStream(stream);
	OSHeapFree(stream);
	return 0;
}

int feof(FILE *stream) {
	return stream->endOfFile;
}

int ferror(FILE *stream) {
	return stream->error;
}

void clearerr(FILE *stream) {
	stream->endOfFile = false;
	stream->error = false;
}

FILE *tmpfile() {
	char r[L_tmpnam + 1];
	r[L_tmpnam] = 0;
	r[0] = '/';
	r[1] = '_';

	for (uintptr_t i = 2; i < L_tmpnam; i++) {
		r[i] = (rand() % 26) + 'A';
	}

	FILE *f = fopen(r, "w+");
	f->temporary = true;
	return f;
}

size_t fread(void *_ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t per = size;

	if (stream->virtualFile) return 0;

	char *ptr = (char *) _ptr;
	size *= nmemb;

	if (stream->ungotc != -1) {
		*ptr = stream->ungotc;
		stream->ungotc = -1;
		size--;
		ptr++;
	}

	int64_t result = (int64_t) OSReadFileSync(stream->node.handle, stream->position, size, ptr);

	if (result >= 0) {
		if (result != (int64_t) size) {
			stream->endOfFile = true;
		}

		stream->position += size;

		return result / per;
	} else {
		stream->error = true;
		return 0;
	}
}

int fputs(const char *s, FILE *stream) {
	if (fwrite(s, 1, OSCStringLength(s), stream)) {
		return 1;
	} else {
		return EOF;
	}
}

int fputc(int c, FILE *stream) {
	char c2 = c;
	if (fwrite(&c2, 1, 1, stream)) {
		return c;
	} else {
		return EOF;
	}
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t per = size;

	if (stream->virtualFile) {
		OSPrintDirect((char *) ptr, size * nmemb);
		return nmemb;
	}

	if (stream->seekToEndBeforeWrites) {
		OSRefreshNodeInformation(&stream->node);
		stream->position = stream->node.fileSize;
	}

	int64_t result = (int64_t) OSWriteFileSync(stream->node.handle, stream->position, size * nmemb, (void *) ptr);

	if (result >= 0) {
		if (result != (int64_t) size) {
			stream->error = true;
		}

		stream->position += result;

		return result / per;
	} else {
		stream->error = true;
		return 0;
	}
}

int fseeko(FILE *stream, off_t offset, int whence) {
	if (stream->virtualFile) return -1;
	int64_t position = 0;

	if (whence == SEEK_CUR) {
		position = stream->position;
	} else if (whence == SEEK_END) {
		OSRefreshNodeInformation(&stream->node);
		position = stream->node.fileSize;
	}

	position += offset;
	stream->position = position;

	return 0;
}

int fileno(FILE *stream) {
	return (int) stream->node.handle;
}

int ftruncate(int fildes, off_t length) {
	OSHandle handle = (OSHandle) fildes;

	if (OSResizeFile(handle, length) == OS_SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}

int fstat(int fildes, struct stat *buffer) {
	OSHandle handle = (OSHandle) fildes;
	OSNodeInformation node;
	node.handle = handle;
	OSRefreshNodeInformation(&node);

	buffer->st_dev = 0;
	buffer->st_ino = 0;
	buffer->st_mode = 0;
	buffer->st_nlink = 1;
	buffer->st_uid = 0;
	buffer->st_gid = 0;
	buffer->st_rdev = 0;
	buffer->st_size = node.fileSize;
	buffer->st_blksize = 512;
	buffer->st_blocks = node.fileSize / 512 + 1;
	buffer->st_atime = 0;
	buffer->st_mtime = 0;
	buffer->st_ctime = 0;

	return 0;
}

int stat(const char *path, struct stat *buffer) {
	OSNodeInformation node;

	if (OSOpenNode((char *) path, OSCStringLength(path), OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node) != OS_SUCCESS) {
		return -1;
	}

	int result = fstat(node.handle, buffer);
	OSCloseHandle(node.handle);
	return result;
}

int access(const char *path, int amode) {
	(void) path;
	(void) amode;
	return 0;
}

int fseek(FILE *stream, long offset, int whence) {
	return fseeko(stream, offset, whence);
}

long ftell(FILE *stream) {
	return (long) ftello(stream);
}

off_t ftello(FILE *stream) {
	return stream->position;
}

int fflush(FILE *stream) {
	(void) stream;
	// TODO.
	return 0;
}

FILE *freopen(const char *path, const char *_mode, FILE *stream) {
	CloseStream(stream);

	if (!OpenStream(path, _mode, stream)) {
		OSHeapFree(stream);
		return nullptr;
	}

	return stream;
}

int fgetc(FILE *stream) {
	char c;

	if (fread(&c, 1, 1, stream)) {
		return c;
	} else {
		return EOF;
	}
}

int ungetc(int c, FILE *stream) {
	stream->ungotc = c;
	return c;
}

char *fgets(char *s, int size, FILE *stream) {
	s[size - 1] = 0;

	int64_t read = fread(s, 1, size - 1, stream);

	if (!read) {
		return nullptr;
	}

	stream->position -= read;
	s[read] = 0;

	for (intptr_t i = 0; i < read && i < size - 1; i++) {
		stream->position++;

		if (s[i] == '\n') {
			s[i + 1] = 0;
			break;
		}
	}

	return s;
}

static char *FormatPrintCallback(char *buffer, void *user, int length) {
	FILE *stream = (FILE *) user;

	if (fwrite(buffer, 1, length, stream)) {
		return buffer;
	} else {
		return 0;
	}
}

int fprintf(FILE *stream, const char *format, ...) {
	char buffer[STB_SPRINTF_MIN];
	va_list arguments;
	va_start(arguments, format);
	int x = stbsp_vsprintfcb(FormatPrintCallback, stream, buffer, format, arguments);
	va_end(arguments);
	return x;
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	(void) stream;
	(void) buf;
	(void) mode;
	(void) size;
	return -1;
}

char *tmpnam(char *s) {
	(void) s;
	return nullptr;
}

double strtod(const char *nptr, char **endptr) {
	// TODO errno
	double result = 0;

	while (isspace(*nptr)) nptr++;
	bool neg = false, hex = false;
	if (*nptr == '+') nptr++;
	else if (*nptr == '-') { neg = true; nptr++; }
	if (*nptr == '0' && tolower(nptr[1]) == 'x') { hex = true; nptr += 2; }

	if (tolower(nptr[0]) == 'i' && tolower(nptr[1]) == 'n' && tolower(nptr[2]) == 'f') {
		nptr += 3;

		if (tolower(nptr[0]) == 'i' && tolower(nptr[1]) == 'n' && tolower(nptr[2]) == 'i' && tolower(nptr[3]) == 't' && tolower(nptr[4]) == 'y') {
			nptr += 5;
		}

		result = neg ? -INFINITY : INFINITY;
		goto done;
	}

	if (tolower(nptr[0]) == 'n' && tolower(nptr[1]) == 'a' && tolower(nptr[2]) == 'n') {
		nptr += 3;
		result = NAN;
		goto done;
	}

	while ((hex && isxdigit(*nptr)) || isdigit(*nptr)) {
		result *= hex ? 16 : 10;
		result += isdigit(*nptr) ? *nptr - '0' : (isupper(*nptr) ? 10 + *nptr - 'A' : 10 + *nptr - 'a');
		nptr++;
	}

	if (*nptr == '.') {
		nptr++;
		double fraction = 0, reduce = 1;

		while ((hex && isxdigit(*nptr)) || isdigit(*nptr)) {
			fraction *= hex ? 16 : 10;
			fraction += isdigit(*nptr) ? *nptr - '0' : (isupper(*nptr) ? 10 + *nptr - 'A' : 10 + *nptr - 'a');
			reduce *= hex ? 16 : 10;
			nptr++;
		}

		result += fraction / reduce;
	}

	if ((hex && tolower(*nptr) == 'p') || (!hex && tolower(*nptr) == 'e')) {
		nptr++;
		long int exponent = strtol(nptr, (char **) &nptr, hex ? 2 : 10);
		result *= pow(hex ? 2 : 10, exponent);
	}

	done:;
	if (endptr) *endptr = (char *) nptr;
	return result;
}

int puts(const char *string) {
	printf("%s\n", string);
	return 1;
}

void perror(const char *string) {
	if (!string) string = "";
	printf("perror: %s\n", string);
}

// ----------- Time function stubs vvvvvvv

clock_t clock() {
	return (clock_t) -1;
}

double difftime(time_t t1, time_t t2) {
	return (double) (t1 - t2);
}

time_t time(time_t *tloc) {
	(void) tloc;
	static time_t t = 0;
	return ++t;
}

size_t strftime(char *s, size_t max, const char *format, const struct tm *tm) {
	(void) max;
	(void) format;
	(void) tm;
	s[0] = 0;
	return 0;
}

time_t mktime(struct tm *tm) {
	(void) tm;
	return (clock_t) -1;
}

static struct tm stubTime;

struct tm *localtime(const time_t *time) {
	(void) time;
	return &stubTime;
}

struct tm *gmtime(const time_t *time) {
	(void) time;
	return &stubTime;
}

// ----------- Time function stubs ^^^^^^^

char *canonicalize_file_name(const char *path) {
	(void) path;
	return nullptr;
}

void *mmap(void *address, size_t length, int protection, int flags, int fildes, off_t offset) {
	if (!address) return MAP_FAILED;
	if (!length) return MAP_FAILED;
	if (protection != PROT_READ) return MAP_FAILED;
	if (flags != MAP_SHARED) return MAP_FAILED;
	if (!fildes) return MAP_FAILED;
	return OSMapObject((OSHandle) fildes, offset, length, OS_MAP_OBJECT_READ_ONLY);
}

int munmap(void *address, size_t length) {
	(void) length;
	OSUnmapObject(address);
	return 0;
}

static void InitialiseCStandardLibrary() {
	stdin = (FILE *) OSHeapAllocate(sizeof(FILE), true);
	stdout = (FILE *) OSHeapAllocate(sizeof(FILE), true);
	stderr = (FILE *) OSHeapAllocate(sizeof(FILE), true);
	stdin->virtualFile = true;
	stdout->virtualFile = true;
	stderr->virtualFile = true;
}
#else
#define __NEED_struct_iovec
#define __NEED_sigset_t
#define __NEED_struct_timespec
#define __NEED_time_t
#define _POSIX_SOURCE
#define _GNU_SOURCE
#include "../ports/musl/obj/include/bits/syscall.h"
#include "../ports/musl/obj/include/bits/alltypes.h"

#include <signal.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <bits/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <sys/utsname.h>
#include <errno.h>

char currentWorkingDirectory[4096];

struct sigaltstack signalHandlerStack;

extern "C" int __libc_start_main(int (*main)(int,char **,char **), int argc, char **argv);

// TODO Proper values?
static char *argv[] = {
	(char *) "/__prefix/ProgramName.esx",
	(char *) "LANG=en_US.UTF-8",
	(char *) "PWD=/__prefix",
	(char *) "HOME=/__user_storage",
	(char *) "PATH=/__posix_bin",
	(char *) nullptr,
};

// HACK HACK HACK HACK
#define GET_POSIX_FILE(fildes) ((PosixFile *) ((intptr_t) fildes | 0x100000000000))

struct PosixFile {
	OSHandle handle;
	uint64_t offset;
	bool seekToEndBeforeWrite;
	char path[4096];
	long flags;
	OSDirectoryChild *children;
	size_t childCount;
};

static void InitialiseCStandardLibrary() {
	// OSPrint("starting musl libc\n");
	strcpy(currentWorkingDirectory, "/__prefix");
	__libc_start_main(nullptr, 1, argv);
}

static char *FixFilename(char *filename) {
	// TODO Thread safety.
	static char fixedFilename[4096];

	char *result;

	if (0 == memcmp(filename, OSLiteral("/__prefix/"))) {
		result = filename + OSCStringLength("/__prefix/");
	} else if (0 == memcmp(filename, OSLiteral("/__user_storage/"))) {
		result = filename + OSCStringLength("/__user_storage/");
	} else if (0 == strcmp(filename, "/__prefix")) {
		result = (char *) "";
	} else if (0 == strcmp(filename, "/__user_storage")) {
		result = (char *) "";
	} else if (0 == memcmp(filename, OSLiteral("./"))) {
		strcpy(fixedFilename, currentWorkingDirectory);
		int x = currentWorkingDirectory[strlen(currentWorkingDirectory) - 1] == '/' ? 1 : 0;
		strcat(fixedFilename, filename + OSCStringLength(".") + x);
		result = (fixedFilename);
	} else if (0 == strcmp(filename, ".")) {
		result = (currentWorkingDirectory);
	} else if (filename[0] != '/') {
		strcpy(fixedFilename, currentWorkingDirectory);
		int x = currentWorkingDirectory[strlen(currentWorkingDirectory) - 1] == '/' ? 1 : 0;
		if (!x) strcat(fixedFilename, "/");
		strcat(fixedFilename, filename);
		result = (fixedFilename);
	} else {
		result = filename;
	}

	// OSPrint("~~~~~~~~FIX %z -> %z\n", filename, result);

	return result;
}

void PrintOutput(char *string, size_t length) {
#if 0
	OSPrint("Print output (%d):\n", length);

	for (uintptr_t i = 0; i < length; i++) {
		OSPrint("\t%d: %X, %c\n", i, string[i], string[i]);
	}
#else
#if 0
	OSPrintDirect(string, length);
#else
	OSMessage m;
	m.type = OS_MESSAGE_PRINT_OUTPUT;
	m.printOutput.string = string;
	m.printOutput.length = length;
	OSSendMessage(osSystemMessages, &m);
#endif
#endif
}

long _OSMakeLinuxSystemCall(long n, long a1, long a2, long a3, long a4, long a5, long a6) {
	switch (n) {
		case SYS_mmap: {
			if (a1 == 0 && a2 && a3 == 3 && a4 == 34 && a5 == -1 && !a6) {
				return (long) OSHeapAllocate(a2, true);
			} else {
				OSPrint("Unsupported mmap [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_munmap: {
			// Warning: we free the whole block regardless of a2
			OSHeapFree((void *) a1);
			return 0;
		} break;

		case SYS_brk: {
			return -1;
		} break;

		case SYS_mremap: {
			void *old = (void *) a1;
			size_t oldSize = a2;
			size_t newSize = a3;

			void *replacement = OSHeapAllocate(newSize, true);
			OSCopyMemory(replacement, old, oldSize);
			OSHeapFree(old);

			return (long) replacement;
		} break;

		case SYS_clock_gettime: {
			struct timespec *tp = (struct timespec *) a2;
			uint64_t microseconds = OSProcessorReadTimeStamp() / osSystemConstants[OS_SYSTEM_CONSTANT_TIME_STAMP_UNITS_PER_MICROSECOND];
			tp->tv_sec = microseconds / 1000;
			tp->tv_nsec = (microseconds * 1000) % 1000000000;
			return 0;
		} break;

		case SYS_arch_prctl: {
			if (a2 == 0x1002) {
				OSSetThreadLocalStorageAddress((void *) a1);
				return 0;
			} else {
				OSPrint("Unsupported arch_prctl [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_set_tid_address: {
			// Warning: we're also supposed to set some kernel variable??
			return OSGetThreadID(OS_CURRENT_THREAD);
		} break;

		case SYS_ioctl: {
			if ((a1 == 0 || a1 == 1 || a1 == 2) && a2 == 21523) {
				// TIOCGWINSZ - get terminal size
				struct winsize *size = (struct winsize *) a3;

				OSMessage m;
				m.type = OS_MESSAGE_GET_TERMINAL_DIMENSIONS;
				OSSendMessage(osSystemMessages, &m);

				size->ws_row = m.getTerminalDimensions.rows;
				size->ws_col = m.getTerminalDimensions.columns;
				size->ws_xpixel = 800;
				size->ws_ypixel = 800;
				return 0;
			}
		} break;

		case SYS_read: {
			if (a1 == 0) {
				// OSPrint("read stdin, max %d\n", a3);
				OSMessage m;
				m.type = OS_MESSAGE_GET_STDIN_BUFFER;
				OSSendMessage(osSystemMessages, &m);
				memcpy((void *) a2, m.getStdinBuffer.string, m.getStdinBuffer.length);
				// OSPrint("\tread %d\n", m.getStdinBuffer.length);
				// for (uintptr_t i = 0; i < m.getStdinBuffer.length; i++) OSPrint("\t%X\n", m.getStdinBuffer.string[i]);
				return m.getStdinBuffer.length;
			} else if (a1 > 2) {
				PosixFile *file = GET_POSIX_FILE(a1);
				uint64_t read = 0;

				intptr_t x = OSReadFileSync(file->handle, file->offset, a3, (void *) a2); 
				if (x >= 0) read += x;
				file->offset += a3;

				return read;
			} else {
				OSPrint("Unsupported read %d [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_access: {
			char *filename = FixFilename((char *) a1);
			OSNodeInformation node;
			OSError error = OSOpenNode(filename, strlen(filename), OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node);
			if (error == OS_SUCCESS) OSCloseHandle(node.handle);
			return error == OS_SUCCESS ? 0 : -1;
		} break;

		case SYS_write: {
			if (a1 == 1 || a1 == 2) {
				PrintOutput((char *) a2, a3);
				return a3;
			} else if (a1 > 2) {
				PosixFile *file = GET_POSIX_FILE(a1);
				uint64_t written = 0;

				if (file->seekToEndBeforeWrite) {
					OSNodeInformation node;
					node.handle = file->handle;
					OSRefreshNodeInformation(&node);
					file->offset = node.fileSize;
				}

				intptr_t x = OSWriteFileSync(file->handle, file->offset, a3, (void *) a2); 
				if (x >= 0) written += x;
				file->offset += a3;

				return written;
			} else {
				OSPrint("Unsupported write %d [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_writev: {
			if (a1 == 1 || a1 == 2) {
				struct iovec *buffers = (struct iovec *) a2;
				size_t s = 0;

				for (intptr_t i = 0; i < a3; i++) {
					if (!buffers[i].iov_len) continue;
					PrintOutput((char *) buffers[i].iov_base, buffers[i].iov_len);
					s += buffers[i].iov_len;
				}

				return s;
			} else {
				OSPrint("Unsupported writev %d [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_sigaltstack: {
			if (a1 && !a2) {
				struct sigaltstack *ss = (struct sigaltstack *) a1;
				signalHandlerStack = *ss;
				return 0;
			} else {
				OSPrint("Unsupported sigaltstack [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_getcwd: {
			if (strlen(currentWorkingDirectory) + 32 > (size_t) a2) {
				return NULL;
			} else {
				if (currentWorkingDirectory[0] == '/') {
					strcpy((char *) a1, "");
				} else {
					strcpy((char *) a1, "/__prefix/");
				}

				strcat((char *) a1, currentWorkingDirectory);
				return a1;
			}
		} break;

		case SYS_chdir: {
			strcpy(currentWorkingDirectory, FixFilename((char *) a1));
			// OSPrint("set cwd to %z\n", currentWorkingDirectory);
		} break;

		case SYS_prlimit64: {
			if (!a1 && a2 == RLIMIT_DATA && !a3 && a4) {
				struct rlimit *limit = (struct rlimit *) a4;
				limit->rlim_cur = limit->rlim_max = RLIM_INFINITY;
				return 0;
			} else {
				OSPrint("Unsupported prlimit64 [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_getuid: {
			return 10;
		} break;

		case SYS_sysinfo: {
			struct sysinfo *info = (struct sysinfo *) a1;

			OSSystemInformation systemInformation;
			OSGetSystemInformation(&systemInformation);

			// Warning: this is mostly made up
			info->uptime = 1;
			info->loads[0] = 1;
			info->loads[1] = 1;
			info->loads[2] = 1;
			info->totalram = systemInformation.totalMemory;
			info->freeram = systemInformation.freeMemory;
			info->sharedram = 0;
			info->bufferram = 0;
			info->totalswap = 0;
			info->freeswap = 0;
			info->procs = systemInformation.processCount;
			info->totalhigh = info->totalram;
			info->freehigh = info->freeram;
			info->mem_unit = 1;

			return 0;
		} break;

		case SYS_fstat: {
			PosixFile *file = GET_POSIX_FILE(a1);
			struct stat *buffer = (struct stat *) a2;
			OSNodeInformation node;
			node.handle = file->handle;
			OSRefreshNodeInformation(&node);

			buffer->st_mode = node.type == OS_NODE_DIRECTORY ? S_IFDIR : S_IFREG;
			buffer->st_dev = 0;
			buffer->st_ino = 0;
			buffer->st_nlink = 1;
			buffer->st_mode |= S_IRWXU | S_IRWXG | S_IRWXO;
			buffer->st_uid = 10;
			buffer->st_gid = 10;
			buffer->st_rdev = 0;
			buffer->st_size = node.fileSize;
			buffer->st_atime = 0;
			buffer->st_mtime = 0;
			buffer->st_ctime = 0;
			buffer->st_blksize = 4096;
			buffer->st_blocks = (node.fileSize + 4095) / 4096;

			return 0;
		} break;

		case SYS_lstat:
		case SYS_stat: {
			char *filename = FixFilename((char *) a1);
			struct stat *buffer = (struct stat *) a2;

			OSZeroMemory(buffer, sizeof(struct stat));

			OSError error;
			OSNodeInformation node;

			error = OSOpenNode(filename, strlen(filename), OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node);

			if (error == OS_ERROR_INCORRECT_NODE_TYPE) {
				error = OSOpenNode(filename, strlen(filename), OS_OPEN_NODE_FAIL_IF_NOT_FOUND | OS_OPEN_NODE_DIRECTORY, &node);
				buffer->st_mode = S_IFDIR;
			} else {
				buffer->st_mode = node.type == OS_NODE_DIRECTORY ? S_IFDIR : S_IFREG;
			}

			if (error == OS_SUCCESS) {
				OSCloseHandle(node.handle);

				buffer->st_dev = 0;
				buffer->st_ino = 0;
				buffer->st_nlink = 1;
				buffer->st_mode |= S_IRWXU | S_IRWXG | S_IRWXO;
				buffer->st_uid = 10;
				buffer->st_gid = 10;
				buffer->st_rdev = 0;
				buffer->st_size = node.fileSize;
				buffer->st_atime = 0;
				buffer->st_mtime = 0;
				buffer->st_ctime = 0;
				buffer->st_blksize = 4096;
				buffer->st_blocks = (node.fileSize + 4095) / 4096;

				// OSPrint("stat %z\n", a1);

				return 0;
			} else {
				// OSPrint("couldn't stat %z (%z)\n", a1, filename);
				return -1;
			}
		} break;

		case SYS_rt_sigaction:
		case SYS_rt_sigprocmask: {
			// OSPrint("Warning, ignoring signal syscall %d [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
			return 0;
		} break;

		case SYS_socket: {
			return -1;
		} break;

		case SYS_chmod: {
			return 0;
		} break;

		case SYS_open: {
			char *filename = FixFilename((char *) a1);
			unsigned flags = 0;

			if ((a2 & O_ACCMODE) == O_RDONLY) flags |= OS_OPEN_NODE_READ_ACCESS;
			if ((a2 & O_ACCMODE) == O_RDWR) flags |= OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS;
			if ((a2 & O_ACCMODE) == O_WRONLY) flags |= OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS;

			if ((a2 & O_CREAT) && (a2 & O_EXCL)) flags |= OS_OPEN_NODE_FAIL_IF_FOUND;
			if (!(a2 & O_CREAT)) flags |= OS_OPEN_NODE_FAIL_IF_NOT_FOUND;

			if (a2 & O_DIRECTORY) flags |= OS_OPEN_NODE_DIRECTORY;
			if (a2 & O_TRUNC) flags |= OS_OPEN_NODE_RESIZE_ACCESS;

			// OSPrint("attempting to open %z (%z) with flags %x\n", filename, (char *) a1, flags);
			OSNodeInformation node;
			OSError error = OSOpenNode(filename, OSCStringLength(filename), flags, &node);
			// OSPrint("\terror = %x\n", error);

			if (error != OS_SUCCESS) {
				// OSPrint("couldn't open %z [%x; %z; %d]\n", (char *) a1, flags, filename, error);
				return -1;
			} else {
				PosixFile *file = (PosixFile *) OSHeapAllocate(sizeof(PosixFile), true);

				// HACK HACK HACK HACK
				int fildes = (int) ((intptr_t) file & ~0x100000000000);

				if ((void *) ((intptr_t) fildes | 0x100000000000) != file) {
					// *sigh*
					OSHeapFree(file);
					OSCloseHandle(node.handle);
					OSPrint("\tcan't represent as posix file\n");
					return -1;
				}

				file->handle = node.handle;
				file->seekToEndBeforeWrite = a2 & O_APPEND;
				file->offset = 0;
				file->flags = a2;
				strcpy(file->path, filename);
				file->childCount = node.directoryChildren;

				if (a2 & O_TRUNC) {
					OSResizeFile(file->handle, 0); 
				}

				// OSPrint("\tsuccess!\n");
				// OSPrint("opened %z [%x]\n", (char *) a1, flags);

				return fildes;
			}
		} break;

		case SYS_fchdir: {
			strcpy(currentWorkingDirectory, GET_POSIX_FILE(a1)->path);
			// OSPrint("set cwd to %z\n", currentWorkingDirectory);
			return 0;
		} break;

		case SYS_close: {
			PosixFile *file = GET_POSIX_FILE(a1);
			OSCloseHandle(file->handle);
			OSHeapFree(file->children);
			OSHeapFree(file);
			return 0;
		} break;

		case SYS_fcntl: {
			PosixFile *file = GET_POSIX_FILE(a1);

			if (a2 == F_GETFD) {
				return file->flags;
			} else if (a2 == F_SETFD && a3 == FD_CLOEXEC) {
				// ignore.
				return 0;
			} else {
				OSPrint("Unsupported fcntl [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_readv: {
			if (a1 > 2) {
				PosixFile *file = GET_POSIX_FILE(a1);
				struct iovec *buffers = (struct iovec *) a2;
				uint64_t read = 0;

				for (intptr_t i = 0; i < a3; i++) {
					if (!buffers[i].iov_len) continue;
					intptr_t x = OSReadFileSync(file->handle, file->offset, buffers[i].iov_len, buffers[i].iov_base); 
					if (x < 0) break;
					read += x;
					file->offset += buffers[i].iov_len;
				}

				return read;
			} else {
				OSPrint("Unsupported readv [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
				OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
			}
		} break;

		case SYS_madvise: {
			// Ignore.
			// It's only "advice" after all, right?
			return 0;
		} break;

		case SYS_getdents64: {
			PosixFile *file = GET_POSIX_FILE(a1);

			if (!file->children) {
				file->children = (OSDirectoryChild *) OSHeapAllocate(sizeof(OSDirectoryChild) * (file->childCount + 32), true);

				if (OS_SUCCESS != OSEnumerateDirectoryChildren(file->handle, file->children, file->childCount + 32)) {
					return -1;
				}
			}

			struct dirent *buffer = (struct dirent *) a2;
			size_t spaceLeft = a3;
			
			while (true) {
				OSDirectoryChild *child = file->children + file->offset;

				if (!child->information.present) {
					return a3 - spaceLeft;
				}

				size_t s = sizeof(struct dirent) + child->nameLengthBytes;

				// OSPrint("read dirent %s\n", child->nameLengthBytes, child->name);

				if (spaceLeft <= s) {
					return a3 - spaceLeft;
				}

				spaceLeft -= s;
				file->offset++;

				buffer->d_ino = 0;
				buffer->d_off = buffer->d_reclen = s;
				buffer->d_type = child->information.type == OS_NODE_DIRECTORY ? DT_DIR : DT_REG;
				memcpy(buffer->d_name, child->name, child->nameLengthBytes);

				buffer = (struct dirent *) ((char *) buffer + s);
			}
		} break;

		case SYS_ftruncate: {
			PosixFile *file = GET_POSIX_FILE(a1);

			if (OS_SUCCESS == OSResizeFile(file->handle, a2)) return 0;
			else return -1;
		} break;

		case SYS_fsync: {
			return 0;
		} break;

		case SYS_lseek: {
			PosixFile *file = GET_POSIX_FILE(a1);

			if (a3 == SEEK_SET) {
				file->offset = a2;
			} else if (a3 == SEEK_CUR) {
				file->offset += a2;
			} else if (a3 == SEEK_END) {
				OSNodeInformation node;
				node.handle = file->handle;
				OSRefreshNodeInformation(&node);
				file->offset = node.fileSize + a2;
			}

			return file->offset;
		} break;

		case SYS_uname: {
			struct utsname *name = (struct utsname *) a1;
			strcpy(name->sysname, "essence");
			strcpy(name->nodename, "node");
			strcpy(name->release, "indev");
			strcpy(name->machine, "machine");
			return 0;
		} break;

		case SYS_getpid: {
			return OSGetProcessID(OS_CURRENT_PROCESS);
		} break;

		case SYS_poll: {
			struct pollfd *fds = (struct pollfd *) a1;

			if (a2 == 1 && fds[0].fd == 0 /*stdin*/ && fds[0].events == POLL_IN) {
				// OSPrint("poll stdin with timeout %d\n", a3);

				// Wait until data on stdin is available.
				OSMessage m;
				m.type = OS_MESSAGE_WAIT_STDIN;
				m.waitStdin.timeoutMs = a3 < 0 ? OS_WAIT_NO_TIMEOUT : a3;
				OSSendMessage(osSystemMessages, &m);

				if (m.waitStdin.timeoutMs) {
					return 0;
				} else {
					fds[0].revents = POLL_IN;
					return 1;
				}
			} else {
				// TODO Support polling on other file descriptors.
				return -1;
			}
		} break;

		case SYS_exit_group: {
			OSCrashProcess(a1 + 1000);
		} break;

		case SYS_nanosleep: {
			const struct timespec *sleep = (const struct timespec *) a1;
			uint64_t milliseconds = sleep->tv_sec * 1000 + sleep->tv_nsec / 1000000;
			if (!milliseconds) milliseconds = 1;
			OSSleep(milliseconds);
			return 0;
		} break;

		case SYS_readlink: {
			// We don't have symbolic links (yet)
			return -EINVAL;
		} break;

		case SYS_unlink: {
			// We don't have symbolic links (yet)
			return -EACCES;
		} break;

		case SYS_rename: {
			return rename((char *) a1, (char *) a2);
		} break;

		default: {
			OSPrint("Unknown linux syscall %d [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
			OSCrashProcess(OS_FATAL_ERROR_UNKNOWN_SYSCALL);
		} break;
	}

	return -1;
}

extern "C" long OSMakeLinuxSystemCall(long n, long a1, long a2, long a3, long a4, long a5, long a6) {
	// OSPrint("-> linux syscall %d [%x, %x, %x, %x, %x, %x]\n", n, a1, a2, a3, a4, a5, a6);
	long result = _OSMakeLinuxSystemCall(n, a1, a2, a3, a4, a5, a6);
	// OSPrint("<- result %x\n", result);
	return result;
}
#endif
