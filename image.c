#include "image.h"
#include "utils.h"
#include "blas.h"
#include "cuda.h"
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "test_detector.h"
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


...................................

void draw_detections(image im, int num, float thresh, box *boxes, float **probs, float **masks, char **names, image **alphabet, int classes)
{
    int i;

    for (i = 0; i < num; ++i) {
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if (prob > thresh) {
            int width = im.h * .006;

            if (0) {
                width = pow(prob, 1. / 2.) * 10 + 1;
                alphabet = 0;
            }
            //printf("%d %s: %.0f%%\n", i, names[class], prob*100);
            printf("%s: %.0f%%\n", names[class], prob * 100);
            int offset = class * 123457 % classes;
            float red = get_color(2, offset, classes);
            float green = get_color(1, offset, classes);
            float blue = get_color(0, offset, classes);
            float rgb[3];

            //width = prob*20+2;

            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
            box b = boxes[i];

            int left  = (b.x - b.w / 2.) * im.w;
            int right = (b.x + b.w / 2.) * im.w;
            int top   = (b.y - b.h / 2.) * im.h;
            int bot   = (b.y + b.h / 2.) * im.h;

            if (left < 0) left = 0;
            if (right > im.w - 1) right = im.w - 1;
            if (top < 0) top = 0;
            if (bot > im.h - 1) bot = im.h - 1;

//***************************** 此处为添加的bounding-box:target_p的限制条件(本文重构了taget_p作为bounding-box，所以与上方有所区别) **********************************//
            if((target_p[t_i].rect.x + target_p[t_i].rect.width) > im.w - 1){ target_p[t_i].rect.x = im.w - (target_p[t_i].rect.width + 1);}
            if((target_p[t_i].rect.y + target_p[t_i].rect.height) > im.h - 1){ target_p[t_i].rect.y = im.h - (target_p[t_i].rect.height + 1);}
            if(target_p[t_i].rect.x < 0 ) {target_p[t_i].rect.x = 0 ;}
            if(target_p[t_i].rect.y < 0 ) {target_p[t_i].rect.y = 0 ;}
            if(target_p[t_i].rect.width <= 0 ) {target_p[t_i].rect.width = 1 ;}
            if(target_p[t_i].rect.height <= 0 ) {target_p[t_i].rect.height = 1 ;}
            if(target_p[t_i].rect.width > im.w - 1 ) {target_p[t_i].rect.width = 1 ;}
            if(target_p[t_i].rect.height > im.h - 1)  {target_p[t_i].rect.height = 1 ;}



            draw_box_width(im, left, top, right, bot, width, red, green, blue);

            if (alphabet) {
                printf("names[class] = %s\n", names[class]);
                image label = get_label(alphabet, names[class], (im.h * .03) / 10);
                draw_label(im, top + width, left, label, rgb);
                free_image(label);
            }
            if (masks) {
                image mask = float_to_image(14, 14, 1, masks[i]);
                image resized_mask = resize_image(mask, b.w * im.w, b.h * im.h);
                image tmask = threshold_image(resized_mask, .5);
                embed_image(tmask, im, left, top);
                free_image(mask);
                free_image(resized_mask);
                free_image(tmask);
            }
        }
    }
}

....................................
