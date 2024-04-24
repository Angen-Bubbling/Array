#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "Dual BSD/GPL";

//定义数组map
struct {
__uint(type, BPF_MAP_TYPE_ARRAY);
__type(key, int);
__type(value, int);
__uint(max_entries, 256);
}arraymap SEC(".maps");

//定义输出缓冲区map
struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(u32));
} output SEC(".maps");


SEC("uprobe//home/fish/Project/Array/src/userarray:array_set")
int BPF_KPROBE(array_set, int index, int value)
{
    bpf_printk("array_set ENTRY: set_index = %d, set_value = %d", index, value);
    bpf_map_update_elem(&arraymap, &index, &value, BPF_ANY);
    return 0;
}

SEC("uprobe//home/fish/Project/Array/src/userarray:array_read")
int BPF_KPROBE(array_read, int index)
{
    int *value_ptr;
    int value;

    bpf_printk("array_read ENTRY: read_index = %d", index);
    value_ptr = bpf_map_lookup_elem(&arraymap, &index);

    if(!value_ptr){
        bpf_printk("array_read: read_value not found");
        return 0;
    }
    else{
        value = *value_ptr;
        bpf_printk("array_read: read_value = %d", value);
        bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &value, sizeof(value));
    }
    return 0;
}
