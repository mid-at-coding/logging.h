# logging.h
A small logging library for C

# Why?

Many logging libraries, in my opinion, are either [too complicated for general usage](https://goatshriek.github.io/stumpless/), or frankly 
too [awkward to use for my preferences](https://github.com/yksz/c-logger). This is not meant to be a slight on these libraries, just a rationale
for yet another simple logging library. For this reason, I have been using this library in various iterations for 
[a couple years](https://github.com/mid-at-coding/palmak/blob/main/error.cpp) in one form or another, and gradually moved it to 
[C](https://github.com/mid-at-coding/cablegen/blob/main/inc/logging.h). 
Since my original cablegen used raylib for a frontend(and it had its own, bespoke, logging), there became a need to be able to namespace 
functions and constants, and I have essentially just taken the logging from cablegen, made it into one file, added a couple features, 
and ended up with this library ;)

# Example usage
```c
 #define LOG_H_IMPLEMENTATION
 #define LOG_H_ENUM_PREFIX_
 #define LOG_H_NAMESPACE_ 
 #include "logging.h"
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
```

# Definitions
`LOG_H_ENUM_PREFIX_:` the prefix for the sensitivity enum values


```
enum LOG_H_LOG_LEVEL:
    LOG_TRACE
    LOG_DBG
    LOG_INFO
    LOG_WARN (yellow color)
    LOG_ERROR (red color) 
```
`LOG_H_NAMESPACE_`: the prefix for the function names


`LOG_H_IMPLEMENTATION`: defines where the implementation lives

# API:
`get_prefix(level):`


returns the prefix for the given level, including setting the color if appropriate

`char* get_prefix_nc(level):`


returns the prefix for the given level, without color (used in _f variants)

`char* get_postfix(level):`


returns the postfix for the given level, mostly just resets the color of the terminal (not called for _f variants)

`set_log_level(level):`


stops logs under the given level from outputting

`bool logif(level):`


returns true if the given level is equal or above the set level

`log_out(str, level):`


outputs str to stdout with the appropriate prefix and a newline

`logf_out(fmt, level, ...):`


outputs a formatted string to stdout with the appropriate prefix

`log_f(fp, str, level):`


outputs str to a file with the appropriate prefix and a newline

`logf_f(fp, fmt, level, ...):`


outputs a formatted string to a file with the appropriate prefix
