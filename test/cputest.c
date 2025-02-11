#include "config.h"
#include "frame.h"
#include <string.h>
#include "init.h"
#include <immintrin.h>
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "sprite.h"
#include "text.h"
#include "pthread.h"

#define NUM_THREADS 8

typedef struct {
    size_t size;
    char* data;
    int offset;
    sm_vec2d center;
    double zoom;
    sm_vec2d constant;
} thread_data;

static const int iterations = 300;

void* render_thread_SIMD_ship(void* argp) {
    thread_data* d = argp;

    size_t size = d->size;
    char* data = d->data;
    double cx = -d->center.x;
    double cy = -d->center.y;
    double zoom = -d->zoom;
    int offset = d->offset;
    //four wide
    
    __m256d coordscale = _mm256_set1_pd(1000);
    __m256d centerx = _mm256_set1_pd(cx);
    __m256d centery = _mm256_set1_pd(cy);

    __m256d one = _mm256_set1_pd(1);

    __m256d zoomscale = _mm256_set1_pd(zoom);
    __m256d offset_c = _mm256_set1_pd(0.5);

    __m256d threshhold = _mm256_set1_pd(4.0);

    __m256d constantx = _mm256_set1_pd(d->constant.x);
    __m256d constanty = _mm256_set1_pd(d->constant.y);
    __m256d iterscale = _mm256_set1_pd(1.0/iterations);

    __m256d minusone = _mm256_set1_pd(-1);
    
    for (int i = offset; i < size/3; i += NUM_THREADS * 4) {
        __m256d indicies = _mm256_set_pd(i + 3, i + 2, i + 1, i + 0);

        //get components
        indicies = _mm256_div_pd(indicies, coordscale);
        __m256d yvals = _mm256_round_pd(indicies, _MM_FROUND_TRUNC);
        __m256d xvals = _mm256_sub_pd(indicies, yvals); 
        yvals = _mm256_div_pd(yvals, coordscale);


        xvals = _mm256_sub_pd(xvals, offset_c);
        yvals = _mm256_sub_pd(yvals, offset_c);


        xvals = _mm256_mul_pd(xvals, zoomscale);
        yvals = _mm256_mul_pd(yvals, zoomscale);

        xvals = _mm256_sub_pd(xvals, centerx);
        yvals = _mm256_sub_pd(yvals, centery);

        __m256d valid = _mm256_setzero_pd();

        __m256d iters = _mm256_set1_pd(0);

        __m256d zx = constantx;
        __m256d zy = constanty;
        for (int j = 0; j < iterations; j++) {
            zx = _mm256_max_pd(zx, _mm256_mul_pd(zx, minusone));
            zy = _mm256_max_pd(zy, _mm256_mul_pd(zy, minusone));

            __m256d x2 = _mm256_mul_pd(zx, zx);
            __m256d y2 = _mm256_mul_pd(zy, zy);
            __m256d xy = _mm256_mul_pd(zx, zy);

            zx = _mm256_sub_pd(_mm256_sub_pd(x2, y2), xvals);
            zy = _mm256_sub_pd(_mm256_add_pd(xy, xy), yvals);

            __m256d cmp = _mm256_cmp_pd(_mm256_add_pd(x2,y2), threshhold, _CMP_LT_OS);
            iters = _mm256_add_pd(_mm256_and_pd(cmp, one), iters);

            if (_mm256_testz_pd(cmp, _mm256_set1_pd(-1))) {
                break;
            }
        }

        double results[4];

        iters = _mm256_mul_pd(iters, iterscale);
        _mm256_storeu_pd(results, iters);


        for (int j = 0; j < 4; j++) {
            double a = results[j];
            double b = 1-a; 
            if (results[j] >= 1.0) {
                data[(i+j)*3 + 0] = 0;
                data[(i+j)*3 + 1] = 0;
                data[(i+j)*3 + 2] = 0;
            } else {
                data[(i+j)*3 + 0] = (char)(255 * a) + (100 * b);
                data[(i+j)*3 + 1] = (char)(55 * a) + (200 * b);
                data[(i+j)*3 + 2] = (char)(55 * a) + (200 * b);
            }
        }
    }
    return NULL;
}

