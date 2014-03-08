
#ifndef __WCN_UTILE_H__
#define __WCN_UTILE_H__

int to_upper(char *str);
int to_upper_n(char *str, int len);
int check_hex_str(char *str, int len);
int check_dec_str(char *str, int len);
int ascii_to_hex(char *in_ascii, uint16_t *out_hex);
int ascii_to_dec(char *in_ascii, int *out_dec);
int trim_string(char **start);
int trim_path(char **start);
int get_int_val(char *src_val, int *dst_val);

#endif //__WCN_UTILE_H__