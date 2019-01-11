#include <stdio.h>
#include "event.h"

#include <iostream>


//void main_loop(struct event_base* base)

int main() {
    const char** m = event_get_supported_methods();

    std::cout << "version=" << event_get_version() << std::endl;
    
    for (int i = 0; m[i] != NULL; i++) {
        std::cout << "support:" << m[i] << std::endl;
    }

    struct event_base* base;
    int f;

    base = event_base_new();
    if (!base) {
        std::cout << "Can not use !" << std::endl;
    } else {
        std::cout << "use " << event_base_get_method(base) << std::endl; 
        f = event_base_get_features(base);
        if ((f & EV_FEATURE_ET)) 
            std::cout << "Edge-trigger supported" << std::endl;
        if ((f & EV_FEATURE_O1))
            std::cout << "O(1) event supported" << std::endl;
        if ((f & EV_FEATURE_FDS))
            std::cout << "All FD types are supported" << std::endl;
    }

    //dump events
    FILE* dump_f = fopen("event.dump", "wb");
    if (dump_f) {
        event_base_dump_events(base, dump_f);
        fclose(dump_f);
    }

    event_base_free(base);
 
    return 0;
}
