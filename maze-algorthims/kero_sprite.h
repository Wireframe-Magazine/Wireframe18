#if !defined(KERO_SPRITE_H)

#ifdef __cplusplus
extern "C"{
#endif
    
#include <string.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"
    
#define KSMax(a, b) ((a)>(b)?(a):(b))
#define KSMin(a, b) ((a)<(b)?(a):(b))
#define KSAbsolute(a) ((a)>0?(a):(-(a)))
#define KSSquare(a) ((a)*(a))
    float KSSquareRootf(float a)
    {
        float precision = a*0.00001f;
        float root = a;
        while ((root - a/root) > precision)
        {
            root= (root + a/root) / 2;
        }
        return root;
    }
#define KSLineLength(ax,ay,bx,by) KSSquareRootf(KSSquare((float)(bx)-(float)(ax))+KSSquare((float)(by)-(float)(ay)))
    int KSRoundf(float a){ return (int)(a+0.5f); }
#define KSSwap(type, a, b) {type t = a; a = b; b = t;}
    
#define KSBLACK 0xff000000
#define KSWHITE 0xffffffff
    
    typedef struct{
        int w,h;
        uint32_t* pixels;
    } ksprite_t;
    
    bool KSLoad(ksprite_t* sprite, char* filepath){
        sprite->pixels = (uint32_t*)stbi_load(filepath, &sprite->w, &sprite->h, 0, 4);
        if(!sprite->pixels){
            return false;
        }
        for(int y = 0; y < sprite->h; ++y){
            for(int x = 0; x < sprite->w; ++x){
                uint8_t* b = (uint8_t*)(sprite->pixels + y*sprite->w + x);
                uint8_t* r = b + 2;
                uint8_t true_red = *b;
                *b = *r;
                *r = true_red;
            }
        }
        return true;
    }
    
    static inline void KSCreate(ksprite_t* sprite, int w, int h){
        sprite->w = w;
        sprite->h = h;
        sprite->pixels = (uint32_t*)malloc(w*h*sizeof(uint32_t));
    }
    
    static inline void KSFree(ksprite_t* sprite) {
        free(sprite->pixels);
    }
    
    void KSCreateFromSprite(ksprite_t* sprite, ksprite_t* source, int left, int top, int right, int bottom) {
        KSCreate(sprite, right-left, bottom-top);
        for(int y = 0; y < sprite->h; ++y){
            for(int x = 0; x < sprite->w; ++x){
                sprite->pixels[y*sprite->w+x] = source->pixels[(y+top)*source->w + x+left];
            }
        }
    }
    
    static inline uint32_t KSGetPixel(ksprite_t* source, int x, int y){
        return source->pixels[y*source->w + x];
    }
    
    static inline uint32_t KSSampleWrapped(ksprite_t* source, float x, float y){
        int ix = Absolute(((int)(x*source->w))%source->w);
        int iy = Absolute(((int)(y*source->h))%source->h);
        return source->pixels[ix + iy*source->w];
    }
    
    static inline uint32_t KSGetPixelSafe(ksprite_t* source, int x, int y){
        if(x < 0 || x > source->w-1 || y < 0 || y > source->h-1) return 0;
        return source->pixels[y*source->w + x];
    }
    
    static inline void KSSetPixel(ksprite_t* dest, int x, int y, uint32_t pixel){
        *(dest->pixels + y*dest->w + x) = pixel;
    }
    
    static inline void KSSetPixelSafe(ksprite_t* dest, int x, int y, uint32_t pixel){
        if(x < 0 || x > dest->w-1 || y < 0 || y > dest->h-1) return;
        *(dest->pixels + y*dest->w + x) = pixel;
    }
    
    static inline void KSSetPixelAlpha10(ksprite_t* dest, int x, int y, uint32_t pixel) {
        if(pixel>>24 == 0 || x < 0 || x > dest->w-1 || y < 0 || y > dest->h-1) return;
        *(dest->pixels + y*dest->w + x) = pixel;
    }
    
    void KSSetPixelWithAlpha(ksprite_t* dest, int x, int y, uint32_t pixel){
        if(pixel>>24 == 0 || x < 0 || x > dest->w-1 || y < 0 || y > dest->h-1) return;
        *(dest->pixels + y*dest->w + x) = pixel;
    }
    
    void KSColorToTransparent(ksprite_t* sprite, uint32_t color) {
        for(int y = 0; y < sprite->h; ++y) {
            for(int x = 0; x < sprite->w; ++x) {
                if(sprite->pixels[y*sprite->w+x] == color) {
                    sprite->pixels[y*sprite->w+x] = 0;
                }
            }
        }
    }
    
    void KSColorKey(ksprite_t* sprite, uint32_t color_before, uint32_t color_after) {
        for(int y = 0; y < sprite->h; ++y) {
            for(int x = 0; x < sprite->w; ++x) {
                if(sprite->pixels[y*sprite->w+x] == color_before) {
                    sprite->pixels[y*sprite->w+x] = color_after;
                }
            }
        }
    }
    
    void KSBlendPixel(ksprite_t* dest, int x, int y, uint32_t pixel){
        if(x < 0 || x > dest->w-1 || y < 0 || y > dest->h-1) return;
        uint8_t* dest_pixel = (uint8_t*)(dest->pixels + y*dest->w + x);
        float alpha = (float)(pixel>>24) / 255.f;
        float one_minus_alpha = 1.f - alpha;
        *dest_pixel = (uint8_t)(pixel) * alpha + (*dest_pixel) * one_minus_alpha;
        ++dest_pixel;
        *dest_pixel = (uint8_t)(pixel>>8) * alpha + (*dest_pixel) * one_minus_alpha;
        ++dest_pixel;
        *dest_pixel = (uint8_t)(pixel>>16) * alpha + (*dest_pixel) * one_minus_alpha;
        ++dest_pixel;
        *dest_pixel += (255 - *dest_pixel) * alpha;
    }
    
    static inline void KSBlit(ksprite_t* source, ksprite_t* dest, int x, int y){
        int left = x;
        int top = y;
        int left_clip = KSMax(0, -left);
        int right_clip = KSMax(0, left + source->w - dest->w);
        int top_clip = KSMax(0, -top);
        int bottom_clip = KSMax(0, top + source->h - dest->h);
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                KSSetPixel(dest, left+sourcex, top+sourcey, *(source->pixels + sourcey*source->w + sourcex));
            }
        }
    }
    
    static inline void KSBlitBlend(ksprite_t* source, ksprite_t* dest, int x, int y){
        int left = x;
        int top = y;
        int left_clip = KSMax(0, -left);
        int right_clip = KSMax(0, left + source->w - dest->w);
        int top_clip = KSMax(0, -top);
        int bottom_clip = KSMax(0, top + source->h - dest->h);
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                KSBlendPixel(dest, left+sourcex, top+sourcey, *(source->pixels + sourcey*source->w + sourcex));
            }
        }
    }
    
    static inline void KSBlitScaled(ksprite_t* sprite, ksprite_t* target, int x, int y, float scalex, float scaley, int originx, int originy){
        int left = x - originx*scalex;
        int top = y - originy*scaley;
        for(int sy = 0; sy < sprite->h; sy++){
            for(int sx = 0; sx < sprite->w; sx++){
                uint32_t source_pixel = KSGetPixel(sprite, sx, sy);
                int scaled_sourcex = sx*scalex;
                int scaled_sourcey = sy*scaley;
                for(int oy = KSMin(0,scaley); oy < KSMax(scaley,0); oy++){
                    for(int ox = KSMin(0,scalex); ox < KSMax(scalex,0); ox++){
                        KSSetPixel(target, left + scaled_sourcex + ox, top + scaled_sourcey + oy, source_pixel);
                    }
                }
            }
        }
    }
    
    static inline void KSBlitScaledAlpha10(ksprite_t* sprite, ksprite_t* target, int x, int y, float scalex, float scaley, int originx, int originy){
        int left = x - originx*scalex;
        int top = y - originy*scaley;
        for(int sy = 0; sy < sprite->h; sy++){
            for(int sx = 0; sx < sprite->w; sx++){
                uint32_t source_pixel = KSGetPixel(sprite, sx, sy);
                int scaled_sourcex = sx*scalex;
                int scaled_sourcey = sy*scaley;
                for(int oy = KSMin(0,scaley); oy < KSMax(scaley,0); oy++){
                    for(int ox = KSMin(0,scalex); ox < KSMax(scalex,0); ox++){
                        KSSetPixelAlpha10(target, left + scaled_sourcex + ox, top + scaled_sourcey + oy, source_pixel);
                    }
                }
            }
        }
    }
    
    void KSBlitScaledSafe(ksprite_t* sprite, ksprite_t* target, int x, int y, float scalex, float scaley, int originx, int originy){
        int left = x - originx*scalex;
        int top = y - originy*scaley;
        for(int sy = 0; sy < sprite->h; sy++){
            for(int sx = 0; sx < sprite->w; sx++){
                uint32_t source_pixel = KSGetPixel(sprite, sx, sy);
                int scaled_sourcex = sx*scalex;
                int scaled_sourcey = sy*scaley;
                for(int oy = KSMin(0,scaley); oy < KSMax(scaley,0); oy++){
                    for(int ox = KSMin(0,scalex); ox < KSMax(scalex,0); ox++){
                        KSSetPixelSafe(target, left + scaled_sourcex + ox, top + scaled_sourcey + oy, source_pixel);
                    }
                }
            }
        }
    }
    
    void KSBlitAlpha10(ksprite_t* source, ksprite_t* dest, int x, int y){
        int left = x;
        int top = y;
        int left_clip = KSMax(0, -left);
        int right_clip = KSMax(0, left + source->w - dest->w);
        int top_clip = KSMax(0, -top);
        int bottom_clip = KSMax(0, top + source->h - dest->h);
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                KSSetPixelAlpha10(dest, left+sourcex, top+sourcey, source->pixels[sourcey*source->w + sourcex]);
            }
        }
    }
    
    void KSBlitAlpha10Flip(ksprite_t* source, ksprite_t* dest, int x, int y){
        int left = x;
        int top = y;
        int left_clip = KSMax(0, -left);
        int right_clip = KSMax(0, left + source->w - dest->w);
        int top_clip = KSMax(0, -top);
        int bottom_clip = KSMax(0, top + source->h - dest->h);
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                KSSetPixelAlpha10(dest, left+sourcex, top+sourcey, source->pixels[sourcey*source->w + (source->w-right_clip-sourcex-1)]);
            }
        }
    }
    
    void KSBlitColored(ksprite_t* source, ksprite_t* dest, int x, int y, int originx, int originy, uint32_t colour){
        int left = x - originx;
        int top = y - originy;
        int left_clip = KSMax(0, -left);
        int right_clip = KSMax(0, left + source->w - dest->w);
        int top_clip = KSMax(0, -top);
        int bottom_clip = KSMax(0, top + source->h - dest->h);
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                uint32_t source_pixel = source->pixels[sourcey*source->w + sourcex];
                float bratio = ((uint8_t)source_pixel)/255.f;
                float gratio = ((uint8_t)(source_pixel>>8))/255.f;
                float rratio = ((uint8_t)(source_pixel>>16))/255.f;
                uint8_t a = (uint8_t)(source_pixel>>24);
                KSSetPixel(dest, left+sourcex, top+sourcey, (uint32_t)(bratio*((uint8_t)colour)) | (uint32_t)(gratio*((uint8_t)(colour>>8)))<<8 | (uint32_t)(rratio*((uint8_t)(colour>>16)))<<16 | a<<24);
            }
        }
    }
    
    void KSBlitColoredAlpha10(ksprite_t* source, ksprite_t* dest, int x, int y, int originx, int originy, uint32_t colour){
        int left = x - originx;
        int top = y - originy;
        int left_clip = KSMax(0, -left);
        int right_clip = KSMax(0, left + source->w - dest->w);
        int top_clip = KSMax(0, -top);
        int bottom_clip = KSMax(0, top + source->h - dest->h);
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                uint32_t source_pixel = source->pixels[sourcey*source->w + sourcex];
                float bratio = ((uint8_t)source_pixel)/255.f;
                float gratio = ((uint8_t)(source_pixel>>8))/255.f;
                float rratio = ((uint8_t)(source_pixel>>16))/255.f;
                uint8_t a = (uint8_t)(source_pixel>>24);
                KSSetPixelAlpha10(dest, left+sourcex, top+sourcey, (uint32_t)(bratio*((uint8_t)colour)) | (uint32_t)(gratio*((uint8_t)(colour>>8)))<<8 | (uint32_t)(rratio*((uint8_t)(colour>>16)))<<16 | a<<24);
            }
        }
    }
    
    static inline void KSClear(ksprite_t* s) {
        memset(s->pixels, 0, sizeof(s->pixels[0]) * s->w * s->h);
    }
    
    void KSSetAllPixelComponents(ksprite_t* sprite, uint8_t component){
        memset(sprite->pixels, component, sprite->w*sprite->h*4);
    }
    
    void KSSetAllPixels(ksprite_t* sprite, uint32_t pixel){
        for(int i = 0; i < sprite->h*sprite->w; ++i){
            *(sprite->pixels + i) = pixel;
        }
    }
    
    static inline void KSDrawLineVertical(ksprite_t* dest, int x, int y0, int y1, uint32_t pixel) {
        if(y0 > y1){
            KSSwap(int, y0, y1);
        }
        for(; y0 <= y1; ++y0){
            KSSetPixel(dest, x, y0, pixel);
        }
    }
    
    static inline void KSDrawLineVerticalSafe(ksprite_t* dest, int x, int y0, int y1, uint32_t pixel) {
        if(y0 > y1){
            KSSwap(int, y0, y1);
        }
        y0 = Min(dest->h-1, Max(0, y0));
        y1 = Min(dest->h-1, Max(0, y1));
        for(; y0 <= y1; ++y0){
            KSSetPixel(dest, x, y0, pixel);
        }
    }
    
    // Got this from https://github.com/miloyip/line/blob/master/line_bresenham.c
    void KSDrawLine(ksprite_t* dest, int x0, int y0, int x1, int y1, uint32_t pixel){
        int dx = KSAbsolute(x1 - x0);
        int dy = KSAbsolute(y1 - y0);
        if(dx == 0){
            KSDrawLineVerticalSafe(dest, x0, y0, y1, pixel);
        }
        int sx = x0<x1 ? 1:-1;
        int sy = y0<y1 ? 1:-1;
        int err = (dx>dy ? dx:-dy) / 2;
        while (KSSetPixelSafe(dest, x0, y0, pixel), x0 != x1 || y0 != y1) {
            int e2 = err;
            if (e2 > -dx) { err -= dy; x0 += sx; }
            if (e2 <  dy) { err += dx; y0 += sy; }
        }
    }
    
    void KSDrawLinef(ksprite_t* dest, float x0, float y0, float x1, float y1, uint32_t pixel){
        float length = KSLineLength(x0, y0, x1, y1);
        float dx = (x1-x0)/length;
        float dy = (y1-y0)/length;
        for(float  d = 0; d < length; ++d){
            KSSetPixel(dest, x0 + d * dx, y0 + d * dy, pixel);
        }
    }
    
    void KSDrawRect(ksprite_t* dest, int x1, int y1, int x2, int y2, uint32_t pixel){
        int left = KSMax(0, KSMin(x1, x2));
        int right = KSMin(dest->w-1, KSMax(x1, x2));
        int top = KSMax(0, KSMin(y1, y2));
        int bottom = KSMin(dest->h-1, KSMax(y1, y2));
        if(left > dest->w-1 || right < 0 || top > dest->h-1 || bottom < 0) return;
        KSDrawLine(dest, left, top, right, top, pixel);
        KSDrawLine(dest, left, bottom, right, bottom, pixel);
        KSDrawLine(dest, left, top, left, bottom, pixel);
        KSDrawLine(dest, right, top, right, bottom, pixel);
    }
    
    static inline void KSScanLine(ksprite_t* dest, int y, int x0, int x1, uint32_t pixel){
        if(y < 0 || y > dest->h-1)return;
        int left = KSMin(x0, x1);
        int right = KSMax(x0, x1);
        if(right < 0 || left > dest->w-1)return;
        left = KSMax(0, left);
        right = KSMin(dest->w-1, right);
        for(int x = left; x <= right; ++x){
            KSSetPixel(dest, x, y, pixel);
        }
    }
    
    static inline void KSScanLineAlpha(ksprite_t* dest, int y, int x0, int x1, uint32_t pixel){
        if(y < 0 || y > dest->h-1)return;
        int left = KSMin(x0, x1);
        int right = KSMax(x0, x1);
        if(right < 0 || left > dest->w-1)return;
        left = KSMax(0, left);
        right = KSMin(dest->w-1, right);
        for(int x = left; x <= right; ++x){
            KSBlendPixel(dest, x, y, pixel);
        }
    }
    
    static inline void KSDrawRectFilled(ksprite_t* dest, int x1, int y1, int x2, int y2, uint32_t pixel){
        int left = KSMax(0, KSMin(x1, x2));
        int right = KSMin(dest->w-1, KSMax(x1, x2));
        int top = KSMax(0, KSMin(y1, y2));
        int bottom = KSMin(dest->h-1, KSMax(y1, y2));
        if(left > dest->w-1 || right < 0 || top > dest->h-1 || bottom < 0) return;
        for(int y = top; y < bottom+1; ++y){
            KSScanLine(dest, y, left, right, pixel);
        }
    }
    
    static inline void KSDrawRectFilledAlpha(ksprite_t* dest, int x1, int y1, int x2, int y2, uint32_t pixel){
        int left = KSMax(0, KSMin(x1, x2));
        int right = KSMin(dest->w-1, KSMax(x1, x2));
        int top = KSMax(0, KSMin(y1, y2));
        int bottom = KSMin(dest->h-1, KSMax(y1, y2));
        if(left > dest->w-1 || right < 0 || top > dest->h-1 || bottom < 0) return;
        for(int y = top; y < bottom+1; ++y){
            KSScanLineAlpha(dest, y, left, right, pixel);
        }
    }
    
    /*void KSDrawTriangleFlatBottom(ksprite_t* dest, float x, float y, float left, float right, float bottom, uint32_t pixel){
        float dl = left-x;
        float dr = right-x;
        float height = bottom-y;
        for(float liney = y; liney <= bottom; ++liney){
            float height_ratio = (liney-y)/height;
            KSScanLine(dest, liney, x+dl*height_ratio, x+dr*height_ratio, pixel);
        }
    }
    
    void KSDrawTriangleFlatTop(ksprite_t* dest, float x, float y, float left, float right, float top, uint32_t pixel){
        float dl = left-x;
        float dr = right-x;
        float height = top-y;
        for(float liney = top; liney <= y; ++liney){
            float height_ratio = (liney-y)/height;
            KSScanLine(dest, liney, x+dl*height_ratio, x+dr*height_ratio, pixel);
        }
    }
    
    void KSDrawTriangle(ksprite_t* dest, float x0, float y0, float x1, float y1, float x2, float y2, uint32_t pixel){
        // Sort vertices vertically so v0y <= v1y <= v2y
        if(y0 > y1){
            float xt = x0, yt = y0;
            x0 = x1;y0 = y1;
            x1 = xt;y1 = yt;
        }
        if(y1 > y2){
            float xt = x1, yt = y1;
            x1 = x2;y1 = y2;
            x2 = xt;y2 = yt;
        }
        if(y0 > y1){
            float xt = x0, yt = y0;
            x0 = x1;y0 = y1;
            x1 = xt;y1 = yt;
        }
        if(y1 == y2){
            KSDrawTriangleFlatBottom(dest, x0, y0, x1, x2, y1, pixel);
        }
        else if(y0 == y1){
            KSDrawTriangleFlatTop(dest, x2, y2, x0, x1, y0, pixel);
        }
        else{
            float x3 = (x0 + ((y1 - y0) / (y2 - y0)) * (x2 - x0));
            KSDrawTriangleFlatBottom(dest, x0, y0, x1, x3, y1, pixel);
            KSDrawTriangleFlatTop(dest, x2, y2, x1, x3, y1+1, pixel);
            KSDrawLinef(dest, x1, y1, x3, y1, pixel);
        }
        KSDrawLinef(dest, x0, y0, x1, y1, pixel);
        KSDrawLinef(dest, x0, y0, x2, y2, pixel);
        KSDrawLinef(dest, x2, y2, x1, y1, pixel);
    }*/
    
    void KSDrawTriangle(ksprite_t* dest, float x0, float y0, float x1, float y1, float x2, float y2, uint32_t pixel){
        // Sort vertices vertically so v0y <= v1y <= v2y
        if(y0 > y1){
            KSSwap(float, x0, x1);
            KSSwap(float, y0, y1);
        }
        if(y1 > y2){
            KSSwap(float, x1, x2);
            KSSwap(float, y1, y2);
        }
        if(y0 > y1){
            KSSwap(float, x0, x1);
            KSSwap(float, y0, y1);
        }
        if(y1>y0){
            for(int y = y0; y <= y1; ++y){
                float x01 = x0+(x1-x0)*((KSMin(KSMax((float)y,y0),y1)-y0)/(y1-y0));
                float x02 = x0+(x2-x0)*((KSMin(KSMax((float)y,y0),y1)-y0)/(y2-y0));
                KSScanLine(dest, y, x01, x02, pixel);
            }
        }
        if(y2>y1){
            for(int y = y1; y <= y2; ++y){
                float x02 = x0+(x2-x0)*((KSMin(KSMax((float)y,y1),y2)-y0)/(y2-y0));
                float x12 = x1+(x2-x1)*((KSMin(KSMax((float)y,y1),y2)-y1)/(y2-y1));
                KSScanLine(dest, y, x02, x12, pixel);
            }
        }
    }
    
    void KSToGreyScale(ksprite_t* sprite) {
        uint8_t *pixel_it = (uint8_t*)sprite->pixels;
        uint8_t* last_pixel = pixel_it + sprite->w*sprite->h*4;
        uint32_t r, g, b, grey;
        while(pixel_it < last_pixel) {
            b = *pixel_it;
            g = *(pixel_it+1);
            r = *(pixel_it+2);
            grey = (b+g+r)/3;
            *pixel_it = *(pixel_it+1) = *(pixel_it+2) = grey;
            pixel_it += 4;
        }
    }
    
    typedef struct{
        int w, h;
        uint8_t* pixels;
    } KMask;
    
    void KMCreate(KMask* mask, int w, int h, uint8_t pixel){
        mask->w = w;
        mask->h = h;
        mask->pixels = (uint8_t*)malloc(w*h);
        memset(mask->pixels, pixel, w*h);
    }
    
    void KMClear(KMask* mask){
        memset(mask->pixels, 0xff, mask->w*mask->h);
    }
    
    void KMSetPixel(KMask* mask, int x, int y, uint8_t pixel){
        if(x < 0 || x > mask->w-1 || y < 0 || y > mask->h-1) return;
        *(mask->pixels + y*mask->w + x) = pixel;
    }
    
    void KSBlitMasked(ksprite_t* source, ksprite_t* dest, KMask* mask, int spritex, int spritey, int maskx, int masky, void(*PixelFunc)(ksprite_t*, int, int, uint32_t)){
        int left_clip = KSMax(0, KSMax(-spritex, -maskx));
        int right_clip = KSMax(0, KSMax(spritex+source->w-dest->w, maskx+mask->w-dest->w));
        int top_clip = KSMax(0, KSMax(-spritey, -masky));
        int bottom_clip = KSMax(0, KSMax(spritey+source->h-dest->h, masky+mask->h-dest->h));
        for(int sourcey = top_clip; sourcey < source->h - bottom_clip; ++sourcey){
            for(int sourcex = left_clip; sourcex < source->w - right_clip; ++sourcex){
                uint32_t pixel = KSGetPixel(source, sourcex, sourcey);
                pixel = ((pixel<<8)>>8) + (mask->pixels[sourcey*mask->w+sourcex]<<24);
                PixelFunc(dest, spritex+sourcex, spritey+sourcey, pixel);
            }
        }
    }
    
    void KSBlitMask(KMask* mask, ksprite_t* dest, int destx, int desty, void(*PixelFunc)(ksprite_t*, int, int, uint32_t)){
        int left = KSMax(0, destx);
        int right = KSMin(dest->w-1, destx + mask->w-1);
        int top = KSMax(0, desty);
        int bottom = KSMin(dest->h-1, desty + mask->h-1);
        for(int y = top; y < bottom; ++y){
            for(int x = left; x < right; ++x){
                uint32_t pixel  = mask->pixels[(y-top)*mask->w + (x-left)];
                PixelFunc(dest, x, y, pixel + (pixel<<8) + (pixel<<16) + (0xff<<24));
            }
        }
    }
    
    void KMScanLine(KMask* dest, int y, int x0, int x1, uint8_t pixel){
        if(y < 0 || y > dest->h-1)return;
        int left = KSMax(0, KSMin(dest->w-1, KSMin(x0, x1)));
        int right = KSMax(0, KSMin(dest->w-1, KSMax(x0, x1)));
        memset(dest->pixels+(y*dest->w+left), pixel, right-left);
    }
    
    void KMDrawLinef(KMask* dest, float x0, float y0, float x1, float y1, uint8_t pixel){
        float length = KSLineLength(x0, y0, x1, y1);
        float dx = (x1-x0)/length;
        float dy = (y1-y0)/length;
        for(int d = 0; d < length; ++d){
            KMSetPixel(dest, x0 + (float)d * dx, y0 + (float)d * dy, pixel);
        }
    }
    
    void KMDrawTriangleFlatBottom(KMask* dest, float x, float y, float left, float right, float bottom, uint8_t pixel){
        float dl = left-x;
        float dr = right-x;
        float height = bottom-y;
        for(float liney = y; liney <= bottom; ++liney){
            float height_ratio = (liney-y)/height;
            KMScanLine(dest, liney, x+dl*height_ratio, x+dr*height_ratio, pixel);
        }
    }
    
    void KMDrawTriangleFlatTop(KMask* dest, float x, float y, float left, float right, float top, uint8_t pixel){
        float dl = left-x;
        float dr = right-x;
        float height = top-y;
        for(float liney = top; liney <= y; ++liney){
            float height_ratio = (liney-y)/height;
            KMScanLine(dest, liney, x+dl*height_ratio, x+dr*height_ratio, pixel);
        }
    }
    
    
    void KMDrawTriangle(KMask* dest, float x0, float y0, float x1, float y1, float x2, float y2, uint8_t pixel){
        // Sort vertices vertically so v0y <= v1y <= v2y
        if(y0 > y1){
            float xt = x0, yt = y0;
            x0 = x1; y0 = y1;
            x1 = xt; y1 = yt;
        }
        if(y1 > y2){
            float xt = x1, yt = y1;
            x1 = x2; y1 = y2;
            x2 = xt; y2 = yt;
        }
        if(y0 > y1){
            float xt = x0, yt = y0;
            x0 = x1; y0 = y1;
            x1 = xt; y1 = yt;
        }
        if(y1>y0){
            for(int y = y0; y <= y1; ++y){
                float x01 = x0+(x1-x0)*((KSMin(KSMax((float)y,y0),y1)-y0)/(y1-y0));
                float x02 = x0+(x2-x0)*((KSMin(KSMax((float)y,y0),y1)-y0)/(y2-y0));
                KMScanLine(dest, y, KSMin(x01,x02), KSMax(x01,x02), pixel);
            }
        }
        if(y2>y1){
            for(int y = y1; y <= y2; ++y){
                float x02 = x0+(x2-x0)*((KSMin(KSMax((float)y,y1),y2)-y0)/(y2-y0));
                float x12 = x1+(x2-x1)*((KSMin(KSMax((float)y,y1),y2)-y1)/(y2-y1));
                KMScanLine(dest, y, KSMin(x02,x12), KSMax(x02,x12), pixel);
            }
        }
    }
    
#ifdef __cplusplus
}
#endif

#define KERO_SPRITE_H
#endif
