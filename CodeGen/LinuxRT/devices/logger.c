#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pyblock.h>

struct _logger {
	double * buff;
	FILE * fp;
};

static void logger_init(python_block * blk);
static void logger_out(python_block * blk);
static void logger_end(python_block * blk);
static void logger_clean_up(struct _logger * logger);

void logger(int flag, python_block * blk)
{
	switch (flag) {
	case CG_INIT:
		logger_init(blk);
		break;
	case CG_OUT:
		logger_out(blk);
		break;
	case  CG_END:
		logger_end(blk);
		break;
	}
}

static void logger_init(python_block * blk)
{
	/* get logger struct */
	struct _logger * logger = malloc(sizeof(*logger));
	if (!logger) {
		fprintf(stderr, "Mem error in logger init\n");
		exit(EXIT_FAILURE);
	}
	/* create internal buffer */
	size_t buff_len = blk->nin * sizeof(*logger->buff);
	logger->buff = malloc(buff_len);
	if (!logger->buff) {
		fprintf(stderr, "Mem error in logger init\n");
		free(logger);
		exit(EXIT_FAILURE);
	}
	/* open log-file */
	if (!blk->str) {
		fprintf(stderr, "Error in logger init, "
		                "no log-file specified\n");
		free(logger->buff);
		free(logger);
		exit(EXIT_FAILURE);
	}
	logger->fp = fopen(blk->str, "w");
	if (!logger->fp) {
		perror("fopen");
		free(logger->buff);
		free(logger);
		exit(EXIT_FAILURE);
	}
	/* append logger to blk */
	blk->ptrPar = (void *)logger;
}

static void logger_out(python_block * blk)
{
	struct _logger * logger = (struct _logger *)blk->ptrPar;
	double * buff = logger->buff;
	unsigned nin = blk->nin;
	FILE * fp = logger->fp;
	/* write u's */
	for (unsigned i = 0; nin > i; i++)
		buff[i] = *(double *)blk->u[i];
	/* write to file */
	if (fwrite((const void *)buff, sizeof(*buff), nin, fp) != nin) {
		fprintf(stderr, "Error in "
		                "logger_out writing file %s\n", blk->str);
		logger_clean_up(logger);
		exit(EXIT_FAILURE);
	}
}

static void logger_clean_up(struct _logger * logger)
{
	/* close log-file */
	if (fclose(logger->fp))
		perror("fclose");
	/* free internal buffer */
	free(logger->buff);
	/* free logger struct */
	free(logger);
}

static void logger_end(python_block * blk)
{
	struct _logger * logger = (struct _logger *)blk->ptrPar;
	/* clean up */
	logger_clean_up(logger);
}
