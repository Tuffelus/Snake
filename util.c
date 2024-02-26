#include "util.h"

int
get_token_value(char *filename, char *token)
{
	FILE *file = fopen(filename, "a+");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Failed to open file\n");
		exit(EXIT_FAILURE);
	}
	int param_val;
	char param_name[32];
	char line[64];
	while (fgets(line, sizeof(line), file) != NULL) {
		if (sscanf(line, "%31[^=] = %d", param_name, &param_val) == 2) {
			if ((strstr(param_name, token))) {
				// fprintf(file, "read score\n");
				fclose(file);
				return param_val;
			}
		}
	}
	fclose(file);
	return FAILURE;
}

int
write_token_value(char *filename, char *token, int new_val)
{
	FILE *file = fopen(filename, "r+");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Failed to open file\n");
		exit(EXIT_FAILURE);
	}

	size_t token_len = strlen(token);
	char line[64];
	long int position = FAILURE;
	while (fgets(line, sizeof(line), file) != NULL) {
		if (strncmp(line, token, token_len) == 0) {
			position = ftell(file);
			fseek(file, position - strlen(line), SEEK_SET);

			int size = fprintf(file, "%s = %d\n", token, new_val);
			fclose(file);
			return size;
		}
	}
	fprintf(stderr, "ERROR: Parameter not in file\n");
	fclose(file);
	return FAILURE;
}

void
add_token(char *filename, char *token, int val)
{
	FILE *file = fopen(filename, "a");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Failed to open file\n");
		exit(EXIT_FAILURE);
	}

	fprintf(file, "%s = %d\n", token, val);

	fclose(file);
}
