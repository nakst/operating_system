#if 0
typedef volatile int sig_atomic_t;
typedef int64_t time_t;
typedef int64_t clock_t;
typedef int64_t off_t;

typedef struct lconv {
	char *decimal_point;
	char *thousands_sep;
	char *grouping;
	char *int_curr_symbol;
	char *currency_symbol;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char frac_digits;
	char p_cs_precedes;
	char n_cs_precedes;
	char p_sep_by_space;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char int_frac_digits;
	char int_p_cs_precedes;
	char int_n_cs_precedes;
	char int_p_sep_by_space;
	char int_n_sep_by_space;
	char int_p_sign_posn;
	char int_n_sign_posn;
} lconv;

typedef struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
} tm;

struct stat {
	int64_t st_dev;
	int64_t st_ino;
	int64_t st_mode;
	int64_t st_nlink;
	int64_t st_uid;
	int64_t st_gid;
	int64_t st_rdev;
	int64_t st_size;
	int64_t st_blksize;
	int64_t st_blocks;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
};

typedef struct FILE {
	OSNodeInformation node;

	uint64_t position;
	int ungotc;

	uint64_t seekToEndBeforeWrites : 1,
		 endOfFile : 1,
		 error : 1,
		 temporary : 1,
		 virtualFile : 1;
} FILE;

extern int errno;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define RAND_MAX (16777215)
#define EOF (-1)
#define BUFSIZ (2048)
#define SEEK_SET (1)
#define SEEK_CUR (2)
#define SEEK_END (4)
#define _IONBF (8)
#define _IOLBF (16)
#define _IOFBF (32)
#define HUGE_VAL (__builtin_huge_val())
#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))
#define EXIT_SUCCESS (OS_CALLBACK_HANDLED)
#define EXIT_FAILURE (OS_CALLBACK_REJECTED)
#define CLOCKS_PER_SEC (1000)
#define L_tmpnam (16)
#define SIG_DFL (NULL)
#define SIGINT (2)
#define LC_ALL (1)
#define LC_COLLATE (2)
#define LC_CTYPE (4)
#define LC_MONETARY (8)
#define LC_NUMERIC (16)
#define LC_TIME (32)
#define PRIx8 "x"
#define PRIx16 "x"
#define PRId32 "d"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIX32 "X"
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"
#define PROT_READ (0x01)
#define PROT_WRITE (0x02)
#define PROT_EXEC (0x04)
#define PROT_NONE (0x08)
#define MAP_SHARED (0x10)
#define MAP_PRIVATE (0x20)
#define MAP_FIXED (0x40)
#define MAP_FAILED (NULL)

