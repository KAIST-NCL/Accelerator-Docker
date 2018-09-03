#include "utils.hpp"

void
str_lower(char *str)
{
        for (char *p = str; *p != '\0'; ++p)
                *p = (char)tolower(*p);
}

bool
str_equal(const char *s1, const char *s2)
{
        return (!strcmp(s1, s2));
}

bool
str_case_equal(const char *s1, const char *s2)
{
        return (!strcasecmp(s1, s2));
}

bool
str_has_prefix(const char *str, const char *prefix)
{
        return (!strncmp(str, prefix, strlen(prefix)));
}

bool
str_has_suffix(const char *str, const char *suffix)
{
        size_t len, slen;

        len = strlen(str);
        slen = strlen(suffix);
        return ((len >= slen) ? str_equal(str + len - slen, suffix) : false);
}

bool
str_empty(const char *str)
{
        return (str != NULL && *str == '\0');
}

bool
str_array_match(const char *str, const char * const arr[], size_t size)
{
        for (size_t i = 0; i < size; ++i) {
                if (str_has_prefix(str, arr[i]))
                        return (true);
        }
        return (false);
}

int
str_join(char **s1, const char *s2, const char *sep)
{
        size_t size = 1;
        char *buf;

        if (*s1 != NULL && **s1 != '\0')
                size += strlen(*s1) + strlen(sep);
        size += strlen(s2);
        if ((buf = (char*)realloc(*s1, size)) == NULL) {
                return (-1);
        }
        if (*s1 == NULL)
                *buf = '\0';
        if (*buf != '\0')
                strcat(buf, sep);
        strcat(buf, s2);
        *s1 = buf;
        return (0);
}

int
str_to_pid(const char *str, pid_t *pid)
{
        char *ptr;
        intmax_t n;

        n = strtoimax(str, &ptr, 10);
        if (ptr == str || *ptr != '\0') {
                goto fail;
        }
        if (n == INTMAX_MIN || n == INTMAX_MAX || n != (pid_t)n) {
                goto fail;
        }
        *pid = (pid_t)n;
        return (0);

 fail:
        return (-1);
}

int
str_to_ugid(char *str, uid_t *uid, gid_t *gid)
{
        char *ptr;
        uintmax_t n;
        struct passwd *passwd = NULL;
        struct group *group = NULL;

        n = strtoumax(str, &ptr, 10);
        if (ptr != str) {
                if (*ptr != '\0' && *ptr != ':') {
                        goto fail;
                }
                if (n == UINTMAX_MAX || n != (uid_t)n) {
                        goto fail;
                }
                if (*ptr == ':')
                        ++ptr;
                *uid = (uid_t)n;
        } else {
                /* Not a numeric UID, check for a username. */
                if ((ptr = strchr(str, ':')) != NULL)
                        *ptr++ = '\0';
                if ((passwd = getpwnam(str)) == NULL) {
                        goto fail;
                }
                *uid = passwd->pw_uid;
        }

        str = ptr;
        if (str == NULL || *str == '\0') {
                /* No group specified, infer it from the UID */
                if (passwd == NULL && (passwd = getpwuid(*uid)) == NULL) {
                        goto fail;
                }
                *gid = passwd->pw_gid;
                return (0);
        }

        n = strtoumax(str, &ptr, 10);
        if (ptr != str) {
                if (*ptr != '\0') {
                        goto fail;
                }
                if (n == UINTMAX_MAX || n != (gid_t)n) {
                        goto fail;
                }
                *gid = (gid_t)n;
        } else {
                /* Not a numeric GID, check for a groupname. */
                if ((group = getgrnam(str)) == NULL) {
                        goto fail;
                }
                *gid = group->gr_gid;
        }
        return (0);

 fail:
        return (-1);
}