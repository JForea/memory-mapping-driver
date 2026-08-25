#include "memory_helpers.h"

#include <linux/pgtable.h>
#include <linux/mm.h>
#include <linux/pfn.h>
#include <asm/pgalloc.h>
#include <asm/current.h>
#include <asm/io.h>
#include <asm/tlbflush.h>

static int kmalloc_phys(phys_addr_t *phys, unsigned long sz, gfp_t gfp) {
    void *virt;
    
    virt = kmalloc(sz, gfp);
    if (!virt) {
        return -ENOMEM;
    }

    *phys = virt_to_phys(virt);
    return 0;
}

int mem_allocate(unsigned long addr) {
    struct mm_struct *mm;
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    pte_t entry;
    // unsigned long page_count;
    unsigned long pfn;
    phys_addr_t phys;
    int err;

    mm = current->mm;

    // if (!len)
	// 	return -EINVAL;

    printk(KERN_INFO "MMD: before PGD.\n");

    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd)) {
        err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
        if (err)
            return err;

        set_pgd(pgd, __pgd(phys | _PAGE_TABLE));
    }

    if (pgd_bad(*pgd))
        return -EFAULT;

    printk(KERN_INFO "MMD: before PUD.\n");

    pud = pud_offset((p4d_t *)pgd, addr);
    if (pud_none(*pud)) {
        err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
        if (err)
            return err;

        set_pud(pud, __pud(phys | _PAGE_TABLE));
    }

    if (pud_bad(*pud))
        return -EFAULT;

    printk(KERN_INFO "MMD: before PMD.\n");

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd)) {
        err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
        if (err)
            return err;

        set_pmd(pmd, __pmd(phys | _PAGE_TABLE));
    }

    if (pmd_bad(*pmd))
        return -EFAULT;

    printk(KERN_INFO "MMD: before PTE.\n");

    pte = pte_offset_kernel(pmd, addr);

    printk(KERN_INFO "MMD: ptr_pte = %px.\n");

    err = kmalloc_phys(&phys, PAGE_SIZE, GFP_KERNEL);
    if (err)
        return err;

    printk(KERN_INFO "MMD: before PTE set.\n");

    set_pte(pte, __pte(phys | _PAGE_TABLE));

    // len = PAGE_ALIGN(len);
    // page_count = len / PAGE_SIZE;

    // while (page_count--) {
        pfn = PHYS_PFN(phys);
        entry = pfn_pte(pfn, PAGE_SHARED);
        set_pte_at(mm, addr, pte, entry);
    // }

    printk(KERN_INFO "MMD: PTE = 0x%lx\n", pte_val(*pte));
    
    return 0;
}

int mem_free(unsigned long addr) {
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long pfn;
    unsigned long phys;
    void *virt;

    mm = current->mm;

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
    __flush_tlb_all();
    kfree(virt);

    return 0;
}
