#include <iup.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "process.h"
#include "gui.h"

static Image *current_image = NULL;
static Image *undo_image = NULL;

static Ihandle *img_label = NULL;
static int img_handle_count = 0;

static void save_undo_state() {
    if (undo_image) free_image(undo_image);
    if (current_image) undo_image = copy_image(current_image);
    else undo_image = NULL;
}

static void refresh_display() {
    if (!current_image) {
        int w = 540, h = 470;
        unsigned char *blank = (unsigned char*) malloc(w * h * 3);
        if (blank) {
            memset(blank, 220, w * h * 3);
            Ihandle *dummy_img = IupImageRGB(w, h, blank);
            free(blank);

            char img_name[64];
            sprintf(img_name, "IMG_HANDLE_%d", ++img_handle_count);
            IupSetHandle(img_name, dummy_img);
            IupSetAttribute(img_label, "IMAGE", img_name);
        }
        return;
    }

    int max_w = 540;
    int max_h = 470;
    int disp_w = current_image->width;
    int disp_h = current_image->height;

    if (disp_w > max_w || disp_h > max_h) {
        float ratio_w = (float)max_w / disp_w;
        float ratio_h = (float)max_h / disp_h;
        float ratio = (ratio_w < ratio_h) ? ratio_w : ratio_h;
        disp_w = (int)(disp_w * ratio);
        disp_h = (int)(disp_h * ratio);
    }
    
    if (disp_w < 1) disp_w = 1;
    if (disp_h < 1) disp_h = 1;

    unsigned char *buf = (unsigned char*) malloc(disp_w * disp_h * 3);
    if (!buf) return;

    for (int y = 0; y < disp_h; y++) {
        for (int x = 0; x < disp_w; x++) {
            int src_x = x * current_image->width / disp_w;
            int src_y = y * current_image->height / disp_h;
            int src_idx = src_y * current_image->width + src_x;
            int dst_idx = (y * disp_w + x) * 3;

            buf[dst_idx + 0] = current_image->pixels[src_idx].r;
            buf[dst_idx + 1] = current_image->pixels[src_idx].g;
            buf[dst_idx + 2] = current_image->pixels[src_idx].b;
        }
    }

    Ihandle *new_iup_img = IupImageRGB(disp_w, disp_h, buf);
    free(buf);

    char img_name[64];
    sprintf(img_name, "IMG_HANDLE_%d", ++img_handle_count);
    IupSetHandle(img_name, new_iup_img);

    IupSetAttribute(img_label, "IMAGE", img_name);

    Ihandle *dlg = IupGetDialog(img_label);
    if (dlg) {
        IupRefresh(dlg);
        IupRedraw(dlg, 0);
    }
}

static int btn_open_cb(Ihandle *self) {
    Ihandle *filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(filedlg, "EXTFILTER", "BMP Files (*.bmp)|*.bmp|");

    IupPopup(filedlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(filedlg, "STATUS") != -1) {
        char *filename = IupGetAttribute(filedlg, "VALUE");
        Image *img = load_image(filename);
        if (img) {
            if (current_image) free_image(current_image);
            if (undo_image) free_image(undo_image);
            current_image = img;
            undo_image = NULL;
            refresh_display();
        } else {
            IupMessage("Error", "Could not load image! Make sure it is a valid 24-bit uncompressed BMP file.");
        }
    }
    IupDestroy(filedlg);
    return IUP_DEFAULT;
}

static int btn_save_cb(Ihandle *self) {
    if (!current_image) {
        IupMessage("Warning", "No image loaded to save!");
        return IUP_DEFAULT;
    }
    Ihandle *filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
    IupSetAttribute(filedlg, "EXTFILTER", "BMP Files (*.bmp)|*.bmp|");

    IupPopup(filedlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(filedlg, "STATUS") != -1) {
        char *filename = IupGetAttribute(filedlg, "VALUE");
        if (save_image(filename, current_image)) {
            IupMessage("Success", "BMP Image saved successfully!");
        } else {
            IupMessage("Error", "Failed to save image!");
        }
    }
    IupDestroy(filedlg);
    return IUP_DEFAULT;
}

static int btn_grayscale_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    apply_grayscale(current_image);
    refresh_display();
    return IUP_DEFAULT;
}

static int btn_brightness_cb(Ihandle *self) {
    if (!current_image) {
        IupMessage("Warning", "Load a BMP image first!");
        return IUP_DEFAULT;
    }

    int factor = 0;

    int ok = IupGetParam("Brightness Adjustment", NULL, 0, "Adjustment Value (-255 to 255): %i\n", &factor, NULL);

    if (!ok) return IUP_DEFAULT;

    if (factor < -255 || factor > 255) {
        IupMessage("Error", "Invalid Value! Enter a number between -255 and 255.");
        return IUP_DEFAULT;
    }

    save_undo_state();
    adjust_brightness(current_image, factor);
    refresh_display();

    return IUP_DEFAULT;
}

static int btn_invert_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    apply_invert(current_image);
    refresh_display();
    return IUP_DEFAULT;
}

static int btn_hflip_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    flip_horizontal(current_image);
    refresh_display();
    return IUP_DEFAULT;
}

static int btn_vflip_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    flip_vertical(current_image);
    refresh_display();
    return IUP_DEFAULT;
}

static int btn_rotate_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    Image *rotated = rotate_90(current_image);
    if (rotated) {
        free_image(current_image);
        current_image = rotated;
        refresh_display();
    }
    return IUP_DEFAULT;
}

