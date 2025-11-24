#define LOG_H_IMPLEMENTATION
#define LOG_H_ENUM_PREFIX_
#define LOG_H_NAMESPACE_
#include "logging.h"
#define STR(name) #name

int main(int argc, char **argv){
	set_log_level(LOG_DBG);
	logf_out("The name of this program is: %s", LOG_DBG, argv[0]);
	if(argc < 2){
		logf_out("Usage: %s [filename]", LOG_ERROR, argv[0]);
		exit(EXIT_FAILURE);
	}
	FILE *fp = fopen(argv[1], "w");
	if(!fp){
		logf_out("Couldn't open %s!", LOG_ERROR, argv[1]);
		exit(EXIT_FAILURE);
	}
	log_f(fp, "Hello world from logging.h!", LOG_INFO);
	exit(EXIT_SUCCESS); 
}