void* render_thread_SIMD_mandel(void* argp) {
    thread_data* d = argp;

    size_t size = d->size;
    char* data = d->data;
    double cx = d->center.x;
    double cy = d->center.y;
    double zoom = d->zoom;
    int offset = d->offset;
    //four wide
    
    __m256d coordscale = _mm256_set1_pd(1000);
    __m256d centerx = _mm256_set1_pd(cx);
    __m256d centery = _mm256_set1_pd(cy);

    __m256d one = _mm256_set1_pd(1);

    __m256d zoomscale = _mm256_set1_pd(zoom);
    __m256d offset_c = _mm256_set1_pd(0.5);

    __m256d threshhold = _mm256_set1_pd(4.0);

    __m256d constantx = _mm256_set1_pd(d->constant.x);
    __m256d constanty = _mm256_set1_pd(d->constant.y);
    __m256d iterscale = _mm256_set1_pd(1.0/iterations);

    for (int i = offset; i < size/3; i += NUM_THREADS * 4) {
        __m256d indicies = _mm256_set_pd(i + 3, i + 2, i + 1, i + 0);

        //get components
        indicies = _mm256_div_pd(indicies, coordscale);
        __m256d yvals = _mm256_round_pd(indicies, _MM_FROUND_TRUNC);
        __m256d xvals = _mm256_sub_pd(indicies, yvals); 
        yvals = _mm256_div_pd(yvals, coordscale);


        xvals = _mm256_sub_pd(xvals, offset_c);
        yvals = _mm256_sub_pd(yvals, offset_c);


        xvals = _mm256_mul_pd(xvals, zoomscale);
        yvals = _mm256_mul_pd(yvals, zoomscale);

        xvals = _mm256_sub_pd(xvals, centerx);
        yvals = _mm256_sub_pd(yvals, centery);

        __m256d valid = _mm256_setzero_pd();

        __m256d iters = _mm256_set1_pd(0);

        __m256d zx = constantx;
        __m256d zy = constanty;
        for (int j = 0; j < iterations; j++) {
            __m256d x2 = _mm256_mul_pd(zx, zx);
            __m256d y2 = _mm256_mul_pd(zy, zy);
            __m256d xy = _mm256_mul_pd(zx, zy);

            zx = _mm256_sub_pd(_mm256_sub_pd(x2, y2), xvals);
            zy = _mm256_sub_pd(_mm256_add_pd(xy, xy), yvals);

            __m256d cmp = _mm256_cmp_pd(_mm256_add_pd(x2,y2), threshhold, _CMP_LT_OS);
            iters = _mm256_add_pd(_mm256_and_pd(cmp, one), iters);

            if (_mm256_testz_pd(cmp, _mm256_set1_pd(-1))) {
                break;
            }
        }

        double results[4];

        iters = _mm256_mul_pd(iters, iterscale);
        _mm256_storeu_pd(results, iters);


        for (int j = 0; j < 4; j++) {
            double a = results[j];
            double b = 1-a; 
            if (results[j] >= 1.0) {
                data[(i+j)*3 + 0] = 0;
                data[(i+j)*3 + 1] = 0;
                data[(i+j)*3 + 2] = 0;
            } else {
                data[(i+j)*3 + 0] = (char)(255 * a) + (100 * b);
                data[(i+j)*3 + 1] = (char)(55 * a) + (200 * b);
                data[(i+j)*3 + 2] = (char)(55 * a) + (200 * b);
            }
        }
    }
    return NULL;
}