static int btn_crop_cb(Ihandle *self) {
    if (!current_image) {
        IupMessage("Warning", "Load a BMP image first!");
        return IUP_DEFAULT;
    }

    int orig_w = current_image->width;
    int orig_h = current_image->height;

    int start_x = 0;
    int start_y = 0;
    int new_w = orig_w;
    int new_h = orig_h / 2;

    char dlg_title[128];
    sprintf(dlg_title, "Crop Image (Original: %dx%d px)", orig_w, orig_h);

    char param_fmt[256];
    sprintf(param_fmt, "Start X (0 to %d): %%i\nStart Y (0 to %d): %%i\nNew Width (px): %%i\nNew Height (px): %%i\n",
            orig_w - 1, orig_h - 1);

    int ok = IupGetParam(dlg_title, NULL, 0, param_fmt,  &start_x, &start_y, &new_w, &new_h, NULL);

    if (!ok) return IUP_DEFAULT;

    if (start_x < 0 || start_y < 0 || new_w <= 0 || new_h <= 0 ||
        (start_x + new_w > orig_w) || (start_y + new_h > orig_h)) {
        IupMessage("Error", "Invalid Crop Area! Selection goes outside the image boundary.");
        return IUP_DEFAULT;
    }

    save_undo_state();
    Image *cropped = crop_image(current_image, start_x, start_y, new_w, new_h);
    if (cropped) {
        free_image(current_image);
        current_image = cropped;
        refresh_display();
    }
    return IUP_DEFAULT;
}

static int btn_blur_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    Image *blurred = apply_blur(current_image);
    if (blurred) {
        free_image(current_image);
        current_image = blurred;
        refresh_display();
    }
    return IUP_DEFAULT;
}

static int btn_sharpen_cb(Ihandle *self) {
    if (!current_image) { IupMessage("Warning", "Load a BMP image first!"); return IUP_DEFAULT; }
    save_undo_state();
    Image *sharpened = apply_sharpen(current_image);
    if (sharpened) {
        free_image(current_image);
        current_image = sharpened;
        refresh_display();
    }
    return IUP_DEFAULT;
}

static int btn_undo_cb(Ihandle *self) {
    if (!undo_image) {
        IupMessage("Info", "Nothing to undo!");
        return IUP_DEFAULT;
    }
    Image *temp = current_image;
    current_image = undo_image;
    undo_image = NULL;
    if (temp) free_image(temp);
    refresh_display();
    return IUP_DEFAULT;
}

void start_gui(int argc, char **argv) {
    IupOpen(&argc, &argv);

    Ihandle *btn_open = IupButton("Open BMP", NULL);
    Ihandle *btn_save = IupButton("Save BMP", NULL);
    Ihandle *btn_undo = IupButton("Undo", NULL);

    Ihandle *btn_gray = IupButton("Grayscale", NULL);
    Ihandle *btn_bright = IupButton("Brightness", NULL);
    Ihandle *btn_invert = IupButton("Invert Color", NULL);
    Ihandle *btn_hflip = IupButton("Flip Horizontal", NULL);
    Ihandle *btn_vflip = IupButton("Flip Vertical", NULL);
    Ihandle *btn_rotate = IupButton("Rotate 90°", NULL);
    Ihandle *btn_crop = IupButton("Crop Image", NULL);
    Ihandle *btn_blur = IupButton("Blur", NULL);
    Ihandle *btn_sharpen = IupButton("Sharpen", NULL);

    IupSetCallback(btn_open, "ACTION", (Icallback)btn_open_cb);
    IupSetCallback(btn_save, "ACTION", (Icallback)btn_save_cb);
    IupSetCallback(btn_undo, "ACTION", (Icallback)btn_undo_cb);

    IupSetCallback(btn_gray, "ACTION", (Icallback)btn_grayscale_cb);
    IupSetCallback(btn_bright, "ACTION", (Icallback)btn_brightness_cb);
    IupSetCallback(btn_invert, "ACTION", (Icallback)btn_invert_cb);
    IupSetCallback(btn_hflip, "ACTION", (Icallback)btn_hflip_cb);
    IupSetCallback(btn_vflip, "ACTION", (Icallback)btn_vflip_cb);
    IupSetCallback(btn_rotate, "ACTION", (Icallback)btn_rotate_cb);
    IupSetCallback(btn_crop, "ACTION", (Icallback)btn_crop_cb);
    IupSetCallback(btn_blur, "ACTION", (Icallback)btn_blur_cb);
    IupSetCallback(btn_sharpen, "ACTION", (Icallback)btn_sharpen_cb);

    Ihandle *controls_box = IupVbox(
        btn_open, btn_save, btn_undo,
        IupLabel("--- Operations ---"),
        btn_gray, btn_bright, btn_invert,
        btn_hflip, btn_vflip, btn_rotate,
        btn_crop, btn_blur, btn_sharpen,
        NULL
    );
    IupSetAttribute(controls_box, "GAP", "5");
    IupSetAttribute(controls_box, "MARGIN", "10x10");

    img_label = IupLabel(NULL);
    IupSetAttribute(img_label, "EXPAND", "YES");
    IupSetAttribute(img_label, "ALIGNMENT", "ACENTER:ACENTER");

    refresh_display();

    Ihandle *main_hbox = IupHbox(controls_box, img_label, NULL);
    IupSetAttribute(main_hbox, "GAP", "15");
    IupSetAttribute(main_hbox, "MARGIN", "10x10");

    Ihandle *dlg = IupDialog(main_hbox);
    IupSetAttribute(dlg, "TITLE", "CSE1101L Image Editor");
    IupSetAttribute(dlg, "SIZE", "750x550");

    IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
    IupMainLoop();

    if (current_image) free_image(current_image);
    if (undo_image) free_image(undo_image);

    IupClose();
}
