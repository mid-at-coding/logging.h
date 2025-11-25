#define LOG_H_IMPLEMENTATION
#define LOG_H_ENUM_PREFIX_
#define LOG_H_NAMESPACE_
#include "logging.h"

int main(int argc, char **argv){
	set_log_level(LOG_INFO);
	logf_out("%d is my number :3", LOG_DBG, 3);
}
