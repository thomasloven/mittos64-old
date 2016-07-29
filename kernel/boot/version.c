#include <debug.h>

char *_kernel_build_date = __DATE__;
char *_kernel_build_time = __TIME__;

char *_kernel_git_hash = GITHASH;
char *_kernel_git_date = GITDATE;
int _kernel_git_dirty = GITDIRTY;
char *_kernel_git_message = GITMESSAGE;
char *_kernel_git_branch = GITBRANCH;

void debug_build_time()
{
  debug_info("Kernel built: %s (%s)\n", _kernel_build_date, _kernel_build_time);
}
void debug_git_info()
{
  debug_info("Kernel git: %s%s%s %s\n", _kernel_git_branch, _kernel_git_hash, (_kernel_git_dirty)?" (dirty)":"", _kernel_git_date);
  debug_info("Git message: %s\n", _kernel_git_message);
}
