
#include <stdarg.h>
#include "term.h"
#include "kstdlib.h"
#include "kstring.h"
#include "kstdio.h"

int kprintf(const char *pFormat, ...) {
	int charsWritten = 0;
	char buf[32];
	va_list args;
    
    kmemset(buf, 0, 32);
    va_start(args, pFormat);
    
    char *fmt = (char*)pFormat;
    while(*fmt != 0) {
    	if(*fmt == '%' && *(fmt + 1) != 0) {
    		fmt++;
    		switch(*fmt) {
    			case '%':
    				kterm_putchar('%');
    				break;
    				
    			case 'c':
    				kterm_putchar((char)va_arg(args, int32_t));
    				break;
    				
    			case 's':
    				kterm_puts(va_arg(args, char*));
    				break;
    				
    			case 'i':
    			case 'd':
    				kterm_puts(kitoa(va_arg(args, int32_t), buf, 10));
    				break;
    				
    			case 'I':
    			case 'D':
    				kterm_puts(klitoa(va_arg(args, int64_t), buf, 10));
    				break;
    				
    			case 'u':
					kterm_puts(kutoa(va_arg(args, uint32_t), buf, 10));
    				break;
    				
    			case 'U':
    				kterm_puts(klutoa(va_arg(args, uint64_t), buf, 10));
    				break;
    				
    			case 'x':
    				kterm_puts(kutoa(va_arg(args, uint32_t), buf, 16));
    				break;
    				
    			case 'y':
    				kterm_puts(klutoa(va_arg(args, uint64_t), buf, 16));
    				break;
    				
    			case 'X':
    				kterm_puts(kstrtoupper(kutoa(va_arg(args, uint32_t), buf, 16)));
    				break;
    				
    			case 'Y':
    				kterm_puts(kstrtoupper(klutoa(va_arg(args, uint64_t), buf, 16)));
    				break;
    		}
    	}
    	else {
    		kterm_putchar(*fmt);
    		charsWritten++;
    	}
    	
    	fmt++;
    }
    
    va_end(args);
    
    return charsWritten;
}

