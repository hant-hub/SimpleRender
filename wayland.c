//borrowed from wayland-book.com
#include <bits/time.h>
#include <bits/types.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

static void randname(char *buf) {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	long r = ts.tv_nsec;
	for (int i = 0; i < 6; ++i) {
		buf[i] = 'A'+(r&15)+(r&16)*2;
		r >>= 5;
	}
}

static int create_shm_file(void) {
	int retries = 100;
	do {
		char name[] = "/wl_shm-XXXXXX";
		randname(name + sizeof(name) - 7);
		--retries;
		int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			shm_unlink(name);
			return fd;
		}
	} while (retries > 0 && errno == EEXIST);
	return -1;
}

int allocate_shm_file(size_t size) {
	int fd = create_shm_file();
	if (fd < 0)
		return -1;
	int ret;
	do {
		ret = ftruncate(fd, size);
	} while (ret < 0 && errno == EINTR);
	if (ret < 0) {
		close(fd);
		return -1;
	}
	return fd;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include "xdg_shell_protocol.h"

const int width = 800;
const int height = 600;
//assuming XRGB8888 which is 4 bytes
const int stride = width * 4;
const int pool_size = height * stride * 2;


typedef struct {
    //globals
    struct wl_display* display;
    struct wl_registry* reg;
    struct wl_compositor* wlc;
    struct wl_shm* shmem;

    struct xdg_wm_base* xdg_base;

    //objects
    struct wl_surface* wlsurface;
    struct xdg_surface* xdgsurface;
    struct xdg_toplevel* xdgtoplevel;

    struct wl_shm_pool* shm_pool;
    struct wl_buffer* buffers[2];
    int shfd;
    int curr_buf;
    uint8_t* data;

    uint32_t last_frame;
} WindowState;

static void release_buffer(void* data, struct wl_buffer* buf) {
    WindowState* win = data;
    win->curr_buf = (win->curr_buf + 1) % 2;
}

static const struct wl_buffer_listener wlbuf_l = {
    .release = release_buffer
};

static struct wl_buffer* draw_frame(WindowState* win) {
    printf("draw\n");
    const int offset = height * stride;
	struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    memset(&win->data[offset * win->curr_buf], (ts.tv_sec * 10) % 255, height * stride);

    //munmap(data, pool_size);
    return win->buffers[win->curr_buf];
}


static void xdg_configure(void* data, struct xdg_surface* surface, uint32_t serial) {
    WindowState* win = data;
    xdg_surface_ack_configure(surface, serial);

    struct wl_buffer* frame = draw_frame(win);
    wl_surface_attach(win->wlsurface, frame, 0, 0);
    wl_surface_commit(win->wlsurface);
}

static void xdg_ping(void* data, struct xdg_wm_base* xdg_base, uint32_t serial) {
    xdg_wm_base_pong(xdg_base, serial);
}


static const struct xdg_surface_listener xdgsurface_l = {
    .configure = xdg_configure 
};

static const struct xdg_wm_base_listener xdgwm_l = {
    .ping = xdg_ping
};

static void handle_global(void* data, struct wl_registry* reg, uint32_t name,
                          const char* interface, uint32_t version) {
    WindowState* state = data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) { 
        state->wlc = wl_registry_bind(reg, name, &wl_compositor_interface, 4);
    }
    if (strcmp(interface, wl_shm_interface.name) == 0) {
        state->shmem = wl_registry_bind(reg, name, &wl_shm_interface, 1);
    }
    if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        state->xdg_base = wl_registry_bind(reg, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(state->xdg_base, &xdgwm_l, data);
    }
}

static void handle_global_remove(void* data, struct wl_registry* reg, uint32_t name) {
}



static const struct wl_registry_listener reg_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove
};


int Wlmain(int argc, char* argv[]) {

    WindowState* win = &(WindowState){0};

    win->display = wl_display_connect(NULL);
    win->reg = wl_display_get_registry(win->display);

    if (!win->display) {
        printf("Failed to Connect\n");
        exit(-1);
    }
    printf("Connected\n");

    wl_registry_add_listener(win->reg, &reg_listener, win);
    wl_display_roundtrip(win->display);

    win->wlsurface = wl_compositor_create_surface(win->wlc);
    win->shfd = allocate_shm_file(pool_size); 
    win->data = mmap(NULL, pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, win->shfd, 0);

    win->shm_pool = wl_shm_create_pool(win->shmem, win->shfd, pool_size);
    
    for (int i = 0; i < 2; i++) {
        int offset = height * stride * i;
        win->buffers[i] = wl_shm_pool_create_buffer(win->shm_pool, offset, width, height, stride, WL_SHM_FORMAT_XRGB8888);
        wl_buffer_add_listener(win->buffers[i], &wlbuf_l, win);
    }



    win->xdgsurface = xdg_wm_base_get_xdg_surface(win->xdg_base, win->wlsurface);
    xdg_surface_add_listener(win->xdgsurface, &xdgsurface_l, win);


    win->xdgtoplevel = xdg_surface_get_toplevel(win->xdgsurface);
    xdg_toplevel_set_title(win->xdgtoplevel, "wayland test");
    wl_surface_commit(win->wlsurface);


    __USECONDS_T_TYPE dt = (1/60.0 * 1000000); 
    while (wl_display_dispatch(win->display)) {
        //usleep(dt);
    }

    wl_display_disconnect(win->display);
    return 0;
}
