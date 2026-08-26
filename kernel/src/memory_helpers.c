#include "memory_helpers.h"

#include "pt_structs.h"

#include <linux/pgtable.h>
#include <linux/mm.h>
#include <linux/pfn.h>
#include <asm/pgalloc.h>
#include <asm/current.h>
#include <asm/io.h>
#include <asm/tlbflush.h>

static int kmalloc_phys(phys_addr_t *phys, unsigned long sz, gfp_t gfp) {
    void *virt;
    
    virt = kzalloc(sz, gfp);
    if (!virt) {
        return -ENOMEM;
    }

    *phys = virt_to_phys(virt);

    printk(KERN_INFO "MMD: memory allocated on 0x%px.\n", &phys);

    return 0;
}

/* Returns number of pages need to alloc.
*/
static unsigned long len_align(unsigned long *len) {
    if (!len)
		return -EINVAL;
    
    *len = PAGE_ALIGN(*len);
    return *len / PAGE_SIZE;
}

int mem_patch(unsigned long addr) {
    phys_addr_t cr3;
    union virtual_addr virt;
    struct pml4e *pml4es;
    struct pdpe *pdpes;
    struct pde *pdes;
    struct pte *ptes;

    virt.value = addr;

    asm( " mov %%cr3, %%rax \n mov %%rax, %0 " : "=m" (cr3) :: "%rax" );

    pml4es = phys_to_virt(cr3);
    pml4es[virt.pml4_offset].us = 1;
    
    pdpes = phys_to_virt( 
        pml4es[virt.pml4_offset].pdp_base_addr << PAGE_SHIFT
    );
    pdpes[virt.pdp_offset].us = 1;

    pdes = phys_to_virt( 
        pdpes[virt.pdp_offset].pd_base_addr << PAGE_SHIFT
    );
    pdes[virt.pd_offset].us = 1;

    ptes = phys_to_virt( 
        pdes[virt.pd_offset].pt_base_addr << PAGE_SHIFT
    );
    ptes[virt.pt_offset].us = 1;

    printk(KERN_INFO "MMD: before PGD.\n");

    __flush_tlb_all();
    
    return 0;
}

int mem_allocate(unsigned long addr, unsigned long len) {
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    pte_t entry;
    unsigned long page_count;
    unsigned long pfn;
    phys_addr_t phys;
    int err;

    mm = current->mm;

    page_count = len_align(&len);

    if (IS_ERR_VALUE(page_count)) {
        return -EINVAL;
    }

    while (page_count--) {
        printk(KERN_INFO "MMD: before PGD.\n");

        pgd = pgd_offset(mm, addr);
        if (pgd_none(*pgd)) {
            err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
            if (err)
                return err;

            set_pgd(pgd, __pgd(phys | _PAGE_TABLE));
        }

        *(unsigned long *)pgd |= 1 << 2;

        if (pgd_bad(*pgd))
            return -EFAULT;

        printk(KERN_INFO "MMD: before P4D.\n");

        p4d = p4d_offset(pgd, addr);
        if (p4d_none(*p4d)) {
            printk(KERN_INFO "MMD: P4D none.\n");
            err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
            if (err)
                return err;

            printk(KERN_INFO "MMD: before P4D set.\n");
            
            set_p4d(p4d, __p4d(phys | _PAGE_TABLE));
        }

        *(unsigned long *)p4d |= 1 << 2;

        printk(KERN_INFO "MMD: after p4d none.\n");

        if (p4d_bad(*p4d)) {
            printk(KERN_INFO "MMD: P4D bad.\n");
            return err;
        }

        printk(KERN_INFO "MMD: before PUD.\n");

        pud = pud_offset(p4d, addr);
        if (pud_none(*pud)) {
            printk(KERN_INFO "MMD: PUD none.\n");
            err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
            if (err)
                return err;

            printk(KERN_INFO "MMD: before set PUD.\n");
            set_pud(pud, __pud(phys | _PAGE_TABLE));
        }

        *(unsigned long *)pud |= 1 << 2;

        printk(KERN_INFO "MMD: after PUD none.\n");

        if (pud_bad(*pud)) {
            printk(KERN_INFO "MMD: PUD bad.\n");
            return -EFAULT;
        }

        printk(KERN_INFO "MMD: before PMD.\n");

        pmd = pmd_offset(pud, addr);
        if (pmd_none(*pmd)) {
            err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
            if (err)
                return err;

            set_pmd(pmd, __pmd(phys | _PAGE_TABLE));
        }

        *(unsigned long *)pmd |= 1 << 2;

        if (pmd_bad(*pmd))
            return -EFAULT;

        printk(KERN_INFO "MMD: before PTE.\n");

        pte = pte_offset_kernel(pmd, addr);

        printk(KERN_INFO "MMD: ptr_pte = %px.\n", &pte);
        if (pte_none(*pte)) {
            err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
            if (err)
                return err;

            printk(KERN_INFO "MMD: before PTE set.\n");

            set_pte(pte, __pte(phys | _PAGE_TABLE));
        }

        *(unsigned long *)pte |= 1 << 2;

        pfn = PHYS_PFN(phys);
        entry = pfn_pte(pfn, PAGE_SHARED);
        if (pte_none(entry))
            set_pte_at(mm, addr, pte, entry);

        printk(KERN_INFO "MMD: PTE = 0x%lx\n", pte_val(*pte));

        addr += PAGE_SIZE;
    }

    __flush_tlb_all();
    
    return 0;
}

int mem_free(unsigned long addr, unsigned long len) {
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long page_count;
    unsigned long pfn;
    unsigned long phys;
    void *virt;

    mm = current->mm;

    page_count = len_align(&len);

    if (IS_ERR_VALUE(page_count)) {
        return -EINVAL;
    }

    while (page_count--) {
        pgd = pgd_offset(mm, addr);
        if (pgd_none(*pgd) || pgd_bad(*pgd))
            return -EINVAL;

        p4d = p4d_offset(pgd, addr);
        if (p4d_none(*p4d) || p4d_bad(*p4d))
            return -EINVAL;

        pud = pud_offset(p4d, addr);
        if (pud_none(*pud) || pud_bad(*pud))
            return -EINVAL;

        pmd = pmd_offset(pud, addr);
        if (pmd_none(*pmd) || pmd_bad(*pmd))
            return -EINVAL;

        pte = pte_offset_kernel(pmd, addr);

        if (pte_none(*pte))
            return -EINVAL;

        pfn = pte_pfn(*pte);
        phys = PFN_PHYS(pfn);

        virt = phys_to_virt(phys);

        pte_clear(mm, addr, pte);

        /* Not very good but at first ok */
        __flush_tlb_all();
        kfree(virt);
        
        addr += PAGE_SIZE;
    }

    return 0;
}
