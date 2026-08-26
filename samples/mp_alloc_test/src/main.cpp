#include <iostream>

#include <mmd/mmd.h>
#include <unistd.h>

int main() {
    int pageSize = getpagesize();
    int allocSize = pageSize * 2;

    int *val = nullptr;
    int err = mem_allocate(reinterpret_cast<unsigned long>(val), allocSize);
    if (err < 0) {
        std::cout << "Something went wrong: " << err << '\n';
        return err;
    }

    int cnt = pageSize * 2 / sizeof(int);
    
    const int printPerLine = 10;
    int currentlyPrinted = 0;
    while (cnt--) {
        *val = cnt;

        if (currentlyPrinted == printPerLine) {
            std::cout << std::endl;
            currentlyPrinted = 0;
        }

        std::cout << *val << ' ';
        currentlyPrinted++;

        val++;
    }

    val = nullptr;

    std::cout << std::endl <<
        *val << std::endl;

    mem_free(reinterpret_cast<unsigned long>(nullptr), allocSize);
}
