#include "server.h"
#include "config.h"
#include "logger.h"
#include <stdio.h>

int main() {
    Config config = load_config("config.ini");
    set_log_level_custom(config.log_level);
    if (config.log_file_path != NULL) {
        set_log_file(config.log_file_path);
        LOG_INFO("Log file set to %s", config.log_file_path);
    }
    start_server(config);
    close_log_file(); // Close log file on server shutdown
    return 0;
}
