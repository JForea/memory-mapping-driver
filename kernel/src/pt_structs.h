#ifndef PT_STRUCTS_H
#define PT_STRUCTS_H

#include <asm-generic/int-ll64.h>

union virtual_addr {
    u64 value;
    struct {
        u64 phys_page_offset : 12;
        u64 pt_offset        : 9;
        u64 pd_offset        : 9;
        u64 pdp_offset       : 9;
        u64 pml4_offset      : 9;
        u64 sign_extend      : 16;
    };
};

union pml4e {
    u64 value;
    struct {
        u64 p : 1;
        u64 rw : 1;
        u64 us : 1;
        u64 pwt : 1;
        u64 pcd : 1;
        u64 a : 1;
        u64 ign : 1;
        u64 mbz1 : 1;
        u64 mbz2 : 1;
        u64 avl : 3;
        u64 pdp_base_addr : 40;
        u64 available : 11;
        u64 nx : 1;
    };
};

union pdpe {
    u64 value;
    struct {
        u64 p : 1;
        u64 rw : 1;
        u64 us : 1;
        u64 pwt : 1;
        u64 pcd : 1;
        u64 a : 1;
        u64 ign1 : 1;
        u64 z : 1;
        u64 ign2 : 1;
        u64 avl : 3;
        u64 pd_base_addr : 40;
        u64 available : 11;
        u64 nx : 1;
    };
};

union pde {
    u64 value;
    struct {
        u64 p : 1;
        u64 rw : 1;
        u64 us : 1;
        u64 pwt : 1;
        u64 pcd : 1;
        u64 a : 1;
        u64 ign1 : 1;
        u64 z : 1;
        u64 ign2 : 1;
        u64 avl : 3;
        u64 pt_base_addr : 40;
        u64 available : 11;
        u64 nx : 1;
    };
};

union pte {
    u64 value;
    struct {
        u64 p : 1;
        u64 rw : 1;
        u64 us : 1;
        u64 pwt : 1;
        u64 pcd : 1;
        u64 a : 1;
        u64 d : 1;
        u64 pat : 1;
        u64 g : 1;
        u64 avl : 3;
        u64 phys_page_base_addr : 40;
        u64 available : 11;
        u64 nx : 1;
    };
};

union pte_pke {
    u64 value;
    struct {
        u64 p : 1;
        u64 rw : 1;
        u64 us : 1;
        u64 pwt : 1;
        u64 pcd : 1;
        u64 a : 1;
        u64 d : 1;
        u64 pat : 1;
        u64 g : 1;
        u64 avl : 3;
        u64 phys_page_base_addr : 40;
        u64 available : 7;
        u64 mem_prot_key : 4;
        u64 nx : 1;
    };
};


#endif