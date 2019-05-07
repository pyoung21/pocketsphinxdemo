#include <cstdio>
#include <cstdint>
#include <array>
#include <fstream>

#include "pocketsphinx.h"

#pragma comment(lib, "sphinxbase.lib")
#pragma comment(lib, "pocketsphinx.lib")

#define MODELDIR "../PocketSphinx/model"

#ifdef _DEBUG
#undef _DEBUG
#endif

int main()
{
	ps_decoder_t	*ps;
	cmd_ln_t		*config;
	FILE			*file;
	const char		*hyp;
	int				ret;
	int32_t			score;

	const char fileName[] = "goforward.raw";
	std::array<int16_t, 512> buffer;

	config = cmd_ln_init(
		nullptr,
		ps_args(), 
		true,
		"-hmm", MODELDIR "/fr-fr/fr-fr",
		"-lm", MODELDIR "/fr-fr/fr-small.lm.bin",
		"-dict", MODELDIR "/fr-fr/fr.dict",
		//"-logfn", "nul",
		//"-allphone", "yes",
		NULL);

	if (config == nullptr)
	{
		fprintf(stderr, "Failed to create config object, see log for details\n");
		return -1;
	}

	ps = ps_init(config);
	if (ps == nullptr) 
	{
		fprintf(stderr, "Init failed\n");
		return -1;
	}

	file = fopen(fileName, "rb");
	if (file == NULL) 
	{
		fprintf(stderr, "Failed to open input file '%s'.\n", fileName);
		return -1;
	}

	ret = ps_start_utt(ps);

	while (!feof(file))
	{
		size_t numberOfSamples = fread(buffer.data(), 2, buffer.size(), file);
		ret = ps_process_raw(ps, buffer.data(), numberOfSamples, false, false);
	}

	ret = ps_end_utt(ps);
	hyp = ps_get_hyp(ps, &score);

	printf("Recognized: %s\n", hyp);

	fclose(file);
	ps_free(ps);
	cmd_ln_free_r(config);

	return 0;
}