void* render_thread_SIMD_julia(void* argp) {
    thread_data* d = argp;

    size_t size = d->size;
    char* data = d->data;
    double cx = d->center.x;
    double cy = d->center.y;
    double zoom = d->zoom;
    int offset = d->offset;
    //four wide
    
    __m256d coordscale = _mm256_set1_pd(1000);
    __m256d centerx = _mm256_set1_pd(cx);
    __m256d centery = _mm256_set1_pd(cy);

    __m256d mult2 = _mm256_set1_pd(2);
    __m256d one = _mm256_set1_pd(1);

    __m256d zoomscale = _mm256_set1_pd(zoom);
    __m256d offset_c = _mm256_set1_pd(0.5);

    __m256d threshhold = _mm256_set1_pd(4.0);

    __m256d constantx = _mm256_set1_pd(d->constant.x);
    __m256d constanty = _mm256_set1_pd(d->constant.y);
    __m256d iterscale = _mm256_set1_pd(1.0/iterations);

    for (int i = offset; i < size/3; i += NUM_THREADS * 4) {
        __m256d indicies = _mm256_set_pd(i + 3, i + 2, i + 1, i + 0);

        //get components
        indicies = _mm256_div_pd(indicies, coordscale);
        __m256d yvals = _mm256_round_pd(indicies, _MM_FROUND_TRUNC);
        __m256d xvals = _mm256_sub_pd(indicies, yvals); 
        yvals = _mm256_div_pd(yvals, coordscale);


        xvals = _mm256_sub_pd(xvals, offset_c);
        yvals = _mm256_sub_pd(yvals, offset_c);


        xvals = _mm256_mul_pd(xvals, zoomscale);
        yvals = _mm256_mul_pd(yvals, zoomscale);

        xvals = _mm256_sub_pd(xvals, centerx);
        yvals = _mm256_sub_pd(yvals, centery);

        __m256d valid = _mm256_setzero_pd();

        __m256d iters = _mm256_set1_pd(0);
        for (int j = 0; j < iterations; j++) {
            __m256d x2 = _mm256_mul_pd(xvals, xvals);
            __m256d y2 = _mm256_mul_pd(yvals, yvals);
            __m256d xy = _mm256_mul_pd(xvals, yvals);

            xvals = _mm256_sub_pd(_mm256_sub_pd(x2, y2), constantx);
            yvals = _mm256_sub_pd(_mm256_add_pd(xy, xy), constanty);

            __m256d cmp = _mm256_cmp_pd(_mm256_add_pd(x2,y2), threshhold, _CMP_LT_OS);
            iters = _mm256_add_pd(_mm256_and_pd(cmp, one), iters);

            if (_mm256_testz_pd(cmp, _mm256_set1_pd(-1))) {
                break;
            }
        }

        double results[4];

        iters = _mm256_mul_pd(iters, iterscale);
        _mm256_storeu_pd(results, iters);


        for (int j = 0; j < 4; j++) {
            double a = results[j];
            double b = 1-a; 
            if (results[j] >= 1.0) {
                data[(i+j)*3 + 0] = 0;
                data[(i+j)*3 + 1] = 0;
                data[(i+j)*3 + 2] = 0;
            } else {
                data[(i+j)*3 + 0] = (char)(0 * a) + (80 * b);
                data[(i+j)*3 + 1] = (char)(255 * a) + (0 * b);
                data[(i+j)*3 + 2] = (char)(255 * a) + (80 * b);
            }
        }
    }
    return NULL;
}

void* render_thread(void* argp) {
    thread_data* d = argp;

    size_t size = d->size;
    char* data = d->data;
    sm_vec2d center = d->center;
    double zoom = d->zoom;
    int offset = d->offset;
    for (int i = offset; i < size/3; i += NUM_THREADS) {
        double x = (double)((i) % 1000) / 1000.0;
        double y = (((double)(i)) / 1000) / 1000.0;

        x -= 0.5;
        y -= 0.5;
        x *= 2.0f;
        y *= 2.0f;
        x *= zoom;
        y *= zoom;
        x -= center.x;
        y -= center.y;

        double px, py;
        float color = 255;
        for (int j = 0; j < 100; j++) {
            px = x;
            py = y;

            x = px * px - py * py;
            y = 2 * px * py;
            x -= 1;
            if (x*x + y*y >= 4) {
                color =  0 * (float)j; 
                color /= 50;
                break;
            }
        }

        //SR_LOG_DEB("c: %f %f", __real__ c, __imag__ c);
        data[3*i] = color * 0.9; 
        data[3*i+1] = color * 0.2;
        data[3*i+2] = color * 0.9;



    }
    //pthread_exit(0); 
    return NULL;
}

int main() {

    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS] = {0};
    SubPass passes[2] = {0};

    SpriteGetSubpass(passes, attachments, 0);
    TextGetSubpass(passes, attachments, SR_SPRITE_ATTACHMENT_NUM);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 2, attachments, 2));

    SpriteRenderer* r = calloc(1, sizeof(SpriteRenderer));
    CRASH_CALL(SpriteInit(r, &p.p, 0, (Camera){.pos = {0, 0}, .size = {WIDTH, HEIGHT}, .rotation = 0}, 1));
    //build multipass

    TextRenderer* t = calloc(1, sizeof(TextRenderer));
    CRASH_CALL(TextInit(t, "resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf",&p.p, 1))


    Texture textures[1] = {0};
    DynamicTexture tex[1] = {0};

    CRASH_CALL(CreateDynTexture(&tex[0], (TextureConfig){
                    .width = 1000,
                    .height = 1000,
                    .filter = VK_FILTER_LINEAR,
                    .format = VK_FORMAT_R8G8B8_SRGB,
                    .channels = 3,
                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .anisotropy = VK_FALSE,
                }));
