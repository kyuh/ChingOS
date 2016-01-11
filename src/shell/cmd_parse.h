#include "shell_utils.h"
#include <stdbool.h>

CmdChain parseCmds(char *buf);
StringArray sepStringWithQuotes(char *buf, char separator, bool exclude_quote_mark);