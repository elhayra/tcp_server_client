#include <string>
#include "../include/pipe_ret_t.h"

pipe_ret_t pipe_ret_t::failure(const std::string &msg) {
    return pipe_ret_t(false, msg);
}

pipe_ret_t pipe_ret_t::success(const std::string &msg) {
    return pipe_ret_t(true, msg);
}
