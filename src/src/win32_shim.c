/**
 * @file win32_shim.c
 *
 * @brief Link shim for the fully static Windows build
 *
 * The MSYS2 libformw.a was compiled against the pcre2 DLL, so it asks for
 * dllimport-style symbols. Provide them from the static pcre2 POSIX wrapper.
 */
#define PCRE2_STATIC
#include <pcre2posix.h>

void *__imp_pcre2_regcomp = (void *) pcre2_regcomp;
void *__imp_pcre2_regexec = (void *) pcre2_regexec;
void *__imp_pcre2_regfree = (void *) pcre2_regfree;
void *__imp_pcre2_regerror = (void *) pcre2_regerror;
