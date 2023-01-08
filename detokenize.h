#ifndef DETOKENIZE_H
#define DETOKENIZE_H


/* BASIC mode selected */
typedef enum basic_e {
	Any, Basic2, Graphics52, TFC3, Basic7, Basic71, Basic35, Basic4, VicSuper
} basic_t;

int detokenize(const char *input_p, char *output_p, basic_t mode);

#endif /* DETOKENIZE_H */
