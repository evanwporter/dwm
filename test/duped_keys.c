#include <X11/Xlib.h>
#include <criterion/criterion.h>
#include "../config.h"


Test(duped_keys, duped_keybinds)
{
    for (size_t i = 0; i < LENGTH(keys); i++) {
        for (size_t j = i + 1; j < LENGTH(keys); j++) {
            if (keys[i].mod == keys[j].mod && 
                keys[i].keysym == keys[j].keysym) {
                const char* name = XKeysymToString(keys[i].keysym);
                cr_assert_fail(
                    "Duplicate keybindings: keys[%zu] and key[%zu]:"
                    "mod = 0x%x, key = %s",
                    i, 
                    j, 
                    keys[i].mod, 
                    name ? name : "<unknown>"
                );
            } 
        }
    }
}
