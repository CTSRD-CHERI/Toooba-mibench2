void putchar(char);
void exit(int);

extern unsigned int end;
//extern unsigned int heapCurrent;
char heap[1<<16];
char * heapCurrent = heap;

void _fini() {}
void __exidx_start() {}
void __exidx_end() {}

void __errno() {}

int _isatty()
{
  return 0;
}
int _fstat()
{
  return 0;
}

void puts(char *string)
{
    int index = 0;
    while(string[index] != '\0')
    {
        putchar(string[index]);
        ++index;
    }
}

void * malloc(int increment)
{
    void * retval = (void *) heapCurrent;
    heapCurrent += increment;
    
    if(heapCurrent >= (heap + (1<<16)))
    {
        exit(1);
    }
    
    return retval;
}

void free(void * ignorned) {}

void * _sbrk(int increment)
{
    heapCurrent += increment;
    
    if(heapCurrent >= (heap + (1<<16)))
    {
        exit(1);
    }
    
    return (void *)heapCurrent;
}

/*
int rand(void)
{
    return 7;
}

int srand(void)
{
    return 11;
}
*/

// Default behavior is for GCC to send printf output here
int _write(int fd, const unsigned char *buf, int count)
{
    int cnt;
    for(cnt = 0; cnt < count; ++cnt)
    {
        putchar(*buf);
	++buf;
    }

    return cnt;
}

void _close()
{
    return;
}

void _read()
{
    return;
}

void _lseek()
{
    return;
}
