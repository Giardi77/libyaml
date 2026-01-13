/* src/uaf-demo.c
 *
 * Deliberate use-after-free example to test static analyzers
 * Uses exactly the same style as the real yaml_parser_delete()
 */

#include "yaml_private.h"   // ← important: gives access to internal types & macros

void
yaml_parser_delete_uaf_demo(yaml_parser_t *parser)
{
    yaml_tag_directive_t tag_directive;

    // Simulate getting a tag directive from the stack (like real code)
    // In real libyaml this comes from POP(parser->tag_directives)
    tag_directive.handle = NULL;
    tag_directive.prefix = NULL;

    // Pretend it was allocated earlier (this is what normally happens)
    tag_directive.handle = yaml_malloc(32);
    if (tag_directive.handle)
    {
        memcpy(tag_directive.handle, "!!demo\0", 8);
    }

    tag_directive.prefix = yaml_malloc(64);
    if (tag_directive.prefix)
    {
        memcpy(tag_directive.prefix, "tag:example.com,2025:demo/\0", 28);
    }

    // ───────────────────────────────────────────────────────────────
    //           THE DELIBERATE USE-AFTER-FREE BUG
    // ───────────────────────────────────────────────────────────────

    if (tag_directive.handle)
    {
        yaml_free(tag_directive.handle);           // ← free

        // Some unrelated parser cleanup code...
        yaml_free(tag_directive.prefix);

        // ────────────────────── UAF ──────────────────────
        // This is the exact pattern you are trying to detect
        char first_byte = *tag_directive.handle;   // ← use after free

        // Just to make sure the compiler doesn't optimize it away
        if (first_byte == 0)
            parser->error = YAML_MEMORY_ERROR;
        // ─────────────────────────────────────────────────
    }

    // Pretend we continue cleaning up other things...
}