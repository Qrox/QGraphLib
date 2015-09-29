#include "profiler.h"

profiler::profiler() {
    start = get_wall_time();
}

profiler::operator double() {
    return get_wall_time() - start;
}