OS_EXTERN_C int isalnum(int c);
OS_EXTERN_C int iscntrl(int c);
OS_EXTERN_C int isgraph(int c);
OS_EXTERN_C int islower(int c);
OS_EXTERN_C int isprint(int c);
OS_EXTERN_C int ispunct(int c);
OS_EXTERN_C int isupper(int c);
OS_EXTERN_C int isxdigit(int c);
OS_EXTERN_C __attribute__((noreturn)) void abort();
OS_EXTERN_C double acos(double x);
OS_EXTERN_C double asin(double x);
OS_EXTERN_C double atan2(double y, double x);
OS_EXTERN_C double ceil(double x);
OS_EXTERN_C void clearerr(FILE *stream);
OS_EXTERN_C clock_t clock();
OS_EXTERN_C double cos(double x);
OS_EXTERN_C double cosh(double x);
OS_EXTERN_C double difftime(time_t t1, time_t t2);
OS_EXTERN_C void exit(int status);
OS_EXTERN_C double exp(double x);
OS_EXTERN_C int fclose(FILE *stream);
OS_EXTERN_C int feof(FILE *stream);
OS_EXTERN_C int ferror(FILE *stream);
OS_EXTERN_C int fflush(FILE *stream);
OS_EXTERN_C char *fgets(char *s, int size, FILE *stream);
OS_EXTERN_C double floor(double x);
OS_EXTERN_C double fmod(double x, double y);
OS_EXTERN_C FILE *fopen(const char *path, const char *mode);
OS_EXTERN_C int fprintf(FILE *stream, const char *format, ...);
OS_EXTERN_C int fputs(const char *s, FILE *stream);
OS_EXTERN_C size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
OS_EXTERN_C FILE *freopen(const char *path, const char *mode, FILE *stream);
OS_EXTERN_C double frexp(double x, int *exp);
OS_EXTERN_C int fseek(FILE *stream, long offset, int whence);
OS_EXTERN_C int fseeko(FILE *stream, off_t offset, int whence);
OS_EXTERN_C long ftell(FILE *stream);
OS_EXTERN_C off_t ftello(FILE *stream);
OS_EXTERN_C size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#define getc fgetc
OS_EXTERN_C int fgetc(FILE *stream);
OS_EXTERN_C struct tm *gmtime(const time_t *time);
OS_EXTERN_C double ldexp(double x, int exp);
OS_EXTERN_C struct tm *localtime(const time_t *time);
OS_EXTERN_C double log(double x);
OS_EXTERN_C double log10(double x);
OS_EXTERN_C double log2(double x);
OS_EXTERN_C time_t mktime(struct tm *tm);
OS_EXTERN_C double pow(double x, double y);
OS_EXTERN_C int remove(const char *path);
OS_EXTERN_C int rename(const char *oldPath, const char *newPath);
OS_EXTERN_C char *setlocale(int category, const char *locale);
OS_EXTERN_C int setvbuf(FILE *stream, char *buf, int mode, size_t size);
OS_EXTERN_C void (*signal(int sig, void (*func)(int)))(int);
OS_EXTERN_C double sin(double x);
OS_EXTERN_C double sinh(double x);
OS_EXTERN_C int snprintf(char *str, size_t size, const char *format, ...);
OS_EXTERN_C int sprintf(char *str, const char *format, ...);
OS_EXTERN_C int printf(const char *format, ...);
OS_EXTERN_C void srand(unsigned int seed);
OS_EXTERN_C int strcoll(const char *s1, const char *s2);
OS_EXTERN_C char *strerror(int errnum);
OS_EXTERN_C size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
OS_EXTERN_C char *strpbrk(const char *s, const char *accept);
OS_EXTERN_C double strtod(const char *nptr, char **endptr);
OS_EXTERN_C int system(const char *command);
OS_EXTERN_C double tan(double x);
OS_EXTERN_C double tanh(double x);
OS_EXTERN_C time_t time(time_t *tloc);
OS_EXTERN_C FILE *tmpfile();
OS_EXTERN_C char *tmpnam(char *s);
OS_EXTERN_C void *memset(void *s, int c, size_t n);
OS_EXTERN_C void *memcpy(void *dest, const void *src, size_t n);
OS_EXTERN_C size_t strlen(const char *s);
OS_EXTERN_C void *malloc(size_t size);
OS_EXTERN_C void *calloc(size_t num, size_t size);
OS_EXTERN_C void *memmove(void *dest, const void *src, size_t n);
OS_EXTERN_C void free(void *ptr);
OS_EXTERN_C void *realloc(void *ptr, size_t size);
OS_EXTERN_C double fabs(double x);
OS_EXTERN_C int abs(int n);
OS_EXTERN_C int ifloor(double x);
OS_EXTERN_C int iceil(double x);
OS_EXTERN_C double sqrt(double x);
OS_EXTERN_C char *getenv(const char *name);
OS_EXTERN_C int strcmp(const char *s1, const char *s2);
OS_EXTERN_C int strcasecmp(const char *s1, const char *s2);
OS_EXTERN_C int strncmp(const char *s1, const char *s2, size_t n);
OS_EXTERN_C int strncasecmp(const char *s1, const char *s2, size_t n);
OS_EXTERN_C int toupper(int c);
OS_EXTERN_C int tolower(int c);
OS_EXTERN_C char *strncpy(char *dest, const char *src, size_t n);
OS_EXTERN_C long int strtol(const char *nptr, char **endptr, int base);
OS_EXTERN_C unsigned long int strtoul(const char *nptr, char **endptr, int base);
#define strtoull strtoul
OS_EXTERN_C char *strcat(char *dest, const char *src);
OS_EXTERN_C char *strsep(char **stringp, const char *delim);
OS_EXTERN_C char *strstr(const char *haystack, const char *needle);
OS_EXTERN_C char *strrchr(const char *s, int c);
OS_EXTERN_C void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
OS_EXTERN_C char *strcpy(char *dest, const char *src);
OS_EXTERN_C char *stpcpy(char *dest, const char *src);
OS_EXTERN_C char *strchr(const char *s, int c);
OS_EXTERN_C size_t strnlen(const char *s, size_t maxlen);
OS_EXTERN_C size_t strspn(const char *s, const char *accept);
OS_EXTERN_C size_t strcspn(const char *s, const char *reject);
OS_EXTERN_C int memcmp(const void *s1, const void *s2, size_t n);
OS_EXTERN_C void *memchr(const void *s, int c, size_t n);
OS_EXTERN_C int isspace(int c);
OS_EXTERN_C int isalpha(int c);
OS_EXTERN_C int isdigit(int c);
OS_EXTERN_C void OSAssertionFailure();
OS_EXTERN_C struct lconv *localeconv();
OS_EXTERN_C int rand();
OS_EXTERN_C int ungetc(int c, FILE *stream);
#define putc fputc
OS_EXTERN_C int fputc(int c, FILE *stream);
OS_EXTERN_C int puts(const char *string);
OS_EXTERN_C void perror(const char *string);
OS_EXTERN_C int vsnprintf(char *string, size_t size, const char *format, va_list arguments);
OS_EXTERN_C int ftruncate(int fildes, off_t length);
OS_EXTERN_C int fileno(FILE *stream);
OS_EXTERN_C int access(const char *path, int amode);
OS_EXTERN_C int fstat(int fildes, struct stat *buffer);
OS_EXTERN_C int stat(const char *path, struct stat *buffer);
OS_EXTERN_C int getpagesize();
OS_EXTERN_C void *mmap(void *address, size_t length, int protection, int flags, int fildes, off_t offset);
OS_EXTERN_C int munmap(void *address, size_t length);
OS_EXTERN_C char *canonicalize_file_name(const char *path);
OS_EXTERN_C int atoi(const char *nptr);
OS_EXTERN_C int sscanf(const char *string, const char *format, ...);

#define assert(x) do{if (!(x)) OSAssertionFailure();}while(0)

#ifdef ARCH_X86_64
#define OS_PAGE_SIZE (4096)
typedef struct jmp_buf { uintptr_t rsp, rbp, rbx, r12, r13, r14, r15, returnAddress; } jmp_buf;
#endif
OS_EXTERN_C int _setjmp(jmp_buf *env);
OS_EXTERN_C __attribute__((noreturn)) void _longjmp(jmp_buf *env, int val);
#define setjmp(x) _setjmp(&(x))
#define longjmp(x, y) _longjmp(&(x), (y))
#endif
