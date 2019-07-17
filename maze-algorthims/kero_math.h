#if !defined(KERO_MATH_H)

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdlib.h>
    
#define Max(a, b) ((a)>(b) ? (a) : (b))
#define Min(a, b) ((a)<(b) ? (a) : (b))
#define Absolute(a) ((a)>0 ? (a) : (-(a)))
#define Square(a) ((a)*(a))
#define Sign(a) ((a)<0 ? -1 : 1)
#define Randf(a) ((float)rand() / (float)RAND_MAX * a)
    
    static inline float Power(float a, int p) {
        if(p == 0) return 1.f;
        float orig = a;
        for(int i = 1; i < p; ++i) {
            a *= orig;
        }
        return a;
    }
    
    static inline int Poweri(float a, int p) {
        if(p == 0) return 1;
        for(int i = 1; i < p; ++i) {
            a *= a;
        }
        return a;
    }
    
    static inline float SquareRoot(float a)
    {
        float precision = a*0.00001f;
        float root = a;
        while (Absolute(root - a/root) > precision)
        {
            root= (root + a/root) / 2;
        }
        return root;
    }
    
    static inline double SquareRootd(double a)
    {
        double precision = a*0.00001;
        double root = a;
        while ((root - a/root) > precision)
        {
            root= (root + a/root) / 2;
        }
        return root;
    }
    
#define LineLength(ax,ay,bx,by) SquareRoot(Square((float)(bx)-(float)(ax))+Square((float)(by)-(float)(ay)))
    
#define PI 3.14159265359f
#define TWOPI 6.28318530718
#define HALFPI 1.570796327f
    
    static inline int Roundf(float a){ return (int)Sign(a)*(Absolute(a)+0.5f); }
    static inline int Roundd(double a){ return (int)Sign(a)*(Absolute(a)+0.5); }
    
    static inline float Lerp(float a, float b, float t) {
        return Absolute(b-a)<0.005f?b:(a + (b-a)*t);
    }
    
#ifdef __cplusplus
}
#endif

#define KERO_MATH_H
#endif