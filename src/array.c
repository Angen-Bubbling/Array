#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include <signal.h>
#include "array.skel.h"

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    return vfprintf(stderr, format, args);
}

void handle_event(void *ctx, int cpu, void *value, unsigned int value_sz)
{
	int *m = value;
	printf("read the value: %d\n", *m);
}


int main(){
  struct array_bpf *skel;
  int err;
  struct perf_buffer *pr = NULL;


  libbpf_set_print(libbpf_print_fn);

  skel = array_bpf__open_and_load();
  
  if (!skel) {
        fprintf(stderr, "Failed to open and load BPF skeleton\n");
        return 1;
  }
  
  err = array_bpf__attach(skel);
  if (err) {
    fprintf(stderr, "Failed to auto-attach BPF skeleton: %d\n", err);
  }
  pr = perf_buffer__new(bpf_map__fd(skel->maps.output), 8, handle_event, NULL, NULL, NULL);

  while(true){
    err = perf_buffer__poll(pr, 100);
    if (err < 0) {
      fprintf(stderr, "Error polling perf buffer: %d\n", err);
      break;
    }
  }

}