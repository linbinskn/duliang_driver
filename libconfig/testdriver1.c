#include <libconfig.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>



int main(int argc, char **argv)
{
	for(int i = 1; i < argc; i++){
		config_t cfg;
		config_init(&cfg);
		config_set_options(&cfg, CONFIG_OPTION_ALLOW_OVERRIDES);
		int ok = config_read_file(&cfg, argv[i]);

		const char *str;
		ok = config_lookup_string(&cfg, "escape_seqs.str", &str);
		ok = config_lookup_string(&cfg, "escape_seqs.tab", &str);
		ok = config_lookup_string(&cfg, "escape_seqs.feed", &str);
		ok = config_lookup_string(&cfg, "escape_seqs.newline", &str);
		ok = config_lookup_string(&cfg, "escape_seqs.dquote", &str);
		ok = config_lookup_string(&cfg, "escape_seqs.backslash", &str);


		config_destroy(&cfg);
	}
	
	char *buf;
  	config_t cfg;
  	int rc;
  	int ival = 123;
  	long long llval;

  	buf = "someint=-2147483649;"; /* -2^31-1 */

  	config_init(&cfg);
  	rc = config_read_string(&cfg, buf);

  	/* Should fail because value was parsed as an int64. */
  	rc = config_lookup_int(&cfg, "someint", &ival);

  	rc = config_lookup_int64(&cfg, "someint", &llval);

  	config_destroy(&cfg);

	return 0;
}
