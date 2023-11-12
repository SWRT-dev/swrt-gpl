#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#define FIVE_IFACE "test5"
#define TWO_IFACE "test2"

#define FIVE_CLIENT "50:31:32:33:34:35"
#define PIN "24033848"

char *multi_tok(char *input, char *delimiter);
struct json_object *poll_test_log(FILE *fp, const char *prefix);
struct uci_package *init_package(struct uci_context **ctx, const char *config);
int cp(const char *to, const char *from);

#endif