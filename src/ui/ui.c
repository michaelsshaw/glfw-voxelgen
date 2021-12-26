#include <string.h>
#include <stdlib.h>

#include "ui.h"

struct uibutton
*ui_button_create(const char *label, const uicallback onclick)
{
    struct uibutton *newbutton = malloc(sizeof(struct uibutton));

    int len = strlen(label);
    strncpy(newbutton->label, label, BUTTON_TEXT_LEN);
}
