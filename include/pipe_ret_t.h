#pragma once

struct pipe_ret_t {
    bool successFlag = false;
    std::string msg = "";

    pipe_ret_t() = default;
    pipe_ret_t(bool successFlag, const std::string &msg) {
       this->successFlag = successFlag;
       this->msg = msg;
    }

    static pipe_ret_t failure(const std::string & msg);
    static pipe_ret_t success(const std::string &msg = "");
};

