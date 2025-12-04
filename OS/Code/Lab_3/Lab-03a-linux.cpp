#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

/* Простые вспомогательные функции для вывода ошибок и строк без stdio-buffering */
static void xputs(const char *s)
{
    write(STDOUT_FILENO, s, strlen(s));
}

static void xperror(const char *msg)
{
    write(STDERR_FILENO, msg ? msg : "error", msg ? strlen(msg) : 5);
    write(STDERR_FILENO, ": ", 2);
    char buf[128];
    int n = snprintf(buf, sizeof(buf), "%s\n", strerror(errno));
    if (n > 0) write(STDERR_FILENO, buf, (size_t)n);
}

/* Преобразование целого в строку (возвращает pointer в buf) */
static char *int_to_str(int v, char *buf, size_t bufsz)
{
    if (bufsz == 0) return buf;
    snprintf(buf, bufsz, "%d", v);
    return buf;
}

int main(int argc, char *argv[])
{
    pid_t p1 = -1, p2 = -1;
    int status;
    const char *child_path = "./lab03x"; /* путь к Lab-03x (отсюда) */
    int iter1 = 5; /* пример значения для первого дочернего */
    int iter2 = 7; /* значение для второго дочернего, через окружение */

    /* Проверка доступности исполняемого файла перед fork */
    if (access(child_path, X_OK) != 0)
    {
        xperror("child executable not accessible");
        return 1;
    }

    /* --- Первый дочерний: передать количество итераций как argv --- */
    p1 = fork();
    if (p1 < 0)
    {
        xperror("fork for child1 failed");
        return 1;
    }
    if (p1 == 0)
    {
        /* дочерний процесс 1 */
        char arg0[256];
        char arg1[32];
        /* arg0 = путь к исполняемому, arg1 = iterations */
        strncpy(arg0, child_path, sizeof(arg0) - 1); arg0[sizeof(arg0)-1] = '\0';
        int_to_str(iter1, arg1, sizeof(arg1));
        char *child_argv[] = { arg0, arg1, NULL };
        /* Среда наследуется (NULL -> environ используется автоматически для execv) */
        execv(child_path, child_argv);
        /* если execv вернуло - ошибка */
        xperror("execv (child1) failed");
        _exit(127);
    }

    /* --- Подготовка окружения и второй дочерний --- */
    /* В родителе установим локальную переменную окружения ITER_NUM для наследования */
    if (setenv("ITER_NUM", int_to_str(iter2, (char[32]){0}, 32), 1) != 0)
    {
        /* Примечание: вызов setenv с временным буфером (compound literal)
           допустим — он формирует строку и сразу кладёт в окружение. */
        xperror("setenv failed");
        /* продолжаем — возможно первый дочерний уже запущен; пытаемся создать второй */
    }

    p2 = fork();
    if (p2 < 0)
    {
        xperror("fork for child2 failed");
        /* попробовать дождаться первого и выйти */
        if (p1 > 0) waitpid(p1, NULL, 0);
        return 1;
    }
    if (p2 == 0)
    {
        /* дочерний процесс 2: не передаём аргументы, пусть Lab-03x возьмёт ITER_NUM из окружения */
        char *child_argv[] = { (char *)child_path, NULL };
        execv(child_path, child_argv);
        xperror("execv (child2) failed");
        _exit(127);
    }

    /* Родитель: дождаться завершения обоих дочерних (выполняются одновременно) */
    int exited = 0;
    while (exited < 2)
    {
        pid_t w = wait(&status);
        if (w == -1)
        {
            if (errno == EINTR) continue;
            xperror("wait failed");
            break;
        }
        if (w == p1)
        {
            if (WIFEXITED(status))
            {
                char buf[128];
                int n = snprintf(buf, sizeof(buf), "child1 (pid %d) exited, code=%d\n", (int)w, WEXITSTATUS(status));
                write(STDOUT_FILENO, buf, (size_t)n);
            }
            else if (WIFSIGNALED(status))
            {
                char buf[128];
                int n = snprintf(buf, sizeof(buf), "child1 (pid %d) killed by signal %d\n", (int)w, WTERMSIG(status));
                write(STDOUT_FILENO, buf, (size_t)n);
            }
            exited++;
        }
        else if (w == p2)
        {
            if (WIFEXITED(status))
            {
                char buf[128];
                int n = snprintf(buf, sizeof(buf), "child2 (pid %d) exited, code=%d\n", (int)w, WEXITSTATUS(status));
                write(STDOUT_FILENO, buf, (size_t)n);
            }
            else if (WIFSIGNALED(status))
            {
                char buf[128];
                int n = snprintf(buf, sizeof(buf), "child2 (pid %d) killed by signal %d\n", (int)w, WTERMSIG(status));
                write(STDOUT_FILENO, buf, (size_t)n);
            }
            exited++;
        }
        else
        {
            /* может быть какой-то другой процесс; просто сообщим */
            char buf[128];
            int n = snprintf(buf, sizeof(buf), "reaped pid %d\n", (int)w);
            write(STDOUT_FILENO, buf, (size_t)n);
        }
    }

    /* Очистка: можно удалить локальную переменную окружения, если нужно */
    unsetenv("ITER_NUM");

    xputs("parent exiting\n");
    return 0;
}