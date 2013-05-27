#ifndef REGISTRYISP_H
#define REGISTRYISP_H

int get_reg(const char *n, const char *defval, char *buff, int buffsize);
int get_reg_int(const char *n, int defval);
void set_reg(const char *n, const char *v);
void set_reg_int(const char *n, int v);

#endif // REGISTRYISP_H
