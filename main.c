#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ps5/kernel.h>

#define MAX_USER_COUNT 16

typedef struct notify_request {
  char useless1[45];
  char message[3075];
} notify_request_t;

int sceKernelSendNotificationRequest(int, notify_request_t*, size_t, int);

int get_all_user_cache_paths(char users[MAX_USER_COUNT][PATH_MAX + 1]) {
  const char *user_path_prefix = "/user/home"; 
  int user_count = 0;
  char user_path[PATH_MAX+1];
  struct stat statbuf;
  struct dirent *dp;
  DIR *dir;
  if(!(dir=opendir(user_path_prefix))) {
    return 0;
  }
  while((dp=readdir(dir))) {
    if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
      continue;
    }
    strncpy(user_path, user_path_prefix, PATH_MAX);
    strncat(user_path, "/", PATH_MAX);
    strncat(user_path, dp->d_name, PATH_MAX);
    if(stat(user_path, &statbuf)) {
      continue;
    }
    if(S_ISDIR(statbuf.st_mode)) {
      strncat(user_path, "/webkit/shell", PATH_MAX);
      strncpy(users[user_count], user_path, PATH_MAX);
      user_count++;
    }
  }
  closedir(dir);
  return user_count;
}

void delete_folder(const char *path) {
  char tmp_path[PATH_MAX + 1];
  struct stat statbuf;
  struct dirent *dp;
  DIR *dir;
  if(!(dir=opendir(path))) {
    return;
  }
  while((dp=readdir(dir))) {
    if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
      continue;
    }
    strncpy(tmp_path, path, PATH_MAX);
    strncat(tmp_path, "/", PATH_MAX);
    strncat(tmp_path, dp->d_name, PATH_MAX);
    if(stat(tmp_path, &statbuf)) {
      continue;
    }
    if(S_ISDIR(statbuf.st_mode)) {
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
    delete_folder(user_cache_path[i]);
  }
  kernel_set_proc_rootdir(pid, rootdir);
  bzero(&req, sizeof req);
  strncpy(req.message, "WebKit Cache Removed for All Users.", sizeof req.message);
  return sceKernelSendNotificationRequest(0, &req, sizeof req, 0);
}
