/**
 * @file    popen_p.c
 * @author  ln
 * @brief   popen likely
 **/

#include "popen_p.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char * const shell_path = "/bin/sh";

/**
 * @brief   popen likely, return the pid of child process & pipe stream
 * @param   cmd     shell command string
 *          type    "r" or "w"
 *          fpid    file stream & pid of shell '/bin/sh' (not your 'cmd' process!)
 *
 * @return  return 0 on success. otherwise, -1 is returned on error and errno is set.
 *
 * 'fpid' is not invalid while return value is not 0.
 * errno is set if fail
 **/
int popen_p(const char *cmd, const char *type, file_t *fpid)
{
    pid_t pid;
    FILE *fp;
    int pfd[2], fd_child, fd_parent;

    if ((type == NULL) || (type[1] != '\0') || (type[0] != 'w' && type[0] != 'r')) {
        errno = EINVAL;
        return -1;
    }

    if (pipe(pfd) < 0) {
        return -1;                  // errno set by pipe()
    }

    if (type[0] == 'r') {
        fd_child = STDOUT_FILENO;   // 1
        fd_parent = STDIN_FILENO;   // 0
    } else {
        fd_child = STDIN_FILENO;    // 0
        fd_parent = STDOUT_FILENO;  // 1
    }

    if ((pid = fork()) < 0) {
        const int err = errno;      // backup errno
        close(pfd[0]);
        close(pfd[1]);
        errno = err;                // restore errno
        return -1;
    }

    if (pid == 0) {                 // child process
        close(pfd[fd_parent]);
        if (pfd[fd_child] != fd_child) {
            if (dup2(pfd[fd_child], fd_child) == -1) {
                close(pfd[fd_child]);
                _Exit(EXIT_FAILURE);
            }
            close(pfd[fd_child]);
        }

        execl(shell_path, "sh", "-c", cmd, NULL);
        _Exit(127);
    } else {                        // parent process
        close(pfd[fd_child]);
        if ((fp = fdopen(pfd[fd_parent], type)) == NULL) {
            close(pfd[fd_parent]);
            return -1;
        }
    }

    fpid->fp = fp;
    fpid->pid = pid;
    return 0;
}

/**
 * @brief   for popen_p
 * @param   fpid    file stream & pid
 * @return  -1 if error, otherwise the child process exit code(0 is ok) is returned.
 **/
int pclose_p(file_t *fpid) 
{
    int status;
    pid_t wpid;

    if (fclose(fpid->fp) == EOF) {
        return -1;
    }

    do {
        wpid = waitpid(fpid->pid, &status, 0);
    } while ((wpid == (pid_t) -1) && errno == EINTR);

    if (wpid == (pid_t)-1){
        errno = ECHILD;
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    errno = ECHILD;
    return -1;
}

#ifdef __cplusplus
}
#endif

