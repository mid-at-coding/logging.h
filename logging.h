/* logging.h: v1.1: GPL v3
 * A simple, cross platform, logging library for C.
 * Example usage:
 * 	#define LOG_H_IMPLEMENTATION
 * 	#define LOG_H_ENUM_PREFIX_
 * 	#define LOG_H_NAMESPACE_ 
 * 	#include "logging.h"
 * 	int main(int argc, char **argv){
 * 		set_log_level(LOG_DBG);
 *		logf_out("The name of this program is: %s", LOG_DBG, argv[0]);
 *		if(argc < 2){
 *			logf_out("Usage: %s [filename]", LOG_ERROR, argv[0]);
 *			exit(EXIT_FAILURE);
 *		}
 *		FILE *fp = fopen(argv[1], "w");
 *		if(!fp){
 *			logf_out("Couldn't open %s!", LOG_ERROR, argv[1]);
 *			exit(EXIT_FAILURE);
 *		}
 *		log_f(fp, "Hello world from logging.h!", LOG_INFO);
 *		exit(EXIT_SUCCESS);
 * 	}
 *
 * Definitions:
 * 	LOG_H_ENUM_PREFIX_: the prefix for the sensitivity enum values and type
 * 	enum LOG_H_LOG_LEVEL:
 * 		LOG_TRACE
 * 		LOG_DBG
 * 		LOG_INFO
 * 		LOG_WARN (yellow color)
 * 		LOG_ERROR (red color)
 * 	LOG_H_NAMESPACE_: the prefix for the function names
 * 	LOG_H_IMPLEMENTATION: defines where the implementation lives
 * 	LOG_H_STDERR_THRESHOLD: defines the log level above which log_* will be sent to stderr instead of stdout (unset = always stdout, 
 * 	    only takes effect when LOG_H_IMPLEMENTATION is defined)
 * API:
 *	get_prefix(level):
 *		returns the prefix for the given level, including setting the color if appropriate
 *
 *	char* get_prefix_nc(level):
 *		returns the prefix for the given level, without color (used in _f variants)
 *
 *	char* get_postfix(level):
 *		returns the postfix for the given level, mostly just resets the color of the terminal (not called for _f variants)
 *
 *	set_log_level(level):
 *		stops logs under the given level from outputting
 *
 *	bool logif(level):
 *		returns true if the given level is equal or above the set level
 *
 *	log_out(str, level):
 *		outputs str to stdout with the appropriate prefix and a newline
 *
 *	logf_out(fmt, level, ...):
 *		outputs a formatted string to stdout with the appropriate prefix
 *
 *	log_f(fp, str, level):
 *		outputs str to a file with the appropriate prefix and a newline
 *
 *	logf_f(fp, fmt, level, ...):
 *		outputs a formatted string to a file with the appropriate prefix
 *
 *	Quirks:
 *	 Setting a different LOG_H_FUN defines a different logging "instance" with completely seperate sensitivity.
 *	 Two instances with the same LOG_H_FUN may not have differing LOG_H_NAME unless the compiler is smart enough
 *	 to figure out that the types are essentially the same.
 *	Changelog:
 *	 1.0 -> 1.1: 
 *	  Fixed bug with log_out not showing colors, added LOG_H_STDERR_THRESHOLD, changed sensitivity to be namespaced
 * */

#ifndef LOGGING_H_
#define LOGGING_H_
#include <stdbool.h>
#include <stdio.h>

#define LOG_PASTER(x, y) x ##  y
#define LOG_EVALUATOR(x, y) LOG_PASTER(x, y)
#define LOG_H_NAME(name) LOG_EVALUATOR(LOG_H_ENUM_PREFIX_, name)
#define LOG_H_FUN(name) LOG_EVALUATOR(LOG_H_NAMESPACE_, name)

#ifdef _WIN32
# define LOG_H_ENDL "\r\n"
#else 
# define LOG_H_ENDL "\n"
#endif // _WIN32

enum LOG_H_NAME(LOG_H_LOG_LEVEL){
	LOG_H_NAME(LOG_TRACE),
	LOG_H_NAME(LOG_DBG),
	LOG_H_NAME(LOG_INFO),
	LOG_H_NAME(LOG_WARN),
	LOG_H_NAME(LOG_ERROR)
};

extern enum LOG_H_NAME(LOG_H_LOG_LEVEL) LOG_H_FUN(sensitivity); // sensitivity isn't a function, but should be locally namespaced to avoid confusing situations such as
/* #define LOG_H_IMPLEMENTATION
 * #define LOG_H_ENUM_PREFIX_ log_h1_
 * #define LOG_H_NAMESPACE_ log_h_
 * #include "logging.h"
 * ...
 * #undef LOG_H_IMPLEMENTATION
 * #define LOG_H_ENUM_PREFIX_ log_h2_
 * #include "logging.h"
 * // differening definition of log_h_sensitivity! (enum log_h2_LOG_H_LOG_LEVEL vs log_h1_LOG_H_LOG_LEVEL)
 * // on top of that, (if that were ignored) log_h1_set_log_level and log_h2_set_log_level both refer to the same
 * // log_h_sensitivity, which may be undesired, and will certainly cause some issues. Better a few
 * // extra symbols than all of this headache
 */

char *LOG_H_FUN(get_prefix)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level);
char *LOG_H_FUN(get_prefix_nc)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level);
char *LOG_H_FUN(get_postfix)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level);

void LOG_H_FUN(set_log_level)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level);
bool LOG_H_FUN(logif)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL));

void LOG_H_FUN(log_out)(const char* str, const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level);
void LOG_H_FUN(log_f)(FILE *out, const char *str, const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level);

