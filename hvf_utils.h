#pragma once
#include <Hypervisor/Hypervisor.h>
#include "test.h"

void setup_vm(test_t*, const char *kernel_name);
void destroy_vm(test_t*);
void step_cpu(hv_vcpu_t);
void run_until_infloop(hv_vcpu_t c, bool dump_traces);
