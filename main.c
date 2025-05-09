#include <dirent.h>
#include <limits.h>
#include <ps5/kernel.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const int MAX_USER_COUNT = 16;
const char *USER_PATH_PREFIX = "/user/home/";
const char *USER_PATH_SUFFIX = "/webkit/shell";

typedef struct notify_request {
  char useless[45];
  char message[3075];
} notify_request_t;

int sceKernelSendNotificationRequest(int, notify_request_t *, size_t, int);

int get_all_user_cache_paths(char users[MAX_USER_COUNT][PATH_MAX + 1]) {
  int user_count = 0;
  char user_path[PATH_MAX + 1];
  struct stat statbuf;
  struct dirent *dp;
  DIR *dir;

  if (!(dir = opendir(USER_PATH_PREFIX))) {
    return 0;
  }

  while ((dp = readdir(dir))) {
    if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
      continue;
    }

    strncpy(user_path, USER_PATH_PREFIX, PATH_MAX);
    strncat(user_path, dp->d_name, PATH_MAX - strlen(user_path));

    if (stat(user_path, &statbuf)) {
      continue;
    }

    if (S_ISDIR(statbuf.st_mode)) {
      strncat(user_path, USER_PATH_SUFFIX, PATH_MAX - strlen(user_path));
      strncpy(users[user_count], user_path, PATH_MAX);
      user_count++;
    }
  }

  closedir(dir);
  return user_count;
}

int is_user_cache_path_valid(char *path) {
  size_t path_len = strlen(path);
  size_t prefix_len = strlen(USER_PATH_PREFIX);
  size_t suffix_len = strlen(USER_PATH_SUFFIX);

  if (path_len <= PATH_MAX && path_len > prefix_len + suffix_len &&
      strncmp(path, USER_PATH_PREFIX, prefix_len) == 0 &&
      strcmp(path + path_len - suffix_len, USER_PATH_SUFFIX) == 0) {
    return 1;
  }

  return 0;
}

void delete_folder(const char *path) {
  char tmp_path[PATH_MAX + 1];
  struct stat statbuf;
  struct dirent *dp;
  DIR *dir;

  if (!(dir = opendir(path))) {
    return;
  }

  while ((dp = readdir(dir))) {
    if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
      continue;
    }

    strncpy(tmp_path, path, PATH_MAX);
    strncat(tmp_path, "/", PATH_MAX - strlen(tmp_path));
    strncat(tmp_path, dp->d_name, PATH_MAX - strlen(tmp_path));

    if (stat(tmp_path, &statbuf)) {
      continue;
    }

    if (S_ISDIR(statbuf.st_mode)) {
      delete_folder(tmp_path);
    } else {
      remove(tmp_path);
    }
  }

  closedir(dir);
  rmdir(path);
}

int main() {
  pid_t pid = getpid();
  intptr_t rootdir = kernel_get_proc_rootdir(pid);
  char user_cache_path[MAX_USER_COUNT][PATH_MAX + 1];
  int user_count;
  notify_request_t req;

  kernel_set_proc_rootdir(pid, kernel_get_root_vnode());
  user_count = get_all_user_cache_paths(user_cache_path);

  for (int i = 0; i < user_count; i++) {
    if (is_user_cache_path_valid(user_cache_path[i])) {
      delete_folder(user_cache_path[i]);
    }
  }

  kernel_set_proc_rootdir(pid, rootdir);

  memset(&req, 0, sizeof(req));
  strncpy(req.message, "WebKit cache removed for all users. (v1.01)",
          sizeof(req.message));

  return sceKernelSendNotificationRequest(0, &req, sizeof(req), 0);
}
