
#include <iostream>
#include "corsair-ctrl.h"

int main()
{
    KernelInterface driver = KernelInterface("\\\\.\\corsair-util");

    ULONG address = driver.GetClientAddress();

    int i = 0;
    KERNEL_REQUEST kRequest;
    // arbitrary address
    kRequest.address = 0x00ECA968;
    kRequest.data = &i;
    kRequest.size = sizeof(i);

    driver.ReadMemory(&kRequest);

    if (kRequest.data != nullptr) {
        int a = *(int*)kRequest.data;
        std::cout << "i value: " << a;
    }

    return 0;
}
