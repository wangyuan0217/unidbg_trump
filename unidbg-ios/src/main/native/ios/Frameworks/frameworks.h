#import <string.h>
#import <dlfcn.h>
#import <stdio.h>
#import <sys/sysctl.h>
#import <CoreFoundation/CoreFoundation.h>

static void print_lr(char *buf, uintptr_t lr) {
  Dl_info info;
  int success = dladdr((const void *) lr, &info);
  if(success) {
    long offset = lr - (long) info.dli_fbase;
    const char *name = info.dli_fname;
    const char *find = name;
    while(find) {
      const char *next = strchr(find, '/');
      if(next) {
        find = &next[1];
      } else {
         break;
      }
    }
    if(find) {
      name = find;
    }
    sprintf(buf, "[%s]%p", name, (void *) offset);
  } else {
    sprintf(buf, "%p", (void *) lr);
  }
}

static void hex(char *buf, void *ptr, size_t size) {
  const char *data = (const char *) ptr;
  int idx = 0;
  for(int i = 0; i < size; i++) {
    idx += sprintf(&buf[idx], "%02x", data[i] & 0xff);
  }
}

static char *cfdata_hex(CFDataRef data) {
  if(data) {
    const UInt8 *ptr = CFDataGetBytePtr(data);
    size_t size = (size_t) CFDataGetLength(data);
    char *buf = malloc(size * 2 + 1);
    hex(buf, (void *)ptr, size);
    return buf;
  } else {
    return NULL;
  }
}

// Logger.getLogger("com.github.unidbg.ios.debug").setLevel(Level.DEBUG);
static int is_debug() {
  int mib[2];
  int values[2];
  size_t size = sizeof(values);

  char *name = "unidbg.debug";
  mib[0] = CTL_UNSPEC;
  mib[1] = 3;
  return sysctl(mib, 2, values, &size, name, strlen(name));
}