//    CRASH_CALL(CreateDynTexture(&t[1], (TextureConfig){
//                    .width = 1000,
//                    .height = 1000,
//                    .filter = VK_FILTER_LINEAR,
//                    .format _mm256_shuffle_epi8= VK_FORMAT_R8G8B8_SRGB,
//                    .channels = 3,
//                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
//                    .anisotropy = VK_FALSE,
//                }));


    textures[0] = tex[0].t;    
//    textures[1] = t[1].t;
    CRASH_CALL(SetTextureSlots(r, textures, 1));


    SpriteHandle s1 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){WIDTH*2, HEIGHT*2}, 0, 0);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    sm_vec2d center = (sm_vec2d){0, 0};
    sm_vec2d constant = (sm_vec2d){0, 0};
    double zoom = 1.0f;
    int active = 0;


    BeginUpdateDynTexture(&tex[0]);
    thread_data args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i] = (thread_data){
            .offset = i*4,
            .data = tex[0].data,
            .size = tex[0].size,
            .center = center,
            .zoom = zoom,
            .constant = constant
        };
        pthread_create(&threads[i], NULL, render_thread_SIMD_mandel, &args[i]); 
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    EndUpdateDynTexture(&tex[0]);
    ClearText(t);
    double rendertimes[10] = {0};
    int perfcursor = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        bool rerender = FALSE;
        glfwPollEvents();


        SpriteEntry* s = GetSprite(r, s1);
        if (glfwGetKey(sr_context.w, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
            if (glfwGetKey(sr_context.w, GLFW_KEY_W) == GLFW_PRESS) {
                center.y += 0.05 * zoom;
                rerender = TRUE;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_A) == GLFW_PRESS) {
                center.x += 0.05 * zoom;
                rerender = TRUE;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_S) == GLFW_PRESS) {
                center.y -= 0.05 * zoom;
                rerender = TRUE;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_D) == GLFW_PRESS) {
                center.x -= 0.05 * zoom;
                rerender = TRUE;
            }
        } else {
            if (glfwGetKey(sr_context.w, GLFW_KEY_W) == GLFW_PRESS) {
                constant.y += 0.02 * zoom;
                rerender = TRUE;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_A) == GLFW_PRESS) {
                constant.x += 0.02 * zoom;
                rerender = TRUE;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_S) == GLFW_PRESS) {
                constant.y -= 0.02 * zoom;
                rerender = TRUE;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_D) == GLFW_PRESS) {
                constant.x -= 0.02 * zoom;
                rerender = TRUE;
            }
        }
        
        if (glfwGetKey(sr_context.w, GLFW_KEY_R) == GLFW_PRESS) {
            center = (sm_vec2d){0, 0};
            zoom = 2.0f;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_H) == GLFW_PRESS) {
            //s->size = sm_vec2_f32_mul(s->size, (sm_vec2f){0.999, 0.999});
            zoom *= 1.1f;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_J) == GLFW_PRESS) {
            //s->size = sm_vec2_f32_mul(s->size, (sm_vec2f){1.001, 1.001});
            zoom *= 0.9f;
            rerender = TRUE;
        }


        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        SpriteDrawFrame(r, &p, frameCounter);
        NextPass(&p, frameCounter);
        TextDrawFrame(t, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

        if (rerender) {
            double startperf = glfwGetTime();
            double endperf = 0;
            BeginUpdateDynTexture(&tex[0]);

            for (int i = 0; i < NUM_THREADS; i++) {
                args[i] = (thread_data){
                        .offset = i*4, //4 because of simd 4 doubles at a time
                        .data = tex[0].data,
                        .size = tex[0].size,
                        .center = center,
                        .zoom = zoom,
                        .constant = constant
                };
                pthread_create(&threads[i], NULL, render_thread_SIMD_mandel, &args[i]); 
            }
            for (int i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
            }
            endperf = glfwGetTime();

            char buffer[128];
            int size = snprintf(buffer, 128, "Frame Time: %.4f\nConstant: %.4f %.4f\nZoom: %.4f", 
                    endperf - startperf,
                    constant.x, constant.y,
                    zoom);
            ClearText(t);
            AppendText(t, buffer, size, (sm_vec2f){10,10}, 1);
            EndUpdateDynTexture(&tex[0]);
        }


    }

    DestroyDynTexture(&tex[0]);
    DestroyPresent(&p);
    SpriteDestroy(r);
    TextDestroy(t);
    free(t);
    free(r);
    DestroyVulkan();
    return 0;
}
