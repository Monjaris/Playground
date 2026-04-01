#pragma once
#include "common.h"
#include <fstream>
#include <expected>

class Parser {
    std::string line = {};
    size_t idx = 0;

    using ValueMap = std::unordered_map<std::string, std::string>;
    ValueMap values;

public:
    std::ifstream source;
    char comment_prefix = '#';


private:

    ValueMap data() {
        return values;
    }

    bool check_size() {
        return line.size() > idx;
    }

    void skip_ws() {
        while (check_size() && isspace(line[idx])) {
            ++idx;
        }
    }

    // accepts sequence of letter and dots
    std::string parse_name() {
        std::string str;
        while (check_size() && (isalpha(line[idx]) || line[idx]=='.')) {
            str += line[idx++];
        }
        return str;
    }

    // accepts sequence of letter and digits
    std::string parse_value() {
        std::string str;
        while (check_size() && isalnum(line[idx]))
        {
            str += line[idx++];
        }
        return str;
    }



public:
    const char* path = nullptr;

    bool init(const char* file) {
        path = file;
        source.open(path);
        return source.is_open();
    }

    /// @arg `file_ext` doesn't automate putting dot prefix
    bool validate(const char* file_ext) {
        if (path == nullptr) {
            return EXIT_FAILURE;
        }
        else if (file_ext != nullptr) {
            const size_t src_len = strlen(path);
            const size_t ext_len = strlen(file_ext);
            if (ext_len > src_len) {
                return EXIT_FAILURE;
            }
            if (strcmp(path+(src_len-ext_len), file_ext)) {
                return EXIT_FAILURE;
            }
            else return EXIT_SUCCESS;
        }
        else return EXIT_SUCCESS;
    }

    bool read() {
        while (std::getline(source, line))
        {
            idx = 0;

            skip_ws();
            if (!check_size() || (line[idx]==comment_prefix)) {
                continue;
            }
            std::string name = parse_name();

            skip_ws();
            if (line[idx++] != '=') {
                return false;
            }

            skip_ws();
            std::string value = parse_value();

            skip_ws();
            if (check_size() && line[idx] != comment_prefix) {
                return false;
            }

            // push to the value map
            values.try_emplace(name, value);
        }
        return true;
    }


    template <typename Number> requires std::is_integral_v<Number> std::expected
    <Number, std::string> get(const std::string& property_name, bool get_raw=false)
    {
        const std::string& raw = values[property_name];
        if (get_raw) {
            return std::unexpected(raw);
        }

        long double value = -1;
        const char* raw_cs = raw.c_str();
        std::errc ec = std::from_chars(raw_cs, raw_cs+strlen(raw_cs), value).ec;
        // number parsing successful
        if (ec == std::errc()) {
            return value;
        }
        // number is unparsable
        else {
            return std::unexpected(values[property_name]);
        }
    }

};

