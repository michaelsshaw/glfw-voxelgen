#ifndef PO_UI_H_
#define PO_UI_H_

typedef void (*uicallback)();

#define BUTTON_TEXT_LEN 24

struct uibutton
{
    int VAO;
    char label[BUTTON_TEXT_LEN + 1];
    int width;
    int height;
};

#endif // PO_UI_H_
