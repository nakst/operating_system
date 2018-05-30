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
#include "../ports/musl/obj/include/bits/syscall.h"

#define __NEED_struct_iovec
#include "../ports/musl/obj/include/bits/alltypes.h"

extern "C" int __libc_start_main(int (*main)(int,char **,char **), int argc, char **argv);

// TODO Proper values?
static char *argv[] = {
	(char *) "/ProgramName.esx",
	(char *) "LANG=en_US.UTF-8",
	(char *) "PWD=/__working_directory",
	(char *) "HOME=/__user_storage",
	(char *) "PATH=/__posix_bin",
	(char *) nullptr,
};

static void InitialiseCStandardLibrary() {
	// OSPrint("starting musl libc\n");
	__libc_start_main(nullptr, 1, argv);
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
			return -1;
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
			if (a1 == 1 && a2 == 21523) {
				// TIOCGWINSZ - get terminal size
				// Warning: this is handled fully.
				return 0;
			}
		} break;

		case SYS_writev: {
			if (a1 == 1) {
				struct iovec *buffers = (struct iovec *) a2;

				for (intptr_t i = 0; i < a3; i++) {
					if (!buffers[i].iov_len) continue;
					OSPrintDirect((char *) buffers[i].iov_base, buffers[i].iov_len);
				}
			}
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
