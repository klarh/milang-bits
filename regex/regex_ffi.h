#ifndef MI_REGEX_H
#define MI_REGEX_H

typedef struct MiRegex MiRegex;
typedef struct MiMatch MiMatch;

MiRegex* mi_re_compile(char* pattern);
void mi_re_free(MiRegex* re);

int mi_re_test(MiRegex* re, char* subject);
char* mi_re_match(MiRegex* re, char* subject);
long mi_re_match_start(MiRegex* re, char* subject);
long mi_re_match_len(MiRegex* re, char* subject);
char* mi_re_capture(MiRegex* re, char* subject, long group);
long mi_re_capture_count(MiRegex* re, char* subject);

char* mi_re_replace(MiRegex* re, char* subject, char* replacement);
char* mi_re_replace_all(MiRegex* re, char* subject, char* replacement);

char* mi_re_find_all(MiRegex* re, char* subject);
char* mi_re_split(MiRegex* re, char* subject);

int mi_re_is_valid(char* pattern);
long mi_re_count(MiRegex* re, char* subject);
char* mi_re_escape(char* str);

#endif
