/* {{name}}: a WebAssembly mod. lce.h is generated from the game's API; see docs/modding.md, "WebAssembly API". */
#include "lce.h"

static char buffer[256];

__attribute__((export_name("lce_alloc"))) void* lce_alloc(int size)
{
    return size <= (int)sizeof(buffer) ? buffer : 0;
}

__attribute__((export_name("lce_init"))) void lce_init(void)
{
    lce_register_command("{{id}}", (int)strlen("{{id}}"));
    lce_log("{{name}} started", (int)strlen("{{name}} started"));
}

/* the text at ptr is the command name, the sender and the arguments, joined by newlines */
__attribute__((export_name("lce_command"))) int lce_command(const char* ptr, int length)
{
    (void)ptr;
    (void)length;
    lce_reply("Hello from {{name}}", (int)strlen("Hello from {{name}}"));
    return 1;
}
