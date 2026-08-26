#include <iostream>

#include <mmd/mmd.h>

int main() {
    int *val = (int *)0xffffffffc0287010;
    int err = mem_allocate(reinterpret_cast<unsigned long>(val), sizeof(int));
    if (err < 0) {
        std::cout << "Something went wrong: " << err << '\n';
        return err;
    }

    std::cout << *val << '\n';

    // err = mem_free(reinterpret_cast<unsigned long>(val), sizeof(int));
    // if (err) {
        // std::cout << "Something went wrong during free: " << err << '\n';
        // return err;
    // }
}