void LOG_H_FUN(logf_out)(const char* fmt, const unsigned int level, ...);
void LOG_H_FUN(logf_f)(FILE *out, const char* fmt, const unsigned int level, ...);

#endif // LOGGING_H_
#ifdef LOG_H_IMPLEMENTATION

#ifdef LOG_H_STDERR_THRESHOLD
#define LOG_H_CHOOSE_OUTPUT(sensitivity) (sensitivity >= LOG_H_STDERR_THRESHOLD ? stderr : stdout)
#else
#define LOG_H_CHOOSE_OUTPUT(sensitivity) (stdout)
#endif // LOG_H_STDERR_THRESHOLD
	   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef _WIN32
# include <windows.h>
#endif // _WIN32

enum LOG_H_NAME(LOG_H_LOG_LEVEL) LOG_H_FUN(sensitivity);

char* LOG_H_FUN(get_prefix)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){
#ifdef _WIN32
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // _WIN32
	switch (level){
		case LOG_H_NAME(LOG_TRACE):
			return (char*)"[TRACE]"; // all these casts are to make C++ happy
		case LOG_H_NAME(LOG_DBG):
			return (char*)"[DEBUG]";
		case LOG_H_NAME(LOG_INFO):
			return (char*)"[INFO]";
		case LOG_H_NAME(LOG_WARN):
#ifdef _WIN32
			SetConsoleTextAttribute(hConsole, 6);
			return (char*)"[WARN]";
#else
			return (char*)"\033[93m[WARN]";
#endif // _WIN32
		case LOG_H_NAME(LOG_ERROR):
#ifdef _WIN32
			SetConsoleTextAttribute(hConsole, 4);
			return (char*)"[ERROR]";
#else
			return (char*)"\033[91m[ERROR]";
#endif // _WIN32
	}
	return (char*)"";
}

char* LOG_H_FUN(get_prefix_nc)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){
	switch (level){
		case LOG_H_NAME(LOG_TRACE):
			return (char*)"[TRACE]"; // all these casts are to make C++ happy
		case LOG_H_NAME(LOG_DBG):
			return (char*)"[DEBUG]";
		case LOG_H_NAME(LOG_INFO):
			return (char*)"[INFO]";
		case LOG_H_NAME(LOG_WARN):
			return (char*)"[WARN]";
		case LOG_H_NAME(LOG_ERROR):
			return (char*)"[ERROR]";
	}
	return (char*)"";
}

char* LOG_H_FUN(get_postfix)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){
#ifdef _WIN32
	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // _WIN32
	if(level == LOG_H_NAME(LOG_ERROR) || level == LOG_H_NAME(LOG_WARN)){
#ifdef _WIN32
		SetConsoleTextAttribute(hConsole, 0);
#endif // _WIN32
		return (char*)"\033[39m";
	}
	return (char*)"";
}

void LOG_H_FUN(set_log_level)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){
	LOG_H_FUN(sensitivity) = level;
}
bool LOG_H_FUN(logif)(const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){ 
	if(level < LOG_H_FUN(sensitivity))
		return false;
	printf("%s", LOG_H_FUN(get_prefix)(level));
	return true;
}

void LOG_H_FUN(log_out)(const char* strin, const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){
	if(level < LOG_H_FUN(sensitivity))
		return;
	char* str = (char*)malloc(strlen(strin) + 1);
	if(str == NULL)
		return; // would print a diagnostic but...
	strcpy(str, strin);
	fprintf(LOG_H_CHOOSE_OUTPUT(level), "%s %s %s%s", LOG_H_FUN(get_prefix)(level), str, LOG_H_FUN(get_postfix)(level), LOG_H_ENDL);
	free(str);
}
void LOG_H_FUN(log_f)(FILE *out, const char* strin, const enum LOG_H_NAME(LOG_H_LOG_LEVEL) level){
	if(level < LOG_H_FUN(sensitivity))
		return;
	char* str = (char*)malloc(strlen(strin) + 1);
	if(str == NULL)
		return; // would print a diagnostic but...
	strcpy(str, strin);
	fprintf(out, "%s %s %s", LOG_H_FUN(get_prefix_nc)(level), str, LOG_H_ENDL);
	free(str);
}

void LOG_H_FUN(logf_out)(const char* fmt, const unsigned int level, ...){
	if(level < LOG_H_FUN(sensitivity))
		return;
	va_list v, v1;
	va_start(v, level);
	va_copy(v1, v);
	size_t size = vsnprintf(NULL, 0, fmt, v);
	va_end(v);
	char *buf = (char*)malloc(size + 1); // cast for C++
	if(!buf){
		// what can ya do
		va_end(v1);
		return;
	}
	vsprintf(buf, fmt, v1);
	va_end(v1);
	fprintf(LOG_H_CHOOSE_OUTPUT(level), "%s %s %s%s", LOG_H_FUN(get_prefix)(level), buf, LOG_H_FUN(get_postfix)(level), LOG_H_ENDL);
	free(buf);
}

void LOG_H_FUN(logf_f)(FILE *out, const char* fmt, const unsigned int level, ...){ 
	if(level < LOG_H_FUN(sensitivity))
		return;
	va_list v, v1;
	va_start(v, level);
	va_copy(v1, v);
	size_t size = vsnprintf(NULL, 0, fmt, v);
	va_end(v);
	char *buf = (char*)malloc(size + 1); // cast for C++
	if(!buf){
		// what can ya do
		va_end(v1);
		return;
	}
	vsprintf(buf, fmt, v1);
	va_end(v1);
	fprintf(out, "%s %s %s", LOG_H_FUN(get_prefix_nc)(level), buf, LOG_H_ENDL);
	free(buf);
}
#endif // LOG_H_IMPLEMENTATION
