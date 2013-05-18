#ifndef REGISTRYISP_H
#define REGISTRYISP_H

void GetReg(const char *n, const char *defval, char *buff, int buffsize);
int GetRegInt(const char *n, int defval);
void SetReg(const char *n, const char *v);
void SetRegInt(const char *n, int v);

#endif // REGISTRYISP_H
