#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	
	FILE *fp = fopen("test.txt", "w+");
	if (!fp) {
		puts("File open failed");
		goto fail;
	}
	int n = 10000;
	srand(time(NULL));

	while(n--) {
		fprintf(fp, "%d %d\n", rand() % 1000, rand() % 1000);
	}

	fclose(fp);
fail:
	return 0;
}
