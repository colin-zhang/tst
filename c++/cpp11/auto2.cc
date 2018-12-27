#include <stdio.h>
#include <vector>

int main()
{
    std::vector<int> v;
    for (int i = 0; i < 100; i++) {
        v.push_back(i);
    }

    for (auto it = v.begin(); it != v.end(); it++) {
        printf("%d ", *it);
    }
    printf("\n");
    return 0;
}
