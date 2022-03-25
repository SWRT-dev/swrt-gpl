#ifndef PRINT_H
#define PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

extern void dbg_print(char *sz, ...);
extern void print(char *sz, ...);
extern void log_buf_ctrl(int drambuf);
extern void log_ctrl(int enable);
extern int  log_status(void);
int ___printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* PRINT_H */
