/*
 * exercise 2 solution
 * linux process enumeration: PID + executable path via readlink
 * hack2.c
 * author: @cocomelonc
 *
 * base: 02-hacking-process-enum/hack.c
 *   original: prints only the PID of the first matching process
 *
 * change: also resolve and print the full path of the executable
 *   by reading the symlink /proc/<pid>/exe with readlink(2).
 *
 * new API: readlink(path, buf, bufsiz)
 *   - /proc/<pid>/exe is a symlink that the kernel maintains pointing
 *     to the executable image of the process.
 *   - readlink does NOT append a null terminator, so we do it manually.
 *   - returns -1 if the process has already exited between the /proc
 *     scan and the readlink call - we handle that gracefully.
 *
 * compile:
 *   gcc hack2.c -o hack2
 * usage:
 *   ./hack2 <process_name>
 * example:
 *   ./hack2 bash
 *   found pid: 1234  exe: /usr/bin/bash
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>    /* readlink */

/*
 * exercise 2 change: new helper.
 * given a numeric PID string (from d_name), resolve /proc/<pid>/exe
 * and print the full executable path.
 * returns 0 on success, -1 if readlink fails (process exited, etc.).
 */
static int print_exe_path(const char *pid_str) {
  char link_path[64];
  char exe_buf[4096];
  ssize_t len;

  /* build the /proc/<pid>/exe path */
  snprintf(link_path, sizeof(link_path), "/proc/%s/exe", pid_str);

  /*
   * readlink(2) reads the target of a symbolic link.
   * it does NOT null-terminate the result - we do it ourselves.
   */
  len = readlink(link_path, exe_buf, sizeof(exe_buf) - 1);
  if (len == -1) {
    /* process may have exited between the /proc scan and here */
    fprintf(stderr, "  [!] readlink(%s) failed (process may have exited)\n", link_path);
    return -1;
  }
  exe_buf[len] = '\0';   /* manual null termination */
  printf("  exe: %s\n", exe_buf);
  return 0;
}

/*
 * exercise 2: extended version of find_process_by_name from hack.c.
 * reads /proc/<pid>/comm for the name match (case-sensitive, same as
 * the original), then calls print_exe_path to show the full binary path.
 */
int find_process_by_name(const char *proc_name) {
  DIR *dir;
  struct dirent *entry;
  int pid = -1;

  dir = opendir("/proc");
  if (dir == NULL) {
    perror("opendir /proc failed");
    return -1;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (!isdigit(*entry->d_name)) continue;

    char path[512];
    snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);

    FILE *fp = fopen(path, "r");
    if (!fp) continue;

    char comm[512];
    int matched = 0;
    if (fgets(comm, sizeof(comm), fp) != NULL) {
      comm[strcspn(comm, "\r\n")] = 0;   /* strip trailing newline */
      if (strcmp(comm, proc_name) == 0) {
        pid = atoi(entry->d_name);
        matched = 1;
      }
    }
    fclose(fp);

    if (matched) {
      /*
       * exercise 2 addition:
       * original: printf("found pid: %d\n", pid);
       * updated:  print pid AND exe path on two lines
       */
      printf("found pid: %d\n", pid);
      print_exe_path(entry->d_name);
      break;   /* stop at first match, same behaviour as original */
    }
  }

  closedir(dir);
  return pid;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <process_name>\n", argv[0]);
    return 1;
  }

  int pid = find_process_by_name(argv[1]);
  if (pid == -1) {
    printf("process '%s' not found.\n", argv[1]);
  }

  return 0;
}
